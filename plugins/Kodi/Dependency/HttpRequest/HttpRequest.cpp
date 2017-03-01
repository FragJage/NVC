#include <iostream>

#include <sstream>
#include <algorithm>    // find
#include "HttpRequest.h"
#include "SimpleSock/SimpleSockTCP.h"

using namespace std;

HttpRequest::HttpRequest()
{
    RazQuery(true, true);
    m_Timeout = 10000000;
}

HttpRequest::~HttpRequest()
{
    m_Parameters.clear();
    m_Headers.clear();
    m_Cookies.clear();
}

void HttpRequest::RazQuery(bool withCookie, bool withProxy)
{
    m_Parameters.clear();
    m_Headers.clear();
    if(withCookie) m_Cookies.clear();
    if(withProxy)
    {
        m_ProxyHost = "";
        m_ProxyPort = 0;
    }

    m_Uri = "";
    Parse();

    SetHttpVersion(HTTP_1_1);
    SetHttpMethod(HTTP_GET);
    AddHeader("Connection", "close");
    AddHeader("Content-Type", "application/x-www-form-urlencoded");
}

void HttpRequest::SetTimeout(int delay)
{
    m_Timeout = delay;
}

void HttpRequest::SetQueryString(const string& uri)
{
    m_Uri = uri;
    Parse();
}

void HttpRequest::SetHttpMethod(HttpMethod method)
{
    m_HttpMethod = method;
}

void HttpRequest::SetHttpVersion(HttpVersion version)
{
    m_HttpVersion = version;
}

void HttpRequest::SetProxy(const std::string& host, int port)
{
    m_ProxyHost = host;
    m_ProxyPort = port;
}

void HttpRequest::AddParameter(const string& key, const string& value)
{
    m_Parameters[key] = value;
}

void HttpRequest::RemoveParameter(const string& key)
{
    m_Parameters.erase(key);
}

void HttpRequest::AddHeader(const string& key, const string& value)
{
    m_Headers[key] = value;
}

void HttpRequest::RemoveHeader(const string& key)
{
    m_Headers.erase(key);
}

void HttpRequest::AddCookie(const std::string& key, const std::string& value)
{
    m_Cookies[key] = value;
}

void HttpRequest::RemoveCookie(const std::string& key)
{
    m_Cookies.erase(key);
}

void HttpRequest::Parse()
{
    size_t posChar;
    string parsedUri = m_Uri;

    m_Protocol = "";
    m_Host = "";
    m_Port = "";
    m_Path = "";
    m_Query = "";

    if (parsedUri.length() == 0) return;

    // query
    posChar = parsedUri.find("?");
    if (posChar != string::npos)
    {
        m_Query = parsedUri.substr(posChar+1);
        parsedUri.erase(posChar, parsedUri.length());
    }

    // protocol
    posChar = parsedUri.find("://");
    if(posChar != string::npos)
    {
        m_Protocol = parsedUri.substr(0, posChar);
        parsedUri.erase(0, posChar+3);
    }

    // path
    posChar = parsedUri.find("/");
    if (posChar != string::npos)
    {
        m_Path = parsedUri.substr(posChar+1);
        parsedUri.erase(posChar, parsedUri.length());
    }

    // host
    posChar = parsedUri.find(":");
    if (posChar != string::npos)
    {
        m_Host = parsedUri.substr(0, posChar);
        m_Port = parsedUri.substr(posChar+1);
    }
    else
    {
        m_Host = parsedUri;
    }
}

string HttpRequest::GetAddress()
{
    string hostName;
    struct hostent *host;


    if(m_ProxyHost!="")
        hostName = m_ProxyHost;
    else
        hostName = m_Host;

    if(hostName=="")
        throw HttpRequest::Exception(0x0061, "HttpRequest::GetAddress: Must set an uri or host before call this function");

    host = gethostbyname(hostName.c_str());
    return inet_ntoa(*(in_addr *)host->h_addr);
}

int HttpRequest::GetPort()
{
    if(m_ProxyPort!=0) return m_ProxyPort;
    if(m_Port=="") return 80;
    return atoi(m_Port.c_str());
}

string HttpRequest::toString()
{
    ostringstream ssRequest;
    ostringstream ssQuery;
    string sQuery;
    nlohmann::fifo_map<string, string>::const_iterator it;
    nlohmann::fifo_map<string, string>::const_iterator itEnd;
    nlohmann::fifo_map<string, string>::const_iterator itBegin;

    itBegin = m_Parameters.begin();
    itEnd = m_Parameters.end();
    for(it=itBegin; it!=itEnd; ++it)
    {
        if(it!=itBegin) ssQuery << "&";
        ssQuery << it->first << "=" << it->second;
    }
    sQuery = ssQuery.str();

    switch(m_HttpMethod)
    {
        case HTTP_GET :
            ssRequest << "GET ";
            break;
        case HTTP_POST :
            ssRequest << "POST ";
            break;
        case HTTP_PUT :
            ssRequest << "PUT ";
            break;
        case HTTP_DELETE :
            ssRequest << "DELETE ";
            break;
        case HTTP_OPTIONS :
            ssRequest << "OPTIONS ";
            break;
        case HTTP_HEAD :
            ssRequest << "HEAD ";
            break;
        case HTTP_PATCH :
            ssRequest << "PATCH ";
            break;
        case HTTP_TRACE :
            ssRequest << "TRACE ";
            break;
        case HTTP_CONNECT :
            ssRequest << "CONNECT ";
            break;
    }

    if(m_ProxyHost!="")
    {
        ssRequest << m_Uri;
    }
    else
    {
        ssRequest << "/" << m_Path;
        if(m_Query != "")
            ssRequest << "?" << m_Query;
    }

    switch(m_HttpVersion)
    {
        case HTTP_0_9 :
            ssRequest << " HTTP/0.9\r\n";
            break;
        case HTTP_1_0 :
            ssRequest << " HTTP/1.0\r\n";
            break;
        case HTTP_1_1 :
            ssRequest << " HTTP/1.1\r\n";
            break;
        case HTTP_2 :
            ssRequest << " HTTP/2\r\n";
            break;
    }

    ssRequest << "Host: " << m_Host << "\r\n";

    itEnd = m_Headers.end();
    for(it=m_Headers.begin(); it!=itEnd; ++it)
        ssRequest << it->first << ": " << it->second << "\r\n";

    ssRequest << "Content-Length: " << sQuery.length() << "\r\n";

    if(m_Cookies.size()>0)
    {
        ssRequest << "Cookie: ";
        itEnd = m_Cookies.end();
        for(it=m_Cookies.begin(); it!=itEnd; ++it)
            ssRequest << it->first << "=" << it->second << ";";
        ssRequest << "\r\n";
    }

    ssRequest << "\r\n";

    return ssRequest.str()+sQuery;
}

string HttpRequest::Execute()
{
    SimpleSockTCP sockHttp;
    string bufResponse;
    string totResponse;

    sockHttp.Connect(GetAddress(), GetPort());
    sockHttp.Blocking(true);

    sockHttp.Send(toString());
    if(!sockHttp.WaitRecv(m_Timeout)) return "";

    do
    {
        sockHttp.Recv(bufResponse);
        totResponse += bufResponse;
    } while(bufResponse!="");

    sockHttp.Close();

    return totResponse;
}

/****************************************************************************************************/
/*** Class Exception                                                                                */
HttpRequest::Exception::Exception(int number, string const& message) throw()
{
    m_number = number;
    m_message = message;
}

HttpRequest::Exception::~Exception() throw()
{
}

const char* HttpRequest::Exception::what() const throw()
{
    return m_message.c_str();
}

int HttpRequest::Exception::GetNumber() const throw()
{
    return m_number;
}
