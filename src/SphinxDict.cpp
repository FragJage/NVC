#include<iostream>
#include<fstream>
#include<cstring>
#include "SphinxDict.h"

using namespace std;

SphinxDict::SphinxDict()
{
    m_NotFoundWordCount = 0;
}

SphinxDict::~SphinxDict()
{
    m_OriginalDict.clear();
    m_AdaptedDict.clear();
}

void SphinxDict::LoadOriginal(string fileName)
{
    ifstream file;
    string line, key, val;
    size_t pos;

    file.open(fileName, ifstream::in);
    if(file.fail())
    {
        cout << "Failed to open file " << fileName << " : " << strerror(errno) << endl;
        return;
    }

    while(getline(file, line))
    {
        pos = line.find(' ');
        if(pos == string::npos) continue;

        key = line.substr(0, pos);
        val = line.substr(pos+1);

        pos = key.find('(');
        if(pos != string::npos)
            key = key.substr(0, pos);

        m_OriginalDict.emplace(key, val);
    }

    file.close();
}

void SphinxDict::SaveAdapted(string fileName)
{
    ofstream file;
    string prev = "";
    int nb = 0;

    file.open(fileName, ofstream::out);
    if(file.fail())
    {
        cout << "Failed to open file " << fileName << " : " << strerror(errno) << endl;
        return;
    }

    multimap<string, string>::const_iterator it = m_AdaptedDict.cbegin();
    multimap<string, string>::const_iterator itEnd = m_AdaptedDict.cend();

    while(it != itEnd)
    {
        if(it->first != prev) nb=1;
        file << it->first;
        if(nb>1) file <<"("<<nb<<")";
        file << " " << it->second << endl;
        prev = it->first;
        nb++;
        ++it;
    }

    file.close();
}

bool SphinxDict::AddWord(string word)
{
    multimap<string, string>::const_iterator it = m_AdaptedDict.find(word);
    if(it!=m_AdaptedDict.cend())
        return true;

    pair<multimap<string, string>::const_iterator, multimap<string, string>::const_iterator> range = m_OriginalDict.equal_range(word);
    if(range.first == range.second)
    {
        m_NotFoundWordCount++;
        return false;
    }

    for (it=range.first; it!=range.second; ++it)
        m_AdaptedDict.emplace(it->first, it->second);

    return true;
}

int SphinxDict::GetNotFoundWordCount()
{
    return m_NotFoundWordCount;
}
