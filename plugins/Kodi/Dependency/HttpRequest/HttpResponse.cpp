#include <cstring>
#include <sstream>
#include <algorithm>
#include "StringTools/StringTools.h"
#include "HttpResponse.h"

using namespace std;

HttpResponse::HttpResponse()
{
}

HttpResponse::~HttpResponse()
{
    m_Header.clear();
}

void HttpResponse::Parse(const string& httpResponse)
{
    istringstream iss(httpResponse);
    ostringstream oss;
    string line, key, value;
    size_t pos;

    m_Status = 0;
    m_Body = "";
    m_Header.clear();

    if(!getline(iss,line))
        return;

    pos = line.find(' ');
    if(pos != string::npos)
        m_Status = atoi(line.substr(pos+1, 3).c_str());

    while(getline(iss,line))
    {
        StringTools::ParasitCar(line);
        if(line=="")
            break;

        StringTools::Split(line, ':', &key, &value);
        m_Header[key] = value;
    }

    while(getline(iss,line))
        oss << line << endl;

    m_Body = oss.str();
}

int HttpResponse::GetStatus()
{
    return m_Status;
}

std::string HttpResponse::GetBody()
{
    return m_Body;
}

std::string HttpResponse::GetHeader(const std::string& key)
{
    std::map<std::string, std::string>::const_iterator it = m_Header.find(key);

    if(it == m_Header.end())
        return "";

    return it->second;
}


/****************************************************************************************************/
/*** Class Exception                                                                                */
HttpResponse::Exception::Exception(int number, string const& message) throw()
{
    m_number = number;
    m_message = message;
}

HttpResponse::Exception::~Exception() throw()
{
}

const char* HttpResponse::Exception::what() const throw()
{
    return m_message.c_str();
}

int HttpResponse::Exception::GetNumber() const throw()
{
    return m_number;
}
