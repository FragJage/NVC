#ifndef IVOICERECOGNISER_H
#define IVOICERECOGNISER_H

#include "IVoiceControl.h"

class IVoiceRecogniser
{
    public:
        IVoiceRecogniser() {}
        virtual ~IVoiceRecogniser() {}

        virtual std::string ListenVoice()=0;
        virtual bool SetArea(const std::string& areaName, IVoiceControl* voiceControl)=0;
        virtual std::string ListenFile(const std::string& fileName)=0;
};
#endif
