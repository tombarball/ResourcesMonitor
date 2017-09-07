#pragma once

#include <stdexcept>

namespace Config
{

/**
  This exception can be thrown from reading configuration from xml.
*/
class ConfigError : public std::runtime_error
{
public:
    ConfigError(const std::string err)
        : std::runtime_error(err)
    {
    }
};

}
