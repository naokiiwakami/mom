#ifndef _badusageexception_hxx_
#define _badusageexception_hxx_

#include <exception>
#include <string>

class BadUsageException : public std::exception
{
public:
    BadUsageException()
        : m_hasMessage(false)
    {
    }

    BadUsageException(const std::string& message)
    {
        m_message = message;
        if (!message.empty()) {
            m_hasMessage = true;
        }
    }

    virtual ~BadUsageException() throw() {
    }

    bool hasMessage() const {
        return m_hasMessage;
    }

    std::string getMessage() const {
        return m_message;
    }

private:
    std::string m_message;
    bool m_hasMessage;
 
};

#endif
