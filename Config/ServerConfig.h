#pragma once

#include <string>

namespace Config
{

/**
  This struct represents the HTTP configuration.
*/
struct HTTPConfig
{
    unsigned short httpPort; // http port that the server listen to.
};

/**
  This struct represents the SMTP configuration.
*/
struct SMTPConfig
{
    std::string smtpHost; // smtp host that the server sends an alert email.
    unsigned short smtpPort; // smtp port that the server sends an alert email.
};

/**
  This struct represents the Database configuration (SQLite 3).
*/
struct DBConfig
{
    std::string databasePath; // path to the database file (SQLite 3).
};


/**
  This class represents the server configuration at the server machine.

  User cannot instantiate it directly but need to call readConfigFromXML
  to construct the object.
*/
class ServerConfig
{
public:
    /**
      Read the server configuration from the given xml.

      The method throw Config::ConfigError if the given xml is invalid.
    */
    static ServerConfig readConfigFromXML(const std::string& xml);

    // Accessors.

    /**
      Get the HTTP configuration.
    */
    const HTTPConfig& getHTTPConfig() const
    {
        return httpConfig_;
    }

    /**
      Get the SMTP configuration.
    */
    const SMTPConfig& getSMTPConfig() const
    {
        return smtpConfig_;
    }

    /**
      Get the Database configuration.
    */
    const DBConfig& getDBConfig() const
    {
        return dbConfig_;
    }

private:
    ServerConfig(const HTTPConfig& httpConfig,
                 const SMTPConfig& smtpConfig,
                 const DBConfig& dbConfig)
        : httpConfig_(httpConfig),
          smtpConfig_(smtpConfig),
          dbConfig_(dbConfig)
    {
    }

    // Properties.
    HTTPConfig httpConfig_;
    SMTPConfig smtpConfig_;
    DBConfig dbConfig_;
};

}
