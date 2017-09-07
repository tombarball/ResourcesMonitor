#pragma once

#include "Config/ServerConfig.h"
#include "Database/ServerData.h"
#include "Protocol/SMTPMailSender.h"
#include "Protocol/StatisticMessage.h"

#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"

namespace Server
{
/**
  The main request handler that supports POST request.

  *NOTE*, by the first version, any client's request-url will be handled
   by this handler for simplicity.
*/
class ServerRequestHandler : public Poco::Net::HTTPRequestHandler
{
public:
    ServerRequestHandler(Database::ServerData& serverData,
                         Protocol::SMTPMailSender& mailSender)
        : serverData_(serverData),
          mailSender_(mailSender)
    {
    }

    void handleRequest(Poco::Net::HTTPServerRequest& req,
                       Poco::Net::HTTPServerResponse& resp) override;

private:
    // This is the operation that the handler manage the received message
    // which contain monitored resources informaton from clients.
    void operateReceivedMessage(const std::string& key,
                                const Protocol::StatisticMessage& message);

    // The life-time of this handler must be died before ServerHandlerFactory.
    Database::ServerData& serverData_;
    Protocol::SMTPMailSender& mailSender_;

};

class ServerHandlerFactory : public Poco::Net::HTTPRequestHandlerFactory
{
public:
    ServerHandlerFactory(const Config::DBConfig& dbConfig,
                         const Config::SMTPConfig& smtpConfig)
        : serverData_(dbConfig.databasePath),
          mailSender_(smtpConfig.smtpHost, smtpConfig.smtpPort)
    {
    }

    Poco::Net::HTTPRequestHandler*
    createRequestHandler(const Poco::Net::HTTPServerRequest& req) override;

private:
    Database::ServerData serverData_;
    Protocol::SMTPMailSender mailSender_;
};

}