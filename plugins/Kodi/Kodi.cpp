#include "Kodi.h"

using namespace std;

Kodi::Kodi(NVC::PLUGIN_CALLBACK_FUNC NVCCallback) : IPlugin(NVCCallback)
{
    SetPluginInformations("Kodi", "Ce plugin permet de piloter kodi.", 0, 1);

    m_ParamVolume.Set("Volume", "Niveau de volume entre 0 et 100", NVC::PluginParameterKind::NVCInteger);
    m_ParamPasVolume.Set("PasVolume", "Pas de volume", NVC::PluginParameterKind::NVCInteger);
    m_ParamHours.Set("Heure", "Heure pour repositionner la lecture", NVC::PluginParameterKind::NVCInteger);
    m_ParamMinutes.Set("Minute", "Minute pour repositionner la lecture", NVC::PluginParameterKind::NVCInteger);
    m_ParamFilm.Set("Film", "Film de la médiathèque de KODI", NVC::PluginParameterKind::NVCInteger, listKind::movies);

    AddPluginFunction(functionKind::playPause, "PlayPause", "Bascule entre lecture et pause");
    AddPluginFunction(functionKind::stop, "Stop", "Arrête la lecture");
    AddPluginFunction(functionKind::incVolume, "IncrementVol", "Augmente le volume", &m_ParamPasVolume);
    AddPluginFunction(functionKind::decVolume, "DecrementVol", "Diminue le volume", &m_ParamPasVolume);
    AddPluginFunction(functionKind::setVolume, "SetVolume", "Fixe le niveau du volume", &m_ParamVolume);
    AddPluginFunction(functionKind::setPosition, "SetPosition", "Déplace l'index de lecture", {&m_ParamHours, &m_ParamMinutes});
    AddPluginFunction(functionKind::smallForward, "SmallForward", "Avance de 30 secondes");
    AddPluginFunction(functionKind::smallBackward, "SmallBackward", "Recule de 30 secondes");
    AddPluginFunction(functionKind::bigForward, "BigForward", "Avance de 10 minutes");
    AddPluginFunction(functionKind::bigBackward, "BigBackward", "Recule de 10 minutes");
    AddPluginFunction(functionKind::playMovie, "PlayMovie", "Joue un film", &m_ParamFilm);

    AddPluginList(listKind::movies, "Films", "Liste des films dans la médiathèque de KODI", NVC::PluginParameterKind::NVCInteger);
    AddPluginList(listKind::actors, "Acteurs", "Liste des acteurs dans la médiathèque de KODI", NVC::PluginParameterKind::NVCInteger);

    //m_KodiApi.SetServer("192.168.0.10", 8080);
    m_KodiApi.SetServer("192.168.0.15");
}

Kodi::~Kodi()
{
}

NVC::PluginListItem* Kodi::GetListItems(unsigned int listNumber, unsigned int skip)
{
    unsigned int i;
    NVC::PluginListItem* firstListItem = nullptr;
    NVC::PluginListItem* currentListItem = nullptr;
    map<int, string>::const_iterator it;
    map<int, string> moviesList;

    switch(listNumber)
    {
        case listKind::movies :
            moviesList = m_KodiApi.MovieList(skip, skip+300);
            for(it=moviesList.begin(); it!=moviesList.end(); ++it)
            {
                currentListItem = AddListItems(it->first, it->second, currentListItem);
                if(firstListItem == nullptr) firstListItem = currentListItem;
            }
            break;

        case listKind::actors :
            for(i=skip; i<m_ActorsList.size(); i++)
            {
                currentListItem = AddListItems(m_ActorsList[i].first, m_ActorsList[i].second, currentListItem);
                if(firstListItem == nullptr) firstListItem = currentListItem;
            }
            break;
    }
    return firstListItem;
}

void Kodi::Execute(unsigned int functionNumber, NVC::PluginParameterHelper& parameters)
{
    int iValue1;
    int iValue2;

    switch(functionNumber)
    {
        case functionKind::playPause :
            m_KodiApi.PlayPause();
            break;

        case functionKind::stop :
            m_KodiApi.Stop();
            break;

        case functionKind::incVolume :
            iValue1 = parameters.Get<int>(&m_ParamPasVolume);
            iValue2 = m_KodiApi.GetVolume();
            m_KodiApi.SetVolume(iValue2+iValue1);
            break;

        case functionKind::decVolume :
            iValue1 = parameters.Get<int>(&m_ParamPasVolume);
            iValue2 = m_KodiApi.GetVolume();
            m_KodiApi.SetVolume(iValue2-iValue1);
            break;

        case functionKind::setVolume :
            iValue1 = parameters.Get<int>(&m_ParamVolume);
            m_KodiApi.SetVolume(iValue1);
            break;

        case functionKind::setPosition :
            iValue1 = parameters.Get<int>(&m_ParamHours);
            iValue2 = parameters.Get<int>(&m_ParamMinutes);
            m_KodiApi.SetPosition(iValue1, iValue2, 0);
            break;

        case functionKind::smallForward :
            m_KodiApi.Seek(KodiApi::smallforward);
            break;

        case functionKind::smallBackward :
            m_KodiApi.Seek(KodiApi::smallbackward);
            break;

        case functionKind::bigForward :
            m_KodiApi.Seek(KodiApi::bigforward);
            break;

        case functionKind::bigBackward :
            m_KodiApi.Seek(KodiApi::bigbackward);
            break;

        case functionKind::playMovie :
            iValue1 = parameters.Get<int>(&m_ParamFilm);
            m_KodiApi.MoviePlay(iValue1);
            break;
    }
}
