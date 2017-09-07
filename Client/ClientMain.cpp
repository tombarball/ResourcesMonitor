#include "Config/ClientConfig.h"
#include "Config/ConfigError.h"
#include "ResourceMonitor/ResourceMonitor.h"
#include "Protocol/StatisticMessage.h"
#include "Util/Util.h"

#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPBasicCredentials.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"

#include <iostream>
#include <sstream>
#include <Windows.h>

namespace Client
{

namespace
{

// This function could thrown the following exceptions.
// - Util::ReadFileError
// - Config::ConfigError
Config::ClientConfig readClientConfig()
{
    // Just hardcode the client configuration file name by the first version.
    std::string xml = Util::readFromFile("client.xml");
    return Config::ClientConfig::readConfigFromXML(xml);
}

} // anonymous namespace.

/**
  This exception can be thrown when there are some error
  from doing HTTP request to the server.
*/
class HTTPResourceSenderError : public std::runtime_error
{
public:
    HTTPResourceSenderError(const std::string& error)
        : std::runtime_error(error)
    {
    }
};

/**
  This class allow user to send monitored resource information to the server
  for the given client unique key.
*/
class HTTPResourceSender
{
public:
    /**
      Create the sender with the given client unique key, server host/port.
    */
    HTTPResourceSender(const std::string& key,
                       const std::string& serverHost,
                       unsigned short serverPort)
        : key_(key),
          serverHost_(serverHost),
          serverPort_(serverPort)
    {
    }

    /**
      Send the monitored resources information to the server.
      The XML body will be constructed from the basic resources information.
    */
    void sendMonitoredResources(double cpuUsage,
                                double memoryUsage,
                                unsigned int processNumber)
    {
        // Create message to send.
        Protocol::StatisticMessage message(cpuUsage, memoryUsage, processNumber);
        sendHTTPPostRequest(message.toXML());
    }

private:
    void sendHTTPPostRequest(const std::string& xmlBody)
    {
        try {
            std::cout << "Sending: " << key_ << std::endl;
            std::cout << xmlBody << std::endl << std::endl;

            // Client session.
            Poco::Net::HTTPClientSession session(serverHost_, serverPort_);

            // Request with authentication information.
            Poco::Net::HTTPRequest req(Poco::Net::HTTPRequest::HTTP_POST, "/",
                Poco::Net::HTTPMessage::HTTP_1_1);
            Poco::Net::HTTPBasicCredentials credential(key_, "");
            credential.authenticate(req);
            req.setContentType("application/xml");
            req.setContentLength(xmlBody.size());

            // Send request with body.
            std::ostream& out = session.sendRequest(req);
            out << xmlBody;

            // Read response.
            Poco::Net::HTTPResponse resp;
            session.receiveResponse(resp);
            if (resp.getStatus() != Poco::Net::HTTPResponse::HTTP_CREATED) {
                std::ostringstream error;
                error << "Got response: " << resp.getStatus();
                throw HTTPResourceSenderError(error.str());
            }
            else {
                std::cout << "Got 201 Created" << std::endl;
            }
        }
        catch (Poco::Exception&) {
            throw HTTPResourceSenderError("Cannot send data for some reason!");
        }
    }

    std::string key_;
    std::string serverHost_;
    unsigned short serverPort_;
};

void startMonitoring()
{
    try {
        // read client.xml
        auto config = readClientConfig();
        HTTPResourceSender sender(config.getUniqueKey(),
            config.getServerHost(), config.getServerPort());

        // Start looping to peak the statistic resource information
        // and send them to the server.
        while (true) {
            try {
                // Just simply use sleep method.
                // *NOTE*, for testing, the monitor interval should not be
                // longer than a minute.
                Sleep(config.getMonitorInterval() * 1000);

                // Get the monitor resources.
                double cpuUsage = ResourceMonitor::getCPUUsage();
                double memoryUsage = ResourceMonitor::getMemoryUsage();
                unsigned int processNumber = ResourceMonitor::getProcessNumber();
                sender.sendMonitoredResources(cpuUsage, memoryUsage, processNumber);
            }
            catch (ResourceMonitor::ResourceMonitorError& err) {
                std::cout << err.what() << std::endl;
            }
            catch (HTTPResourceSenderError& err) {
                std::cout << err.what() << std::endl;
            }
        }
    }
    catch (Util::ReadFileError& err) {
        std::cerr << err.what() << std::endl;
        return;
    }
    catch (Config::ConfigError& err) {
        std::cerr << err.what() << std::endl;
        return;
    }
}

} // Client namespace.

int main()
{
    std::cout << "Start monitoring ..." << std::endl;
    Client::startMonitoring();

    return 0;
}