#include "MonitorRequestHandler.h"

#include "Config/ClientInfoConfig.h"
#include "Protocol/MessageError.h"
#include "Util/Util.h"

#include "Poco/Net/HTTPBasicCredentials.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"

#include <iostream>
#include <cassert>

namespace Server
{

namespace
{

void sendCreated(Poco::Net::HTTPServerResponse& resp)
{
    std::cout << "  Resposne 201" << std::endl;
    resp.setStatus(Poco::Net::HTTPResponse::HTTP_CREATED);
    resp.send();
}

void sendBadRequest(Poco::Net::HTTPServerResponse& resp,
                    const std::string& body = "")
{
    std::cout << "  Response 400 due to " << body << std::endl;
    resp.setStatus(Poco::Net::HTTPResponse::HTTP_BAD_REQUEST);
    std::ostream& out = resp.send();
    if (!body.empty()) {
        out << body << std::endl;
        out.flush();
    }
}

void sendUnauthorized(Poco::Net::HTTPServerResponse& resp)
{
    std::cout << "  Response 401" << std::endl;
    resp.setStatus(Poco::Net::HTTPResponse::HTTP_UNAUTHORIZED);
    resp.send();
}

void sendMethodNotAllowed(Poco::Net::HTTPServerResponse& resp)
{
    std::cout << "  Response 405" << std::endl;
    resp.setStatus(Poco::Net::HTTPResponse::HTTP_METHOD_NOT_ALLOWED);
    resp.send();
}

void sendInternalError(Poco::Net::HTTPServerResponse& resp,
                       const std::string& body)
{
    std::cout << "  Response 500 due to " << body << std::endl;
    resp.setStatus(Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
    std::ostream& out = resp.send();
    out << body << std::endl;
    out.flush();
}

// Utility function to extract the request body.
std::string extractBody(Poco::Net::HTTPServerRequest& req)
{
    std::istream& in = req.stream();
    std::string body(std::istreambuf_iterator<char>(in), {});

    return body;
}

// The function check and send an email to SMTP server
// if the monitored resources of the associated client are over the limits.
void checkLimitAndSendMail(const Config::ClientInfoConfig& clientInfo,
                           const Protocol::StatisticMessage& message,
                           Protocol::SMTPMailSender& mailSender)
{
    std::string subject;
    std::string content;

    if (message.getCPUUsage() > clientInfo.getCPULimit()) {
        subject += "CPU, ";
        content += "CPU is over the limit. ";
    }

    if (message.getMemoryUsage() > clientInfo.getMemoryLimit()) {
        subject += "Memory, ";
        content += "Memory is over the limit. ";
    }

    if (message.getProcessNumber() > clientInfo.getProcessesLimit()) {
        subject += "Process, ";
        content += "Number of process is over the limit. ";
    }

    // Some resources are over the limit.
    if (!subject.empty()) {
        std::cout << "  Found some resources over the limit!" << std::endl;
        assert(!content.empty());
        subject += " overloading!";
        mailSender.sendMail("notreply@mydomain.com",
            clientInfo.getMail(), subject, content);
    }
}

// Extract the key from the request credentail.
// Return empty string if the request does not contain the credentail.
std::string extractKeyFromRequest(const Poco::Net::HTTPServerRequest& req)
{
    if (!req.hasCredentials())
        return "";

    std::string key;
    Poco::Net::HTTPBasicCredentials credential(req);

    // In this system, we just simply use username.
    return credential.getUsername();
}

/**
  This handler is to just simply return 405 method not allowed to client
  for any request that is not POST operation.
*/
class DisallowedHandler : public Poco::Net::HTTPRequestHandler
{
public:
    void handleRequest(Poco::Net::HTTPServerRequest& req,
                       Poco::Net::HTTPServerResponse& resp) override;

};


} // anonymous namespace.

void ServerRequestHandler::operateReceivedMessage(const std::string& key,
                                                  const Protocol::StatisticMessage& message)
{
    try {
        // save into database.
        serverData_.saveStatistic(key, message.getCPUUsage(),
            message.getMemoryUsage(), message.getProcessNumber());

        // get the client info config from the database.
        std::string configPath = serverData_.getClientConfigPath(key);
        auto clientInfo = Config::ClientInfoConfig::readConfigFromXML(
            Util::readFromFile(configPath));

        // check the limit and send mail if needed.
        checkLimitAndSendMail(clientInfo, message, mailSender_);
    }
    catch (Database::DatabaseError& err) {
        std::cerr << err.what() << std::endl;
    }
    catch (Util::ReadFileError& err) {
        std::cerr << err.what() << std::endl;
    }
    catch (Protocol::SMTPException& err) {
        std::cerr << err.what() << std::endl;
    }
}

void ServerRequestHandler::handleRequest(Poco::Net::HTTPServerRequest& req,
                                         Poco::Net::HTTPServerResponse& resp)
{
    try {
        std::cout << "Got POST request" << std::endl;

        std::string contentType = req.getContentType();
        if (contentType != "application/xml") {
            sendBadRequest(resp, "invalid content-type");
            return;
        }

        // Check authentication here and extract key here.
        //std::string key = "client1";
        std::string key = extractKeyFromRequest(req);
        std::cout << "  Auth info: " << key << std::endl;
        if (!serverData_.clientExist(key)) {
            sendUnauthorized(resp);
            return;
        }

        // Parse the body
        std::string body = extractBody(req);
        auto message = Protocol::StatisticMessage::fromXML(body);
        
        // Response as 201 Created.
        sendCreated(resp);

        // Operate the received message;
        operateReceivedMessage(key, message);
    }
    catch (Protocol::MessageError& err) {
        sendBadRequest(resp, err.what());
    }
}

void DisallowedHandler::handleRequest(Poco::Net::HTTPServerRequest& req,
                                      Poco::Net::HTTPServerResponse& resp)
{
    // Just simply return 405 as this verb is disallowed.
    sendMethodNotAllowed(resp);
}


Poco::Net::HTTPRequestHandler*
ServerHandlerFactory::createRequestHandler(const Poco::Net::HTTPServerRequest& req)
{
    if (req.getMethod() == "POST") {
        return new ServerRequestHandler(serverData_, mailSender_);
    }
    else {
        std::cout << "Got " << req.getMethod() << " which is disallowed." << std::endl;
        return new DisallowedHandler();
    }
}

} // Server namespace.