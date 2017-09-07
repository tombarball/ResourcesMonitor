#include "SMTPMailSender.h"

#include "Poco/Net/MailMessage.h"
#include "Poco/Net/MailRecipient.h"
#include "Poco/Net/SMTPClientSession.h"
#include "Poco/Net/NetException.h"

namespace Protocol
{

namespace
{

/**
  This class in only used to ensure that the session is closed after it is
  out of scope. So we don't need to call session.close() everytime.
*/
class ScopeSession
{
public:
    ScopeSession(Poco::Net::SMTPClientSession& session)
        : session_(session)
    {
    }

    ~ScopeSession()
    {
        session_.close();
    }

private:
    Poco::Net::SMTPClientSession& session_;
};

} // anonymous namespace.

void SMTPMailSender::sendMail(const std::string& from,
                              const std::string& to,
                              const std::string& subject,
                              const std::string& content)
{
    std::string my_subject = Poco::Net::MailMessage::encodeWord(subject, "UTF-8");
    Poco::Net::MailMessage message;
    message.setSender(from);
    message.addRecipient(Poco::Net::MailRecipient(
        Poco::Net::MailRecipient::PRIMARY_RECIPIENT, to));
    message.setSubject(my_subject);
    // The email can be text/plain for simplicity.
    message.setContentType("text/plain; charset=UTF-8");
    message.setContent(content, Poco::Net::MailMessage::ENCODING_8BIT);
    try {
        Poco::Net::SMTPClientSession session(smtpHost_, smtpPort_);
        ScopeSession scopedSession(session);
        session.open();
        session.sendMessage(message);
    }
    catch (Poco::Net::SMTPException&) {
        throw SMTPException("Cannot send email due to some reason!");
    }
    catch (Poco::Net::NetException&) {
        throw SMTPException("Cannot send email due to some reason!");
    }
}

} // Protocol namespace.