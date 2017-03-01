#include<iostream>
#include<ctime>
#include "Internal.h"

using namespace std;

Internal::Internal(NVC::PLUGIN_CALLBACK_FUNC NVCCallback) : IPlugin(NVCCallback)
{
    SetPluginInformations("Internal", "Ce plugin regroupe les fonctions internes de NVC.", 0, 1);

    AddPluginFunction(functionKind::botStop, "botStop", "Arrête NVC");
    AddPluginFunction(functionKind::sayTime, "DireHeure", "Donne l'heure");
    AddPluginFunction(functionKind::sayDate, "DireDate", "Donne la date");
}

Internal::~Internal()
{
}

NVC::PluginListItem* Internal::GetListItems(unsigned int listNumber, unsigned int skip)
{
    return nullptr;
}

void Internal::Execute(unsigned int functionNumber, NVC::PluginParameterHelper& parameters)
{
    if(m_VoiceFunctionId==-1) InitVoice();

    switch(functionNumber)
    {
        case functionKind::sayTime :
            CallVoice(GetTime());
            break;

        case functionKind::sayDate :
            CallVoice(GetDate());
            break;
    }
}

string Internal::GetTime()
{
  time_t rawtime;
  struct tm * timeinfo;
  ostringstream oss;

  time (&rawtime);
  timeinfo = localtime(&rawtime);
  oss << "il est " << GetHour(timeinfo->tm_hour);

  if(timeinfo->tm_min == 0) return oss.str();

  oss << " heure " << GetMinute(timeinfo->tm_min);

  return oss.str();
}

std::string Internal::GetHour(int hour)
{
    ostringstream oss;

    if(hour == 0) return "minuit";
    if(hour == 1) return "une";
    if(hour == 12) return "midi";
    if(hour == 21) return "vingt et une";

    oss << hour;
    return oss.str();
}

std::string Internal::GetMinute(int minute)
{
    ostringstream oss;

    if(minute == 1) return "une";
    if(minute == 21) return "vingt et une";
    if(minute == 31) return "trente et une";
    if(minute == 41) return "quarante et une";
    if(minute == 51) return "cinquante et une";

    oss << minute;
    return oss.str();
}

string Internal::GetDate()
{
    time_t rawtime;
    struct tm * timeinfo;
    char buffer[80];

    setlocale(LC_ALL,"");
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer,80,"nous sommes le %A %d %B %Y",timeinfo);

    string str(buffer);
    return str;
}

void Internal::InitVoice()
{
    m_VoiceFunctionId = Callback_GetFunctionId("Voice.Dire");
    if(m_VoiceFunctionId == -1)
    {
        m_VoiceFunctionId = -2;
        return;
    }

    m_phraseParameter = Callback_GetFunctionParameter(m_VoiceFunctionId, 0);
    if(string(m_phraseParameter->Name) != "Phrase")
    {
        Callback_Log(LogLevel::LVL_ERROR, "Le 1er paramètre de Voide.Dire n'est pas Phrase");
        m_VoiceFunctionId = -2;
        return;
    }
}

void Internal::CallVoice(string phrase)
{
    NVC::PluginParameterHelper params;

    if(m_VoiceFunctionId<0) return;
    params.Add(m_phraseParameter, phrase);
    Callback_Execute(m_VoiceFunctionId, params.ToPublic());
}
