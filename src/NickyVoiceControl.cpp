#include <iostream>
#include <fstream>
#include <cstring>

#include "jsoncpp/include/json/json.h"
#include "StringTools/StringTools.h"
#include "NickyVoiceControl.h"

using namespace std;
using namespace NVC;

NickyVoiceControl::NickyVoiceControl() : m_Log(&m_DefaultLog), m_PluginFactory(m_Log)
{
}

NickyVoiceControl::~NickyVoiceControl()
{
    //dtor
}

bool NickyVoiceControl::LoadSettings(const string& fileName)
{
    Json::Value root;
    Json::Value rootArray;
    Json::Reader reader;
    ifstream file;


    LOG_DEBUG(m_Log) << "*** ENTER ***";
    LOG_VERBOSE(m_Log) << "Settings file name : " << fileName;

    //*** Open settings file
    file.open(fileName, ifstream::in);
    if(file.fail())
    {
        LOG_ERROR(m_Log) << "Failed to open file " << fileName << " : " << strerror(errno);
        LOG_DEBUG(m_Log) << "*** EXIT with error ***";
        return false;
    }

    if(!reader.parse(file, root))
    {
        file.close();
        LOG_ERROR(m_Log)  << "Failed to parse settings : " << reader.getFormattedErrorMessages();
        LOG_DEBUG(m_Log) << "*** EXIT with error ***";
        return false;
    }

    file.close();

    //*** Read MyName
    m_MyName = root.get("MyName", "Nicky").asString();
    LOG_VERBOSE(m_Log) << "Bot name : " << m_MyName;

    //*** Read Plugins
    rootArray = root["Plugins"];
    if(rootArray.isArray())
    {
        for( Json::ValueIterator itr = rootArray.begin() ; itr != rootArray.end() ; itr++ )
        {
            m_Plugins.push_back(itr->asString());
        }
    }

    //*** Read Voice areas
    rootArray = root["VoiceAreas"];
    if(rootArray.isArray())
    {
        string area;
        string phrase;
        string command;
        Json::Value VCarray;


        for( Json::ValueIterator itr = rootArray.begin() ; itr != rootArray.end() ; itr++ )
        {
            area = itr->get("Name", "").asString();
            if(area=="") continue;
            LOG_VERBOSE(m_Log) << "Found area : " << area;

            VCarray = itr->get("VoiceCommands", "");
            if(!VCarray.isArray()) continue;

            for( Json::ValueIterator VCitr = VCarray.begin() ; VCitr != VCarray.end() ; VCitr++ )
            {
                phrase = VCitr->get("Phrase", "").asString();
                command = VCitr->get("Command", "").asString();
                AddVoiceCommand(area, phrase, command);
            }
        }
    }

    LOG_DEBUG(m_Log) << "*** EXIT with succes ***";
    return true;
}

void NickyVoiceControl::AddVoiceCommand(const string& area, const string& phrase, const string& command)
{
    m_VoiceAreas[area].emplace_back(phrase, command);
}

void NickyVoiceControl::LoadPlugins()
{
    std::vector<std::string>::const_iterator it = m_Plugins.begin();
    std::vector<std::string>::const_iterator itEnd = m_Plugins.end();

    LOG_DEBUG(m_Log) << "*** ENTER ***";

    while(it != itEnd)
    {
        if(!m_PluginFactory.Add(*it))
            LOG_ERROR(m_Log) << m_PluginFactory.GetLastPluginError();
        ++it;
    }

    LOG_DEBUG(m_Log) << "*** EXIT ***";
}

bool NickyVoiceControl::CheckSettings()
{
    return true;
}

const vector<VoiceCommand>* NickyVoiceControl::GetVoiceCommands(const string& area)
{
    map<string, vector<VoiceCommand>>::iterator it = m_VoiceAreas.find(area);

    if(it == m_VoiceAreas.end())
        return nullptr;

    return &(it->second);
}

const std::string& NickyVoiceControl::GetBotName()
{
    return m_MyName;
}

const vector<string> NickyVoiceControl::GetList(string listName)
{
    return m_PluginFactory.GetList(listName);
}

int NickyVoiceControl::GetCommandId(const std::string& command)
{
    string funct;
    size_t pos;


    pos = command.find('(');
    if(pos == string::npos)
        funct = command;
    else
        funct = command.substr(0, pos);

    return m_PluginFactory.GetFunctionId(funct);
}

void NickyVoiceControl::ExecutePlugin(int commandId, std::vector<std::string>& paramValues)
{
    int i;
    PluginParameterHelper pluginParams;
    vector<string>::const_iterator it;
    vector<string>::const_iterator itEnd = paramValues.cend();
    const vector<PluginFunctionParameterDesc*>& paramDesc = m_PluginFactory.GetFunctionParameters(commandId);


    for(it=paramValues.cbegin(), i=0; it != itEnd; ++it, ++i)
    {
cout << "Param " << paramDesc[i]->Name << endl;
cout << "AssociedList " << paramDesc[i]->AssociedList << endl;
cout << "Reserved " << paramDesc[i]->Reserved << endl;
        if(paramDesc[i]->AssociedList != -1)
            pluginParams.Add(paramDesc[i], m_PluginFactory.GetListCode(paramDesc[i]->Reserved, *it));
        else
            pluginParams.Add(paramDesc[i], *it);
    }
    m_PluginFactory.Execute(commandId, pluginParams);
}

void NickyVoiceControl::SetVoiceRecogniser(IVoiceRecogniser* voiceRecogniser)
{
    m_VoiceRecogniser = voiceRecogniser;
    m_VoiceRecogniser->SetArea("HOME", this);
}

void NickyVoiceControl::SetIntentParser(IIntentParser* intentParser)
{
    m_IntentParser = intentParser;
    m_IntentParser->SetArea("HOME", this);
}

void NickyVoiceControl::SetCallBack(PLUGIN_CALLBACK_FUNC callback)
{
    m_PluginFactory.SetCallback(callback);
}

int NickyVoiceControl::Listen()
{
    IntentFound intentFound;
    string phrase;
    bool stop = false;
    while(!stop)
    {
        //phrase = Nvc.ListenFile("audio/FraClochetteSalon.raw");
        phrase = ListenVoice();
        cout << phrase << endl;

        if(phrase == "sarah stop") stop = true;

        intentFound = m_IntentParser->Parse(phrase);
        if(!intentFound.IsNull())
        {
            ExecutePlugin(intentFound.CommandId, intentFound.Parameters);
        }
    }

    return 0;
}

string NickyVoiceControl::ListenVoice()
{
    return m_VoiceRecogniser->ListenVoice();
}

string NickyVoiceControl::ListenFile(const string& fileName)
{
    return m_VoiceRecogniser->ListenFile(fileName.c_str());
}

int NickyVoiceControl::ForCallback_GetFunctionId(const char* PluginOrigin, const char* CanonicFunctionName)
{
    int fid;

    fid = m_PluginFactory.GetFunctionId(CanonicFunctionName);
    if(fid == -1)
        LOG_INFO(m_Log) << "Plugin " << PluginOrigin << " search an unknown function " << CanonicFunctionName;
    else
        LOG_VERBOSE(m_Log) << "Plugin " << PluginOrigin << " find the function " << CanonicFunctionName << ", this ID is " << fid;

    return fid;
}

PluginFunctionParameterDesc* NickyVoiceControl::ForCallback_GetFunctionParameter(const char* PluginOrigin, unsigned int functionId, unsigned int parameterNb)
{
    const vector<PluginFunctionParameterDesc*>& paramsDesc = m_PluginFactory.GetFunctionParameters(functionId);

    LOG_VERBOSE(m_Log) << "Plugin " << PluginOrigin << " search the parameter nb " << parameterNb << " for the function nb " << functionId;

    if(parameterNb >= paramsDesc.size())
        return nullptr;

    return paramsDesc[parameterNb];
}

void NickyVoiceControl::ForCallback_ExecutePlugin(const char* PluginOrigin, int functionId, const char* params)
{
    LOG_VERBOSE(m_Log) << "Plugin " << PluginOrigin << " execute function nb  " << functionId << " (" << params << ")";
    m_PluginFactory.Execute(functionId, params);
}
