#ifndef IINTENTPARSER_H
#define IINTENTPARSER_H

#include "IVoiceControl.h"

struct IntentFound
{
    int CommandId;
    std::vector<std::string> Parameters;

    bool IsNull()
    {
        return (CommandId==-1);
    }
};

class IIntentParser
{
    public:
        IIntentParser() {}
        virtual ~IIntentParser() {}

        virtual bool SetArea(const std::string& areaName, IVoiceControl* voiceControl)=0;
        virtual IntentFound Parse(const std::string& phrase)=0;
};
#endif
