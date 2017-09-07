#pragma once

#include <string>
#include <stdexcept>

namespace Util
{

/**
  This excetion can be thrown when there are some error when reading a file.
*/
class ReadFileError : public std::runtime_error
{
public:
    ReadFileError(const std::string& error)
        : std::runtime_error(error)
    {
    }
};

/**
  Read file from the given filePath and return the content.

  This function possibly throw Util::ReadFileError.
*/
std::string readFromFile(const std::string& filePath);

}
