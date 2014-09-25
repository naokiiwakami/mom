#ifndef _applicationexception_hxx_
#define _applicationexception_hxx_

#include <exception>
#include <string>

class ApplicationException : public std::exception
{
public:
    ApplicationException()
        : m_hasMessage(false)
    {
    }

    ApplicationException(const std::string& message)
    {
        m_message = message;
        if (!message.empty()) {
            m_hasMessage = true;
        }
    }

    virtual ~ApplicationException() throw() {
    }

    bool hasMessage() const {
        return m_hasMessage;
    }

    std::string getMessage() const {
        return m_message;
    }

    virtual const char* what() const throw()
    {
        if (m_hasMessage) {
            return m_message.c_str();
        }
        else {
            return std::exception::what();
        }
    }

private:
    std::string m_message;
    bool m_hasMessage;
 
};

#endif
