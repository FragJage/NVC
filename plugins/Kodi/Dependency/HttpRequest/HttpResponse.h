#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H

#include<string>
#include<map>

class HttpResponse
{
    public:
        class Exception;
        HttpResponse();
        ~HttpResponse();
        void Parse(const std::string& uri);
        int GetStatus();
        std::string GetBody();
        std::string GetHeader(const std::string& key);

    private:
        int m_Status;
        std::string m_Body;
        std::map<std::string, std::string> m_Header;

};

/// \brief    Exception management
/// \details  Class to manage exception on HttpResponse.
class HttpResponse::Exception: public std::exception
{
public:
    /// \brief    Constructor of HttpResponse::Exception
    /// \details  Constructor of HttpResponse::Exception personalized exception for HttpResponse.
    Exception(int number, std::string const& message) throw();

    /// \brief    Destructor of HttpResponse::Exception
    /// \details  Destructor of HttpResponse::Exception personalized exception for HttpResponse.
    ~Exception() throw();

    /// \brief    Get string identifying exception
    /// \details  Returns a null terminated character sequence that may be used to identify the exception.
    const char* what() const throw();

    /// \brief    Get the number of exception
    /// \details  Returns a number to identify the exception.
    int GetNumber() const throw();

    private:
        int m_number;
        std::string m_message;
};

#endif // HTTPRESPONSE_H
