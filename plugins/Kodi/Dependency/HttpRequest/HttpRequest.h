#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include<string>
#include"fifo_map.hpp"

class HttpRequest
{
    public:
        class Exception;
        enum HttpMethod {HTTP_GET, HTTP_POST, HTTP_PUT, HTTP_DELETE, HTTP_OPTIONS, HTTP_HEAD, HTTP_PATCH, HTTP_TRACE, HTTP_CONNECT};
        enum HttpVersion {HTTP_0_9, HTTP_1_0, HTTP_1_1, HTTP_2};
        HttpRequest();
        ~HttpRequest();

        void SetQueryString(const std::string& uri);
        void SetHttpMethod(HttpMethod httpMethod);
        void SetHttpVersion(HttpVersion httpVersion);
        void SetProxy(const std::string& host, int port);
        void SetTimeout(int delay);
        void RazQuery(bool withCookie, bool withProxy);
        void AddParameter(const std::string& key, const std::string& value);
        void RemoveParameter(const std::string& key);
        void AddHeader(const std::string& key, const std::string& value);
        void RemoveHeader(const std::string& key);
        void AddCookie(const std::string& key, const std::string& value);
        void RemoveCookie(const std::string& key);
        std::string toString();
        std::string Execute();

    private:
        std::string GetAddress();
        int GetPort();
        void Parse();

        std::string m_Uri;
        std::string m_Protocol;
        std::string m_Host;
        std::string m_Port;
        std::string m_Path;
        std::string m_Query;
        HttpMethod m_HttpMethod;
        HttpVersion m_HttpVersion;
        std::string m_ProxyHost;
        int m_ProxyPort;
        int m_Timeout;
        nlohmann::fifo_map<std::string, std::string> m_Parameters;
        nlohmann::fifo_map<std::string, std::string> m_Headers;
        nlohmann::fifo_map<std::string, std::string> m_Cookies;
};

/// \brief    Exception management
/// \details  Class to manage exception on HttpRequest.
class HttpRequest::Exception: public std::exception
{
public:
    /// \brief    Constructor of HttpRequest::Exception
    /// \details  Constructor of HttpRequest::Exception personalized exception for HttpRequest.
    Exception(int number, std::string const& message) throw();

    /// \brief    Destructor of HttpRequest::Exception
    /// \details  Destructor of HttpRequest::Exception personalized exception for HttpRequest.
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

#endif // HTTPREQUEST_H
