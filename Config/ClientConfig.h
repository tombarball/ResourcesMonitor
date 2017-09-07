#pragma once

#include <string>

namespace Config
{

/**
  This class represents the client configuration at the client machine.

  User cannot instantiate it directly but need to call readConfigFromXML
  to construct the object.
*/
class ClientConfig
{
public:
    /**
      Read the client configuration from the given xml.

      The method throw Config::ConfigError if the given xml is invalid.
    */
    static ClientConfig readConfigFromXML(const std::string& xml);

    // Accessors.

    /**
      Get the unique key from the configuration.
    */
    std::string getUniqueKey() const
    {
        return uniqueKey_;
    }

    /**
      Get the monitor interval from the configuration

      The return value is in milliseconds.
    */
    unsigned int getMonitorInterval() const
    {
        return monitorInterval_;
    }

    /**
      Get the HTTP server host to send the resource information.
    */
    std::string getServerHost() const
    {
        return serverHost_;
    }

    /**
      Get the HTTP server port to send the resource information.
    */
    unsigned short getServerPort() const
    {
        return serverPort_;
    }

private:
    ClientConfig(const std::string& uniqueKey,
                 unsigned int monitorInterval,
                 const std::string& serverHost,
                 unsigned short serverPort)
        : uniqueKey_(uniqueKey),
          monitorInterval_(monitorInterval),
          serverHost_(serverHost),
          serverPort_(serverPort)
    {
    }

    // Properties.
    std::string uniqueKey_;
    unsigned int monitorInterval_;
    std::string serverHost_;
    unsigned short serverPort_;
};

}