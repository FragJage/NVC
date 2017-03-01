#ifndef IVOICECONTROL_H
#define IVOICECONTROL_H

#include "VoiceCommand.h"

class IVoiceControl
{
    public:
        IVoiceControl() {}
        virtual ~IVoiceControl() {}

        virtual const std::vector<VoiceCommand>* GetVoiceCommands(const std::string& area)=0;
        virtual const std::string& GetBotName()=0;
        virtual const std::vector<std::string> GetList(std::string listName)=0;
        virtual int GetCommandId(const std::string& command)=0;
};
#endif  //IVOICECONTROL_H
