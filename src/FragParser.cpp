#include <iostream>
#include "StringTools/StringTools.h"
#include "FragParser.h"

using namespace std;

FragParser::FragParser()
{
}

FragParser::~FragParser()
{
    m_IntentItems.clear();
}

void FragParser::IntentAdd(int commandId, const std::string& phrase, const std::string& command)
{
    string tmp = phrase;
    size_t posDeb = 0;
    size_t posFin = 0;
    bool isFound;

    do
    {
        posDeb = tmp.find_first_of("<", posFin);
        posFin = tmp.find_first_of(">", posDeb);
        isFound = ((posDeb!=string::npos)&&(posFin!=string::npos));
        if(isFound)
            tmp.replace(posDeb, posFin-posDeb+1, "(.*)");
    } while(isFound);

    StringTools::ReplaceAll(tmp, "?", "");
    StringTools::ReplaceAll(tmp, "/", " ");
    StringTools::ReplaceAll(tmp, "'", " ");
    StringTools::ReplaceAll(tmp, "-", " ");
    StringTools::trim(tmp);
    StringTools::ToLower(tmp);

    m_IntentItems.emplace_back(commandId, "^"+tmp+"$");

    posDeb = command.find('(');
    if(posDeb != string::npos)
    {
        tmp = command.substr(posDeb+1, command.length()-posDeb-2);
        m_IntentItems[m_IntentItems.size()-1].Parameters = StringTools::Split(tmp, ',');
    }
}

bool FragParser::SetArea(const std::string& areaName, IVoiceControl* voiceControl)
{
    int commandId;
    string command;


    const vector<VoiceCommand>* voiceCommands = voiceControl->GetVoiceCommands(areaName);
    vector<VoiceCommand>::const_iterator it = voiceCommands->begin();
    vector<VoiceCommand>::const_iterator itEnd = voiceCommands->end();
    string bot = voiceControl->GetBotName();

    StringTools::ToLower(bot);
    while(it != itEnd)
    {
        command = it->GetCommand();
        commandId = voiceControl->GetCommandId(command);
        if(commandId != -1)
            IntentAdd(commandId, bot+" "+it->GetPhrase(), command);

        ++it;
    }
    return true;
}

IntentFound FragParser::Parse(const std::string& phrase)
{
    vector<IntentItem>::const_iterator it = m_IntentItems.begin();
    vector<IntentItem>::const_iterator itEnd = m_IntentItems.end();
    IntentFound intentFound;
    smatch sm;
    string param;

    intentFound.CommandId = -1;
    while(it != itEnd)
    {
        if(regex_match(phrase, sm, it->Pattern))
        {
            vector<string>::const_iterator itp;
            vector<string>::const_iterator itpEnd = it->Parameters.end();
            for (itp = it->Parameters.begin(); itp!=itpEnd; ++itp)
            {
                param = *itp;
                if(param[0]=='$')
                    param = sm[StringTools::stoi(param.substr(1))].str();
                intentFound.Parameters.push_back(param);
            }

            intentFound.CommandId = it->CommandId;
            break;
        }
        ++it;
    }

    return intentFound;
}
