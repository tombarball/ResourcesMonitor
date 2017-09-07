#pragma once

#include <stdexcept>

namespace Protocol
{

/**
  This exception can be thrown when parsing StatisticMessage from the given xml.
*/
class MessageError : public std::runtime_error
{
public:
    MessageError(const std::string err)
        : std::runtime_error(err)
    {
    }
};

}