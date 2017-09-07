#include "stdafx.h"
#include "CppUnitTest.h"

#include "Config/ConfigError.h"
#include "Config/ClientConfig.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest
{
    TEST_CLASS(ClientConfigTest)
    {
    public:
        TEST_METHOD(FromValidXML)
        {
            std::string xml = R"(
                <client>
                    <uniqueKey>client1</uniqueKey>
                    <monitorInterval>5</monitorInterval>
                    <serverHost>localhost</serverHost>
                    <serverPort>30080</serverPort>
                </client>
            )";
            auto config = Config::ClientConfig::readConfigFromXML(xml);
            Assert::AreEqual<std::string>(config.getUniqueKey(), "client1");
            Assert::AreEqual<unsigned int>(config.getMonitorInterval(), 5u);
            Assert::AreEqual<std::string>(config.getServerHost(), "localhost");
            Assert::AreEqual<unsigned int>(config.getServerPort(), 30080u);
        }

        TEST_METHOD(DefaultValue)
        {
            std::string xml = R"(
                <client>
                    <uniqueKey>client1</uniqueKey>
                    <serverHost>localhost</serverHost>
                </client>
            )";
            auto config = Config::ClientConfig::readConfigFromXML(xml);
            Assert::AreEqual<unsigned int>(config.getMonitorInterval(), 300);
            Assert::AreEqual<unsigned int>(config.getServerPort(), 40080u);
        }

        TEST_METHOD(NonXML)
        {
            auto func = [] {
                Config::ClientConfig::readConfigFromXML("nonxml");
            };
            Assert::ExpectException<Config::ConfigError>(func);
        }

        TEST_METHOD(NotClientAtRoot)
        {
            std::string xml = R"(
                <foo>
                    <uniqueKey>client1</uniqueKey>
                    <monitorInterval>5</monitorInterval>
                    <serverHost>localhost</serverHost>
                    <serverPort>30080</serverPort>
                </foo>
            )";
            auto func = [xml] {
                Config::ClientConfig::readConfigFromXML(xml);
            };
            Assert::ExpectException<Config::ConfigError>(func);
        }

        TEST_METHOD(NoUniqueKey)
        {
            std::string xml = R"(
                <foo>
                    <monitorInterval>5</monitorInterval>
                    <serverHost>localhost</serverHost>
                    <serverPort>30080</serverPort>
                </foo>
            )";
            auto func = [xml] {
                Config::ClientConfig::readConfigFromXML(xml);
            };
            Assert::ExpectException<Config::ConfigError>(func);
        }

        TEST_METHOD(NoServerHost)
        {
            std::string xml = R"(
                <foo>
                    <uniqueKey>client1</uniqueKey>
                    <monitorInterval>5</monitorInterval>
                    <serverPort>30080</serverPort>
                </foo>
            )";
            auto func = [xml] {
                Config::ClientConfig::readConfigFromXML(xml);
            };
            Assert::ExpectException<Config::ConfigError>(func);
        }

        TEST_METHOD(InvalidMonitorInterval)
        {
            std::string xml = R"(
                <foo>
                    <uniqueKey>client1</uniqueKey>
                    <monitorInterval>foo</monitorInterval>
                    <serverHost>localhost</serverHost>
                    <serverPort>30080</serverPort>
                </foo>
            )";
            auto func = [xml] {
                Config::ClientConfig::readConfigFromXML(xml);
            };
            Assert::ExpectException<Config::ConfigError>(func);
        }

        TEST_METHOD(InvalidServerPort)
        {
            std::string xml = R"(
                <foo>
                    <uniqueKey>client1</uniqueKey>
                    <monitorInterval>5</monitorInterval>
                    <serverHost>localhost</serverHost>
                    <serverPort>foo</serverPort>
                </foo>
            )";
            auto func = [xml] {
                Config::ClientConfig::readConfigFromXML(xml);
            };
            Assert::ExpectException<Config::ConfigError>(func);
        }
    };
}