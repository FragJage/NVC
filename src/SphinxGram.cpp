#include<iostream>

//#include<stdio.h>
#include<fstream>
#include<sstream>
#include "StringTools/StringTools.h"
#include "SphinxGram.h"

using namespace std;

SphinxGram::SphinxGram()
{
    m_GrammarName = "";
    m_PublicRuleName = "";
    m_PublicRuleContent = "";
    m_LastError = "";
}

SphinxGram::~SphinxGram()
{
    m_Rules.clear();
}

void SphinxGram::AddDictionary(const string& fileName)
{
    m_SphinxDict.LoadOriginal(fileName);
}

void SphinxGram::SetGrammarName(const string& grammarName)
{
    m_GrammarName = grammarName;
}

bool SphinxGram::AddPublicRule(const string& ruleName, const string& content)
{
    string low = lowerFilter(content);

    if(!addWords(low)) return false;
    m_PublicRuleName = ruleName;
    m_PublicRuleContent = lowerFilter(content);
    return true;
}

bool SphinxGram::AddRule(const string& ruleName, const string& content)
{
    string low = lowerFilter(content);

    if(!addWords(low)) return false;
    m_Rules[ruleName] = low;
    return true;
}

bool SphinxGram::CompleteRule(const string& ruleName, const string& content)
{
    string low = lowerFilter(content);

    if(!addWords(low)) return false;
    if(m_Rules[ruleName] != "") m_Rules[ruleName] += " | ";
    m_Rules[ruleName] += low;
    return true;
}

void SphinxGram::SaveDictionary(const std::string& fileName)
{
    m_SphinxDict.SaveAdapted(fileName);
}
/*
void SphinxGram::SaveGrammar(const string& fileName)
{
    FILE* gramFile;
    map<string, string>::const_iterator it = m_Rules.cbegin();
    map<string, string>::const_iterator itEnd = m_Rules.cend();


    gramFile = fopen(fileName.c_str(), "w+,ccs=UTF-8");
    fwprintf(gramFile, L"#JSGF V1.0;\n");
    fwprintf(gramFile, L"grammar %hs;\n", m_GrammarName.c_str());
    fwprintf(gramFile, L"\n");

    while(it!=itEnd)
    {
        fwprintf(gramFile, L"<%hs> = %hs;\n", it->first.c_str(), it->second.c_str());
        ++it;
    }

    fwprintf(gramFile, L"public <%hs> = %hs;\n", m_PublicRuleName.c_str(), m_PublicRuleContent.c_str());

    fclose(gramFile);
}
*/
void SphinxGram::SaveGrammar(const string& fileName)
{
    ofstream gramFile;
    map<string, string>::const_iterator it = m_Rules.cbegin();
    map<string, string>::const_iterator itEnd = m_Rules.cend();


    gramFile.open(fileName, ofstream::out);
    if(gramFile.fail())
    {
        cout << "Failed to open file " << fileName << " : " << strerror(errno) << endl;
        return;
    }

    gramFile << "#JSGF V1.0;" << endl;
    gramFile << "grammar " << m_GrammarName << ";" << endl;
    gramFile << endl;

    while(it!=itEnd)
    {
        gramFile << "<" << it->first << "> = " << it->second << ";" << endl;
        ++it;
    }

    gramFile << "public <" << m_PublicRuleName << "> = " << m_PublicRuleContent << ";" << endl;

    gramFile.close();
}

string SphinxGram::lowerFilter(const string& command)
{
    char c, prev = '?';
    bool bList = false;
    ostringstream cmdOss;
    string cmdStr;


    for(size_t i=0; i<command.size(); i++)
    {
        c = command[i];
        switch(c)
        {
            case '?' :
            case '!' :
            case '/' :
            case '\'' :
            case '-' :
            case ',' :
            case ';' :
            case ':' :
            case '.' :
                if(!bList) c = ' ';
                break;
            case '<' :
                bList = true;
                break;
            case '>' :
                bList = false;
                break;
            default :
                if(!bList) c = tolower(c);
        }
        if((prev!=' ')||(c!=' ')) cmdOss << c;
        prev = c;
    }

    cmdStr = cmdOss.str();
    return StringTools::trim(cmdStr);
}

bool SphinxGram::addWords(const string& command)
{
    string word;
    string firstCar;
    stringstream ss;

    ss.str(command);
    while(getline(ss, word, ' '))
    {
        firstCar = word.substr(0,1);
        if(firstCar=="|") continue;
        if(firstCar=="<") continue;
        if(firstCar=="(") continue;

        if(!m_SphinxDict.AddWord(word))
        {
            ostringstream msg;
            msg << "Word '" << word << "' unknown in sentence '" << command << "'.";
            m_LastError = msg.str();
            return false;
        }
    }
    return true;
}

string SphinxGram::GetGramError()
{
    string error = m_LastError;
    m_LastError = "";
    return error;
}

int SphinxGram::GetNotFoundWordCount()
{
    return m_SphinxDict.GetNotFoundWordCount();
}
