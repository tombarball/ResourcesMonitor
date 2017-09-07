#include "stdafx.h"
#include "CppUnitTest.h"

#include "Protocol/MessageError.h"
#include "Protocol/StatisticMessage.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest
{		
	TEST_CLASS(StatisticMessageTest)
	{
	public:
		TEST_METHOD(FromValidXML)
		{
            std::string xml = R"(
                <message>
                    <cpuUsage>0.20</cpuUsage>
                    <memoryUsage>0.50</memoryUsage>
                    <processNumber>240</processNumber>
                </message>
            )";
            auto message = Protocol::StatisticMessage::fromXML(xml);
            Assert::AreEqual(message.getCPUUsage(), 0.2);
            Assert::AreEqual(message.getMemoryUsage(), 0.5);
            Assert::AreEqual(message.getProcessNumber(), 240u);
		}

        TEST_METHOD(NonXML)
        {
            auto func = [] {
                Protocol::StatisticMessage::fromXML("nonxml");
            };
            Assert::ExpectException<Protocol::MessageError>(func);
        }

        TEST_METHOD(NotMessageAtRoot)
        {
            std::string xml = R"(
                <foo>
                    <cpuUsage>0.20</cpuUsage>
                    <memoryUsage>0.50</memoryUsage>
                    <processNumber>240</processNumber>
                </foo>
            )";
            auto func = [xml] {
                Protocol::StatisticMessage::fromXML(xml);
            };
            Assert::ExpectException<Protocol::MessageError>(func);
        }

        TEST_METHOD(NoCPUUsage)
        {
            std::string xml = R"(
                <foo>
                    <memoryUsage>0.50</memoryUsage>
                    <processNumber>240</processNumber>
                </foo>
            )";
            auto func = [xml] {
                Protocol::StatisticMessage::fromXML(xml);
            };
            Assert::ExpectException<Protocol::MessageError>(func);
        }

        TEST_METHOD(NoMemoryUsage)
        {
            std::string xml = R"(
                <foo>
                    <cpuUsage>0.20</cpuUsage>
                    <processNumber>240</processNumber>
                </foo>
            )";
            auto func = [xml] {
                Protocol::StatisticMessage::fromXML(xml);
            };
            Assert::ExpectException<Protocol::MessageError>(func);
        }

        TEST_METHOD(NoProcessNumber)
        {
            std::string xml = R"(
                <foo>
                    <cpuUsage>0.20</cpuUsage>
                    <memoryUsage>0.50</memoryUsage>
                </foo>
            )";
            auto func = [xml] {
                Protocol::StatisticMessage::fromXML(xml);
            };
            Assert::ExpectException<Protocol::MessageError>(func);
        }

        TEST_METHOD(InvalidCPUUsage)
        {
            std::string xml = R"(
                <message>
                    <cpuUsage>foo</cpuUsage>
                    <memoryUsage>0.50</memoryUsage>
                    <processNumber>240</processNumber>
                </message>
            )";
            auto func = [xml] {
                Protocol::StatisticMessage::fromXML(xml);
            };
            Assert::ExpectException<Protocol::MessageError>(func);
        }

        TEST_METHOD(InvalidMemoryUsage)
        {
            std::string xml = R"(
                <message>
                    <cpuUsage>0.20</cpuUsage>
                    <memoryUsage>foo</memoryUsage>
                    <processNumber>240</processNumber>
                </message>
            )";
            auto func = [xml] {
                Protocol::StatisticMessage::fromXML(xml);
            };
            Assert::ExpectException<Protocol::MessageError>(func);
        }

        TEST_METHOD(InvalidProcessNumber)
        {
            std::string xml = R"(
                <message>
                    <cpuUsage>0.20</cpuUsage>
                    <memoryUsage>0.50</memoryUsage>
                    <processNumber>foo</processNumber>
                </message>
            )";
            auto func = [xml] {
                Protocol::StatisticMessage::fromXML(xml);
            };
            Assert::ExpectException<Protocol::MessageError>(func);
        }

        TEST_METHOD(ToXML)
        {
            Protocol::StatisticMessage message(0.2, 0.5, 240);
            std::string xml = message.toXML();

            Assert::AreNotEqual(xml.find("<message>"), std::string::npos);
            Assert::AreNotEqual(xml.find("<cpuUsage>0.20</cpuUsage>"), std::string::npos);
            Assert::AreNotEqual(xml.find("<memoryUsage>0.50</memoryUsage>"), std::string::npos);
            Assert::AreNotEqual(xml.find("<processNumber>240</processNumber>"), std::string::npos);
        }

	};
}