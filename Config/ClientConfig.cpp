#include "ClientConfig.h"
#include "ConfigError.h"

#include "Poco/DOM/DOMParser.h"
#include "Poco/DOM/Document.h"
#include "Poco/DOM/AutoPtr.h"
#include "Poco/Exception.h"

namespace Config
{

ClientConfig ClientConfig::readConfigFromXML(const std::string& xml)
{
    std::string uniqueKey;
    unsigned int monitorInterval = 300u; // 5 minutes by default.
    std::string serverHost;
    unsigned short serverPort = 40080u; // port 40001 by default.

    try {
        Poco::XML::DOMParser parser;
        auto doc = parser.parseString(xml);
        auto root = doc->documentElement();
        if (!root || root->nodeName() != "client")
            throw ConfigError("This is not client XML configuration!");

        // uniqueKey is mandatory.
        auto uniqueKeyElement = root->getChildElement("uniqueKey");
        if (!uniqueKeyElement)
            throw ConfigError("uniqueKey is not found!");
        uniqueKey = uniqueKeyElement->innerText();

        // monitorInterval is optional.
        auto monitorIntervalElement = root->getChildElement("monitorInterval");
        if (monitorIntervalElement)
            monitorInterval = std::stoul(monitorIntervalElement->innerText());

        // serverHost is mandatory.
        auto serverHostElement = root->getChildElement("serverHost");
        if (!serverHostElement)
            throw ConfigError("serverHost is not found!");
        serverHost = serverHostElement->innerText();

        // serverPort is optional.
        auto serverPortElement = root->getChildElement("serverPort");
        if (serverPortElement) {
            serverPort = static_cast<unsigned short>(std::stoul(
                serverPortElement->innerText()));
        }

        return ClientConfig(uniqueKey, monitorInterval, serverHost, serverPort);
    }
    catch (Poco::Exception&) {
        throw ConfigError("Cannot read client XML configuration!");
    }
    catch (std::invalid_argument&) {
        throw ConfigError("Some configuration value is invalid!");
    }
}

} // Config namespace.