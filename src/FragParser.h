#ifndef FRAGPARSER_H
#define FRAGPARSER_H

#include <regex>
#include "IIntentParser.h"
#include "IVoiceControl.h"


struct IntentItem
{
    int CommandId;
    std::regex Pattern;
    std::vector<std::string> Parameters;

    IntentItem(int commandId, std::string pattern)
    {
        CommandId = commandId;
        Pattern = pattern;
    }
};

class FragParser : public IIntentParser
{
    public:
        FragParser();
        virtual ~FragParser();

        void IntentAdd(int commandId, const std::string& phrase, const std::string& command);
        IntentFound Parse(const std::string& phrase);
        bool SetArea(const std::string& areaName, IVoiceControl* voiceControl);

    private:
        std::vector<IntentItem> m_IntentItems;
};

#endif // FRAGPARSER_H
