#include "MonitorRequestHandler.h"
#include "Config/ServerConfig.h"
#include "Config/ConfigError.h"
#include "Util/Util.h"

#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/HTTPServer.h"
#include "Poco/Util/ServerApplication.h"

#include <iostream>
#include <vector>

namespace Server
{

namespace
{

Config::ServerConfig readServerConfig()
{
    std::string xml = Util::readFromFile("server.xml");
    return Config::ServerConfig::readConfigFromXML(xml);
}

} // anonymous namespace.

class MyServerApp : public Poco::Util::ServerApplication
{
protected:
    int main(const std::vector<std::string>&)
    {
        try {
            auto config = readServerConfig();
            auto handlerFactory = new ServerHandlerFactory(config.getDBConfig(), config.getSMTPConfig());
            Poco::Net::HTTPServer server(handlerFactory,
                Poco::Net::ServerSocket::ServerSocket(config.getHTTPConfig().httpPort),
                new Poco::Net::HTTPServerParams());

            server.start();
            std::cout << "Server started" << std::endl;

            waitForTerminationRequest();  // wait for CTRL-C or kill

            std::cout << "Shutting down..." << std::endl;
            server.stop();

            return Application::EXIT_OK;
        }
        catch (Util::ReadFileError& err) {
            std::cout << err.what() << std::endl;
            return Application::EXIT_DATAERR;
        }
        catch (Config::ConfigError& err) {
            std::cout << err.what() << std::endl;
            return Application::EXIT_DATAERR;
        }
    }
};

} // Server namespace.

int main(int argc, char** argv)
{
    std::vector<std::string> tmp;
    Server::MyServerApp app;

    return app.run(argc, argv);
}