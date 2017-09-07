#pragma once

#include <string>

namespace ResourceMonitor
{

/**
  The exception which could be thrown from the ResourceMonitor library.
*/
class ResourceMonitorError : public std::runtime_error
{
public:
    ResourceMonitorError(const std::string& desc)
        : std::runtime_error(desc)
    {
    }
};

/**
  Get CPU usage in percentage (the value is between 0.00 - 1.00).
*/
double getCPUUsage();

/**
  Get memory usage in percentage (the value is between 0.00 - 1.00).
*/
double getMemoryUsage();

/**
  Get the number of process which are running on the machine.

  This function possibly throw ResourceMonitor::ResourceMonitorError.
*/
unsigned long getProcessNumber();

}
