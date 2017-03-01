#ifndef INTERNAL_H
#define INTERNAL_H

#include <vector>
#include "IPlugin.h"

class Internal : public NVC::IPlugin
{
    public:
        Internal(NVC::PLUGIN_CALLBACK_FUNC NVCCallback);
        virtual ~Internal();

        virtual void Execute(unsigned int functionNumber, NVC::PluginParameterHelper& parameters);
        virtual NVC::PluginListItem* GetListItems(unsigned int listNumber, unsigned int skip);

    private:
        enum functionKind {botStop, sayTime, sayDate};
        std::string GetTime();
        std::string GetHour(int hour);
        std::string GetMinute(int minute);
        std::string GetDate();
        void InitVoice();
        void CallVoice(std::string phrase);
        int m_VoiceFunctionId = -1;
        NVC::PluginFunctionParameterDesc* m_phraseParameter = nullptr;
};

DECLARE_PLUGIN(Internal)

#endif // INTERNAL_H
