#include <iostream>
#include <map>
#include <sstream>
#include <cstring>

#include <ao/ao.h>

extern "C"
{
#include "svoxpico/picoapi.h"
#include "svoxpico/picoapid.h"
#include "svoxpico/picoos.h"
}

class Pico
{
private:
    pico_Char *         local_text;

    int             m_Speed;
    int             m_Pitch;
    int             m_Volume;
    std::string     m_ResourcePath;
    std::string     m_LastError;

    void*           m_PicoMemArea;
    pico_System     m_PicoSystem;
    pico_Resource   m_PicoTaResource;
    pico_Resource   m_PicoSgResource;
    pico_Engine     m_PicoEngine;

    std::string     m_VoiceLang;
    const std::map<std::string, std::string> m_VoiceParts = {
        { "de-DE", "gl0"},
        { "en-GB", "kh0"},
        { "en-US", "lh0"},
        { "es-ES", "zl0"},
        { "fr-FR", "nk0"},
        { "it-IT", "cm0"}
    };
    const std::string     m_VoiceName = "DefaultVoice";
    pico_Char*            m_PicoVoiceName;

    ao_sample_format    m_PcmFormat;
    ao_device*          m_PcmDevice;
    int                 m_PcmDriver;

    const int       OUT_BUFFER_SIZE = 256;
    const int       PCM_BUFFER_SIZE = 512;
    const int       WRK_BUFFER_SIZE = 2500000;

    void SetPicoError(std::string text, int errorNo);

public:
    Pico() ;
    virtual ~Pico() ;

    void SetPath(const std::string& path);
    bool SetVoice(const std::string& lang);
    void SetSpeed(float val);
    void SetPitch(float val);
    void SetVolume(float val);
    std::string GetPicoError();

    bool InitializeSystem();
    bool Process(const std::string &text);
    void Cleanup() ;
};
