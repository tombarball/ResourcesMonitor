#include "ServerConfig.h"
#include "ConfigError.h"

#include "Poco/DOM/DOMParser.h"
#include "Poco/DOM/Document.h"
#include "Poco/DOM/AutoPtr.h"
#include "Poco/Exception.h"

namespace Config
{

// Extract the HTTP configuration.
// This configuration is optional.
void extractHTTPConfig(const Poco::XML::Element* root,
                       HTTPConfig& httpConfig)
{
    auto httpElement = root->getChildElement("HTTPConfig");
    if (!httpElement)
        return;

    // port is optional.
    auto portElement = httpElement->getChildElement("port");
    if (portElement)
        httpConfig.httpPort = static_cast<unsigned short>(
            std::stoul(portElement->innerText()));
}

// Extract the SMTP configuration.
void extractSMTPConfig(const Poco::XML::Element* root,
                       SMTPConfig& smtpConfig)
{
    auto smtpElement = root->getChildElement("SMTPConfig");
    if (!smtpElement)
        throw ConfigError("SMTP configuration is not found!");

    // host is mandatory.
    auto hostElement = smtpElement->getChildElement("host");
    if (!hostElement)
        throw ConfigError("No host configured for SMTP configuration!");
    smtpConfig.smtpHost = hostElement->innerText();

    // port is optional.
    auto portElement = smtpElement->getChildElement("port");
    if (portElement)
        smtpConfig.smtpPort = static_cast<unsigned short>(
            std::stoul(portElement->innerText()));
}

// Extract the Database (SQLite) configuration.
void extractDBConfig(const Poco::XML::Element* root,
                     DBConfig& dbConfig)
{
    auto dbElement = root->getChildElement("DatabaseConfig");
    if (!dbElement)
        return;

    // path is optional. Fallback to default.db.
    auto pathElement = dbElement->getChildElement("path");
    if (pathElement)
        dbConfig.databasePath = pathElement->innerText();
}

ServerConfig ServerConfig::readConfigFromXML(const std::string& xml)
{
    HTTPConfig httpConfig;
    httpConfig.httpPort = 40080; // port 40080 by default.

    SMTPConfig smtpConfig;
    smtpConfig.smtpPort = 40025; // port 40025 by default.

    DBConfig dbConfig;
    dbConfig.databasePath = "default.db"; // default.db at the executed path by default.

    try {
        Poco::XML::DOMParser parser;
        auto doc = parser.parseString(xml);
        auto root = doc->documentElement();
        if (!root || root->nodeName() != "server")
            throw ConfigError("This is not server XML configuration!");

        extractHTTPConfig(root, httpConfig);
        extractSMTPConfig(root, smtpConfig);
        extractDBConfig(root, dbConfig);

        return ServerConfig(httpConfig, smtpConfig, dbConfig);
    }
    catch (Poco::Exception&) {
        throw ConfigError("Cannot read server XML configuration!");
    }
    catch (std::invalid_argument&) {
        throw ConfigError("Some configuration value is invalid!");
    }
}

} // Config namespace.