#include "stdafx.h"
#include "CppUnitTest.h"

#include "Config/ConfigError.h"
#include "Config/ClientInfoConfig.h"

#include <climits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest
{
    TEST_CLASS(ClientInfoConfigTest)
    {
    public:
        TEST_METHOD(FromValidXML)
        {
            std::string xml = R"(
                <client key="client1" mail="client1@mydomain.com">
                    <alert type="memory" limit="80%" />
                    <alert type="cpu" limit="70%" />
                    <alert type="processes" limit="500" />
                </client>
            )";
            auto config = Config::ClientInfoConfig::readConfigFromXML(xml);
            Assert::AreEqual<std::string>(config.getKey(), "client1");
            Assert::AreEqual<std::string>(config.getMail(), "client1@mydomain.com");
            Assert::AreEqual(config.getCPULimit(), 0.70);
            Assert::AreEqual(config.getMemoryLimit(), 0.80);
            Assert::AreEqual<unsigned int>(config.getProcessesLimit(), 500u);
        }

        TEST_METHOD(DefaultValue)
        {
            std::string xml = R"(
                <client key="client1" mail="client1@mydomain.com">
                </client>
            )";
            auto config = Config::ClientInfoConfig::readConfigFromXML(xml);
            Assert::AreEqual(config.getCPULimit(), 1.0);
            Assert::AreEqual(config.getMemoryLimit(), 1.0);
            Assert::AreEqual<unsigned int>(config.getProcessesLimit(), UINT_MAX);
        }

        TEST_METHOD(NonXML)
        {
            auto func = [] {
                Config::ClientInfoConfig::readConfigFromXML("nonxml");
            };
            Assert::ExpectException<Config::ConfigError>(func);
        }

        TEST_METHOD(NotClientAtRoot)
        {
            std::string xml = R"(
                <foo key="client1" mail="client1@mydomain.com">
                    <alert type="memory" limit="80%" />
                    <alert type="cpu" limit="70%" />
                    <alert type="processes" limit="500" />
                </foo>
            )";
            auto func = [xml] {
                Config::ClientInfoConfig::readConfigFromXML(xml);
            };
            Assert::ExpectException<Config::ConfigError>(func);
        }

        TEST_METHOD(NoKey)
        {
            std::string xml = R"(
                <client mail="client1@mydomain.com">
                    <alert type="memory" limit="80%" />
                    <alert type="cpu" limit="70%" />
                    <alert type="processes" limit="500" />
                </client>
            )";
            auto func = [xml] {
                Config::ClientInfoConfig::readConfigFromXML(xml);
            };
            Assert::ExpectException<Config::ConfigError>(func);
        }

        TEST_METHOD(NoMail)
        {
            std::string xml = R"(
                <client key="client1">
                    <alert type="memory" limit="80%" />
                    <alert type="cpu" limit="70%" />
                    <alert type="processes" limit="500" />
                </client>
            )";
            auto func = [xml] {
                Config::ClientInfoConfig::readConfigFromXML(xml);
            };
            Assert::ExpectException<Config::ConfigError>(func);
        }

        TEST_METHOD(InvalidMemoryLimit)
        {
            std::string xml = R"(
                <client key="client1" mail="client1@mydomain.com">
                    <alert type="memory" limit="foo" />
                    <alert type="cpu" limit="70%" />
                    <alert type="processes" limit="500" />
                </client>
            )";
            auto func = [xml] {
                Config::ClientInfoConfig::readConfigFromXML(xml);
            };
            Assert::ExpectException<Config::ConfigError>(func);
        }

        TEST_METHOD(InvalidCPULimit)
        {
            std::string xml = R"(
                <client key="client1" mail="client1@mydomain.com">
                    <alert type="memory" limit="80%" />
                    <alert type="cpu" limit="foo" />
                    <alert type="processes" limit="500" />
                </client>
            )";
            auto func = [xml] {
                Config::ClientInfoConfig::readConfigFromXML(xml);
            };
            Assert::ExpectException<Config::ConfigError>(func);
        }

        TEST_METHOD(InvalidProcessesLimit)
        {
            std::string xml = R"(
                <client key="client1" mail="client1@mydomain.com">
                    <alert type="memory" limit="80%" />
                    <alert type="cpu" limit="70%" />
                    <alert type="processes" limit="foo" />
                </client>
            )";
            auto func = [xml] {
                Config::ClientInfoConfig::readConfigFromXML(xml);
            };
            Assert::ExpectException<Config::ConfigError>(func);
        }
    };
}