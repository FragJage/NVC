#ifndef NICKYVOICECONTROL_H
#define NICKYVOICECONTROL_H

#include <map>
#include <vector>
#include "SimpleLog/SimpleLog.h"
#include "IVoiceControl.h"
#include "IVoiceRecogniser.h"
#include "IIntentParser.h"
#include "VoiceCommand.h"
#include "PluginFactory.h"
#include "IPluginImport.h"

struct NvcIntent
{
    int FunctionId;
    std::vector<std::string> Params;
};

class NickyVoiceControl : public IVoiceControl
{
    public:
        NickyVoiceControl();
        virtual ~NickyVoiceControl();

        bool LoadSettings(const std::string& fileName);
        void LoadPlugins();
        bool CheckSettings();
        const std::vector<VoiceCommand>* GetVoiceCommands(const std::string& area);
        const std::string& GetBotName();
        const std::vector<std::string> GetList(std::string listName);
        int GetCommandId(const std::string& command);
        void ExecutePlugin(int intentId, std::vector<std::string>& params);
        void SetVoiceRecogniser(IVoiceRecogniser* voiceRecogniser);
        void SetIntentParser(IIntentParser* intentParser);
        void SetCallBack(NVC::PLUGIN_CALLBACK_FUNC callback);
        std::string ListenVoice();
        std::string ListenFile(const std::string& fileName);
        int Listen();

        int ForCallback_GetFunctionId(const char* PluginOrigin, const char* CanonicFunctionName);
        NVC::PluginFunctionParameterDesc* ForCallback_GetFunctionParameter(const char* PluginOrigin, unsigned int functionId, unsigned int parameterNb);
        void ForCallback_ExecutePlugin(const char* PluginOrigin, int functionId, const char* params);

    private:
        void AddVoiceCommand(const std::string& area, const std::string& phrase, const std::string& command);

        SimpleLog m_DefaultLog;
        SimpleLog* m_Log;
        std::string m_MyName;
        std::vector<std::string> m_Plugins;
        std::map<std::string, std::vector<VoiceCommand>> m_VoiceAreas;
        NVC::PluginFactory m_PluginFactory;
        IVoiceRecogniser* m_VoiceRecogniser;
        IIntentParser* m_IntentParser;
};

#endif // NICKYVOICECONTROL_H
