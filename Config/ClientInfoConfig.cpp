#include "ClientInfoConfig.h"
#include "ConfigError.h"

#include "Poco/DOM/DOMParser.h"
#include "Poco/DOM/Document.h"
#include "Poco/DOM/AutoPtr.h"
#include "Poco/DOM/NodeList.h"
#include "Poco/Exception.h"

#include <climits>

namespace Config
{

// Conver percentage (e.g. "80%") string to double.
// The function might throw std::invalid_argument for invalid percentage string.
double percentageToDouble(const std::string& percentage)
{
    unsigned int percentageNum = std::stoul(percentage);
    return percentageNum / 100.0;
}

ClientInfoConfig ClientInfoConfig::readConfigFromXML(const std::string& xml)
{
    std::string key;
    std::string mail;
    double memoryLimit = 1.0; // No limit by default.
    double cpuLimit = 1.0; // No limit by default.
    unsigned int processesLimit = UINT_MAX; // No limit by default.

    try {
        Poco::XML::DOMParser parser;
        auto doc = parser.parseString(xml);
        auto root = doc->documentElement();
        if (!root || root->nodeName() != "client")
            throw ConfigError("This is not clientinfo XML configuration!");

        // key and mail are mandatory.
        key = root->getAttribute("key");
        mail = root->getAttribute("mail");
        if (key.empty() || mail.empty())
            throw ConfigError("key or mail are not configured");

        // alert elements are optional.
        auto children = root->getElementsByTagName("alert");
        if (children && children->length() != 0u) {
            for (unsigned long ii = 0u; ii < children->length(); ii++) {
                auto child = children->item(ii);
                // Can this happen?
                if (child->nodeType() != Poco::XML::Node::ELEMENT_NODE)
                    continue;

                auto element = static_cast<Poco::XML::Element*>(child);
                std::string type = element->getAttribute("type");
                std::string limit = element->getAttribute("limit");
                if (type == "memory")
                    memoryLimit = percentageToDouble(limit);
                else if (type == "cpu")
                    cpuLimit = percentageToDouble(limit);
                else if (type == "processes")
                    processesLimit = std::stoul(limit);
                else
                    continue;
            }
        }

        return ClientInfoConfig(key, mail, memoryLimit, cpuLimit, processesLimit);
    }
    catch (Poco::Exception&) {
        throw ConfigError("Cannot read clientinfo XML configuration!");
    }
    catch (std::invalid_argument&) {
        throw ConfigError("Some configuration value is invalid!");
    }
}

} // Config namespace.