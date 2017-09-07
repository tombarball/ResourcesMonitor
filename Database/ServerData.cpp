#include "ServerData.h"

#include "Poco/Data/Session.h"
#include "Poco/Data/RecordSet.h"
#include "Poco/Data/SQLite/Connector.h"

#include <iomanip>
#include <sstream>

namespace Database
{

namespace
{

// Query client info configuration file path from the given database and client key.
std::string queryClientConfigPath(const std::string& databasePath,
                                  const std::string& key)
{
    try {
        // Create SQLite session.
        Poco::Data::Session session("SQLite", databasePath);
        Poco::Data::Statement select(session);

        // Query by SQL.
        std::string queryStr =
            "SELECT configPath from client WHERE key='" + key + "'";
        select << queryStr;
        select.execute();

        // Read the result.
        Poco::Data::RecordSet rs(select);
        if (!rs.moveFirst())
            throw DatabaseError("Not found client: " + key + " in the database!");

        return rs[0].convert<std::string>();
    }
    catch (Poco::Exception&) {
        throw DatabaseError("Cannot query client config for some reason!");
    }
}

// Utility function to convert from double to string with two digits.
std::string doubleToString(double value)
{
    std::ostringstream oss;

    oss << std::fixed << std::setprecision(2) << value;
    return oss.str();
}

// Insert statistic monitored resources into the database.
void insertStatisticToDB(const std::string& databasePath,
                         const std::string& key,
                         double cpuUsage,
                         double memoryUsage,
                         unsigned int processNumber)
{
    try {
        // Create SQLite session
        Poco::Data::Session session("SQLite", databasePath);
        Poco::Data::Statement select(session);

        // Query by SQL.
        std::string insertStr =
            "INSERT INTO statistic (clientKey, cpuUsage, memoryUsage, processNumber) "
            "VALUES('" + key + "', " +
            doubleToString(cpuUsage) + ", " +
            doubleToString(memoryUsage) + ", " +
            std::to_string(processNumber) + ")";
        session << insertStr, Poco::Data::Keywords::now;
    }
    catch (Poco::Exception&) {
        throw DatabaseError("Cannot insert statistic data for some reason!");
    }
}

} // anonymous namespace.

void ServerData::registerConnector()
{
    static bool registered = false;
    if (registered)
        return;

    Poco::Data::SQLite::Connector::registerConnector();
    registered = true;
}

bool ServerData::clientExist(const std::string& key)
{
    // Search from the cache first to avoid accessing database many times.
    auto it = clientConfigPaths_.find(key);
    if (it != clientConfigPaths_.end())
        return true;

    try {
        std::string path = queryClientConfigPath(databasePath_, key);
        clientConfigPaths_.emplace(key, path);

        return true;
    }
    catch (DatabaseError&) {
        return false;
    }
}

std::string ServerData::getClientConfigPath(const std::string& key)
{
    // Search from the cache first to avoid accessing database many times.
    auto it = clientConfigPaths_.find(key);
    if (it != clientConfigPaths_.end())
        return it->second;

    std::string path = queryClientConfigPath(databasePath_, key);
    clientConfigPaths_.emplace(key, path);

    return path;
}

void ServerData::saveStatistic(const std::string& key,
                               double cpuUsage,
                               double memoryUsage,
                               unsigned int processNumber)
{
    insertStatisticToDB(databasePath_, key, cpuUsage, memoryUsage, processNumber);
}

} // Database namespace.