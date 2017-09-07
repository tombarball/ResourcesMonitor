#include "stdafx.h"
#include "CppUnitTest.h"

#include "Config/ConfigError.h"
#include "Config/ServerConfig.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest
{
    TEST_CLASS(ServerConfigTest)
    {
    public:
        TEST_METHOD(FromValidXML)
        {
            std::string xml = R"(
                <server>
                    <HTTPConfig>
                        <port>30080</port>
                    </HTTPConfig>
                    <SMTPConfig>
                        <host>127.0.0.1</host>
                        <port>30025</port>
                    </SMTPConfig>
                    <DatabaseConfig>
                        <path>monitor.db</path>
                    </DatabaseConfig>
                </server>
            )";
            auto config = Config::ServerConfig::readConfigFromXML(xml);
            Assert::AreEqual<unsigned int>(config.getHTTPConfig().httpPort, 30080u);
            Assert::AreEqual<std::string>(config.getSMTPConfig().smtpHost, "127.0.0.1");
            Assert::AreEqual<unsigned int>(config.getSMTPConfig().smtpPort, 30025u);
            Assert::AreEqual<std::string>(config.getDBConfig().databasePath, "monitor.db");
        }

        TEST_METHOD(DefaultValue)
        {
            std::string xml = R"(
                <server>
                    <SMTPConfig>
                        <host>127.0.0.1</host>
                    </SMTPConfig>
                </server>
            )";
            auto config = Config::ServerConfig::readConfigFromXML(xml);
            Assert::AreEqual<unsigned int>(config.getHTTPConfig().httpPort, 40080u);
            Assert::AreEqual<unsigned int>(config.getSMTPConfig().smtpPort, 40025u);
            Assert::AreEqual<std::string>(config.getDBConfig().databasePath, "default.db");
        }

        TEST_METHOD(NonXML)
        {
            auto func = [] {
                Config::ServerConfig::readConfigFromXML("nonxml");
            };
            Assert::ExpectException<Config::ConfigError>(func);
        }

        TEST_METHOD(NotServerAtRoot)
        {
            std::string xml = R"(
                <foo>
                    <HTTPConfig>
                        <port>30080</port>
                    </HTTPConfig>
                    <SMTPConfig>
                        <host>127.0.0.1</host>
                        <port>30025</port>
                    </SMTPConfig>
                    <DatabaseConfig>
                        <path>monitor.db</path>
                    </DatabaseConfig>
                </foo>
            )";
            auto func = [xml] {
                Config::ServerConfig::readConfigFromXML(xml);
            };
            Assert::ExpectException<Config::ConfigError>(func);
        }

        TEST_METHOD(NoSMTPConfig)
        {
            std::string xml = R"(
                <server>
                    <HTTPConfig>
                        <port>30080</port>
                    </HTTPConfig>
                    <DatabaseConfig>
                        <path>monitor.db</path>
                    </DatabaseConfig>
                </server>
            )";
            auto func = [xml] {
                Config::ServerConfig::readConfigFromXML(xml);
            };
            Assert::ExpectException<Config::ConfigError>(func);
        }

        TEST_METHOD(NoSMTPHost)
        {
            std::string xml = R"(
                <server>
                    <HTTPConfig>
                        <port>30080</port>
                    </HTTPConfig>
                    <SMTPConfig>
                        <port>30025</port>
                    </SMTPConfig>
                    <DatabaseConfig>
                        <path>monitor.db</path>
                    </DatabaseConfig>
                </server>
            )";
            auto func = [xml] {
                Config::ServerConfig::readConfigFromXML(xml);
            };
            Assert::ExpectException<Config::ConfigError>(func);
        }

        TEST_METHOD(InvalidHTTPPort)
        {
            std::string xml = R"(
                <server>
                    <HTTPConfig>
                        <port>foo</port>
                    </HTTPConfig>
                    <SMTPConfig>
                        <host>127.0.0.1</host>
                        <port>30025</port>
                    </SMTPConfig>
                    <DatabaseConfig>
                        <path>monitor.db</path>
                    </DatabaseConfig>
                </server>
            )";
            auto func = [xml] {
                Config::ServerConfig::readConfigFromXML(xml);
            };
            Assert::ExpectException<Config::ConfigError>(func);
        }

        TEST_METHOD(InvalidSMTPPort)
        {
            std::string xml = R"(
                <server>
                    <HTTPConfig>
                        <port>30080</port>
                    </HTTPConfig>
                    <SMTPConfig>
                        <host>127.0.0.1</host>
                        <port>foo</port>
                    </SMTPConfig>
                    <DatabaseConfig>
                        <path>monitor.db</path>
                    </DatabaseConfig>
                </server>
            )";
            auto func = [xml] {
                Config::ServerConfig::readConfigFromXML(xml);
            };
            Assert::ExpectException<Config::ConfigError>(func);
        }
    };
}