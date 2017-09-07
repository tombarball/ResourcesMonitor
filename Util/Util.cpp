#include "Util.h"

#include <fstream>
#include <sstream>

namespace Util
{

std::string readFromFile(const std::string& filePath)
{
    std::ifstream inFile;
    inFile.open(filePath);
    if (!inFile.is_open())
        throw ReadFileError("Cannot read file at: " + filePath);

    std::stringstream ss;
    ss << inFile.rdbuf();
    return ss.str();
}

} // Util namespace.