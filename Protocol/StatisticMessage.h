#pragma once

#include <string>

namespace Protocol
{

/**
  This class represents the xml message which is used to convey
  monitored resuorces information from client to server.
*/
class StatisticMessage
{
public:
    /**
      Parse the message from the given xml.

      The method throw Protocol::MessageError if the given xml is invalid.
    */
    static StatisticMessage fromXML(const std::string& xml);

    /**
      Construt the object from the given monitored resources.
    */
    StatisticMessage(double cpuUsage,
                     double memoryUsage,
                     unsigned int processNumber)
        : cpuUsage_(cpuUsage),
          memoryUsage_(memoryUsage),
          processNumber_(processNumber)
    {
    }

    /**
      Construct the XML which represents the statistic message.

      This XML can be sent to the server.
    */
    std::string toXML() const;

    // Accessors.

    /**
      Get the CPU usage.

      The value is between 0.00 - 1.00.
    */
    double getCPUUsage() const
    {
        return cpuUsage_;
    }

    /**
      Get the memory usage.

      The value is between 0.00 - 1.00.
    */
    double getMemoryUsage() const
    {
        return memoryUsage_;
    }

    /**
      Get the number of runnig process.
    */
    unsigned int getProcessNumber() const
    {
        return processNumber_;
    }

private:
    // Properties.
    double cpuUsage_;
    double memoryUsage_;
    unsigned int processNumber_;
};

}
