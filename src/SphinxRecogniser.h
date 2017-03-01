#ifndef SPHINXRECOGNISER_H
#define SPHINXRECOGNISER_H

#include <string>
#include <sphinxbase/ad.h>
#include <pocketsphinx.h>
#include "IVoiceRecogniser.h"
#include "IVoiceControl.h"


class SphinxRecogniser : public IVoiceRecogniser
{
    public:
        SphinxRecogniser(const std::string& languageFolder, const std::string& acousticFolder, const std:: string& dictionaryFileName);
        virtual ~SphinxRecogniser();

        bool SetArea(const std::string& areaName, IVoiceControl* voiceControl);
        virtual std::string ListenVoice();
        virtual std::string ListenFile(const std::string& fileName);

    private:
        void ClearConfig();
        int CheckWavHeader(char *header, int expected_sr);

        std::string m_LanguageFolder;
        std::string m_AcousticFolder;
        std::string m_DictionaryFileName;
        cmd_ln_t *m_config;
        ps_decoder_t *m_ps;
};

#endif // SPHINXRECOGNISER_H
