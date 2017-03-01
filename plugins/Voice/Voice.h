#ifndef VOICE_H
#define VOICE_H

#include <vector>
#include "IPlugin.h"
#include "picotts.h"

class Voice : public NVC::IPlugin
{
    public:
        Voice(NVC::PLUGIN_CALLBACK_FUNC NVCCallback);
        virtual ~Voice();

        virtual void Execute(unsigned int functionNumber, NVC::PluginParameterHelper& parameters);
        virtual NVC::PluginListItem* GetListItems(unsigned int listNumber, unsigned int skip);

    private:
        NVC::PluginFunctionParameterDesc m_ParamPhrase;
        Pico m_Pico;
};

DECLARE_PLUGIN(Voice)

#endif // VOICE_H
