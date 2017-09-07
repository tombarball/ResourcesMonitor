#pragma once

#include <string>
#include <map>
#include <stdexcept>

namespace Database
{

/**
  This exception can be thrown when there are some problems
  connecting to the database (SQLite 3).
*/
class DatabaseError : public std::runtime_error
{
public:
    DatabaseError(const std::string err)
        : std::runtime_error(err)
    {
    }
};

/**
  This class allow users to access the client information
  and save the statistic monitored resources from the database (SQLite 3).
*/
class ServerData
{
public:
    /**
      Construct the object with the given database file path (SQLite 3).
    */
    ServerData(const std::string& databasePath)
        : databasePath_(databasePath)
    {
        registerConnector();
    }

    /**
      Check whether the client from the given key does exist in the database.

      This method possibly throw Database::DatabaseError.
    */
    bool clientExist(const std::string& key);

    /**
      Get the client information configuration file path from the given client key.

      This method possibly throw Database::DatabaseError.
    */
    std::string getClientConfigPath(const std::string& key);
    void saveStatistic(const std::string& key,
                       double cpuUsage,
                       double memoryUsage,
                       unsigned int processNumber);

private:
    // To register the SQLite connector by the first time.
    static void registerConnector();

    std::string databasePath_;
    std::map<std::string, std::string> clientConfigPaths_;
};

}