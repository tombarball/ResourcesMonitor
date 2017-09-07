#pragma once

#include <string>
#include <stdexcept>

namespace Protocol
{

/**
  This exception can be thrown when sending an email to the SMTP server.
*/
class SMTPException : public std::runtime_error
{
public:
    SMTPException(const std::string& error)
        : std::runtime_error(error)
    {
    }
};

/**
  The class allow users to send an email to the SMTP server
  from the given host/port.
*/
class SMTPMailSender
{
public:
    /**
      Construct the mail sender object from the given host/port.
    */
    SMTPMailSender(const std::string& smtpHost,
                   unsigned short smtpPort)
        : smtpHost_(smtpHost),
          smtpPort_(smtpPort)
    {
    }

    /**
      Send an email to the given "to" parameter by "from" with the given
      mail subject and content.

      The method possibly throw Protocol::SMTPException.
    */
    void sendMail(const std::string& from,
                  const std::string& to,
                  const std::string& subject,
                  const std::string& content);

private:
    std::string smtpHost_;
    unsigned short smtpPort_;
};

}
