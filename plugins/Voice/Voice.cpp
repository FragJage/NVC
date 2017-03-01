#include<iostream>
#include "Voice.h"

using namespace std;

Voice::Voice(NVC::PLUGIN_CALLBACK_FUNC NVCCallback) : IPlugin(NVCCallback)
{
    SetPluginInformations("Voice", "Ce plugin donne de la voix à NVC.", 0, 1);
    m_ParamPhrase.Set("Phrase", "Phrase à prononcer", NVC::PluginParameterKind::NVCString);
    AddPluginFunction(0, "Dire", "Prononce une phrase", {&m_ParamPhrase});

    m_Pico.SetPath("./lang");
    if(!m_Pico.SetVoice("fr-FR"))
        Callback_Log(LogLevel::LVL_ERROR, m_Pico.GetPicoError());

    if(!m_Pico.InitializeSystem())
        Callback_Log(LogLevel::LVL_ERROR, m_Pico.GetPicoError());
}

Voice::~Voice()
{
    m_Pico.Cleanup();
}

NVC::PluginListItem* Voice::GetListItems(unsigned int listNumber, unsigned int skip)
{
    return nullptr;
}

void Voice::Execute(unsigned int functionNumber, NVC::PluginParameterHelper& parameters)
{
    if(functionNumber == 0)
    {
        string value = parameters.Get(&m_ParamPhrase);
        m_Pico.Process(value);
    }
}
