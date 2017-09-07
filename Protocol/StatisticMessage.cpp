#include "StatisticMessage.h"
#include "MessageError.h"

#include "Poco/DOM/DOMParser.h"
#include "Poco/DOM/Document.h"
#include "Poco/DOM/AutoPtr.h"
#include "Poco/DOM/Text.h"
#include "Poco/DOM/DOMWriter.h"
#include "Poco/XML/XMLWriter.h"
#include "Poco/Exception.h"

#include <sstream>
#include <iomanip>

namespace Protocol
{

namespace
{

std::string extractChildText(const Poco::XML::Element* element,
                             const std::string& name)
{
    auto child = element->getChildElement(name);
    if (!child)
        throw MessageError(name + " is not found!");

    return child->innerText();
}

// Utility function to convert double to string with two digits.
std::string doubleToString(double value)
{
    std::ostringstream oss;

    oss << std::fixed << std::setprecision(2) << value;
    return oss.str();
}

void createSubElement(Poco::XML::AutoPtr<Poco::XML::Document> doc,
                      Poco::XML::Element* element,
                      const std::string& name,
                      const std::string& value)
{
    auto subElement = doc->createElement(name);
    auto text = doc->createTextNode(value);
    subElement->appendChild(text);
    element->appendChild(subElement);
}

}

StatisticMessage StatisticMessage::fromXML(const std::string& xml)
{
    double cpuUsage = 0.0;
    double memoryUsage = 0.0;
    unsigned int processNumber = 0;

    try {
        Poco::XML::DOMParser parser;
        auto pDoc = parser.parseString(xml);
        auto root = pDoc->documentElement();
        if (!root || root->nodeName() != "message")
            throw MessageError("This is not message XML!");

        // Each resources are mandory.
        cpuUsage = std::stod(extractChildText(root, "cpuUsage"));
        memoryUsage = std::stod(extractChildText(root, "memoryUsage"));
        processNumber = std::stoul(extractChildText(root, "processNumber"));

        return StatisticMessage(cpuUsage, memoryUsage, processNumber);
    }
    catch (Poco::Exception&) {
        throw MessageError("Cannnot parse statistic message!");
    }
    catch (std::invalid_argument&) {
        throw MessageError("Some value in statistic message are invalid!");
    }
}

std::string StatisticMessage::toXML() const
{
    // message element.
    auto doc = Poco::XML::AutoPtr<Poco::XML::Document>(new Poco::XML::Document());
    auto messageElement = doc->createElement("message");
    doc->appendChild(messageElement);

    // each properties value.
    createSubElement(doc, messageElement, "cpuUsage", doubleToString(cpuUsage_));
    createSubElement(doc, messageElement, "memoryUsage", doubleToString(memoryUsage_));
    createSubElement(doc, messageElement, "processNumber", std::to_string(processNumber_));

    // writhe down the xml.
    std::ostringstream oss;
    Poco::XML::DOMWriter writer;
    writer.setNewLine("\n");
    writer.setOptions(Poco::XML::XMLWriter::PRETTY_PRINT);
    writer.writeNode(oss, doc);

    return oss.str();
}

}