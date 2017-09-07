#pragma once

#include <string>

namespace Config
{

/**
  This class represents the client information configuration at the server machine.
  The configuration should contain the alert information of the relevant client.

  User cannot instantiate it directly but need to call readConfigFromXML
  to construct the object.
*/
class ClientInfoConfig
{
public:
    /**
      Read the client information configuration from the given xml.

      The method throw Config::ConfigError if the given xml is invalid.
    */
    static ClientInfoConfig readConfigFromXML(const std::string& xml);

    // Accessors.

    /**
      Get the client key.
    */
    std::string getKey() const
    {
        return key_;
    }

    /**
      Get the client email address to send alert email when its monitored
      resource are over the limit
    */
    std::string getMail() const
    {
        return mail_;
    }

    /**
      Get the memory usage limit of this client.
    */
    double getMemoryLimit() const
    {
        return memoryLimit_;
    }

    /**
      Get the cpu usage limit of this client.
    */
    double getCPULimit() const
    {
        return cpuLimit_;
    }

    /**
      Get the number of process limit of this client.
    */
    unsigned int getProcessesLimit() const
    {
        return processesLimit_;
    }

private:
    ClientInfoConfig(const std::string& key,
                     const std::string& mail,
                     double memoryLimit,
                     double cpuLimit,
                     unsigned int processesLimit)
        : key_(key),
          mail_(mail),
          memoryLimit_(memoryLimit),
          cpuLimit_(cpuLimit),
          processesLimit_(processesLimit)
    {
    }

    // Properties.
    std::string key_;
    std::string mail_;
    double memoryLimit_;
    double cpuLimit_;
    unsigned int processesLimit_;
};

}