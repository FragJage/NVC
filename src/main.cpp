//TODO LIST
// - Identifier le probl�me du TTS avec les accents
// - G�rer le param�trage des plugins 
// - Plugin kodi � compl�ter (liste film par acteur, gestion des musiques)
// - Activer la reconnaissance par mot cl� pour le nom du bot
// - Plugin Voix Microsoft
// - Pouvoir basculer entre MSVoice et PicoVoice
// - G�rer les variables globales/environnementales
// - Activer la reconnaissance d'emprunte vocale
// - G�n�ration dynamique des mots inconnu dans le dictionnaire
// - Conversion des chiffres (romain aussi) en lettres pour la reconnaissance vocales
// - Plugin mail
// - Plugin agenda
// - Switch d'aera
// - Plugin m�t�o
// - Plugin 1-wire
// - Support de Google pour la reconnaissance vocale
// - Support de LUIS ou ??.ai pour le parser d'intentions
// - G�rer des �v�nements/t�ches r�p�titives/cron/trigger

#include <iostream>
#include "NickyVoiceControl.h"
#include "SphinxRecogniser.h"
#include "FragParser.h"

using namespace std;

typedef NVC::PluginInformations* (*PLUGIN_GETINFO_FUNC)(NVC::IPluginImport*);
NickyVoiceControl Nvc;

extern "C"
{
	__declspec(dllexport) void* NVCExecute(const char* plugin, unsigned int f, unsigned int no, const char* param)
	{
	    switch(f)
	    {
            case NVC::CallbackFunctionKind::CallbackFunctionId :
                return (void *)Nvc.ForCallback_GetFunctionId(plugin, param);
	        case NVC::CallbackFunctionKind::CallbackFunctionParameter :
                return Nvc.ForCallback_GetFunctionParameter(plugin, no, atoi(param));
	        case NVC::CallbackFunctionKind::CallbackExecute :
                Nvc.ForCallback_ExecutePlugin(plugin, no, param);
                return nullptr;
	    }
        return nullptr;
    }
}

int main()
{
    SphinxRecogniser myRecogniser("french_f0", "french_f0", "frenchWords62K.dic");
    FragParser myParser;


    Nvc.SetCallBack(NVCExecute);

    if(!Nvc.LoadSettings("settings-light.json"))
        return -1;

    Nvc.LoadPlugins();

    if(!Nvc.CheckSettings())
        return -3;

    ///Initialisation de la reconnaissance vocale
    Nvc.SetVoiceRecogniser(&myRecogniser);

    ///Initialisation du parser d'intention
    Nvc.SetIntentParser(&myParser);

    ///D�codage
    return Nvc.Listen();
}
