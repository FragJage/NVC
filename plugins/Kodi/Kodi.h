#ifndef KODI_H
#define KODI_H

#include <vector>
#include "KodiApi.h"
#include "IPlugin.h"

class Kodi : public NVC::IPlugin
{
    public:
        Kodi(NVC::PLUGIN_CALLBACK_FUNC NVCCallback);
        virtual ~Kodi();

        virtual void Execute(unsigned int functionNumber, NVC::PluginParameterHelper& parameters);
        virtual NVC::PluginListItem* GetListItems(unsigned int listNumber, unsigned int skip);

    private:
        enum functionKind {playPause, stop, incVolume, decVolume, setVolume, setPosition, smallForward, smallBackward, bigForward, bigBackward, playMovie};
        enum listKind {movies, actors};
        NVC::PluginFunctionParameterDesc m_ParamVolume;
        NVC::PluginFunctionParameterDesc m_ParamPasVolume;
        NVC::PluginFunctionParameterDesc m_ParamHours;
        NVC::PluginFunctionParameterDesc m_ParamMinutes;
        NVC::PluginFunctionParameterDesc m_ParamFilm;
        KodiApi m_KodiApi;
        std::vector<std::pair<std::string, std::string>> m_ActorsList =
        {
            { "1", "Sean Connery" }, { "2", "Mathilda May" }, { "3", "Pierre Richard" }
        };
};

DECLARE_PLUGIN(Kodi)

#endif // KODI_H
