#ifndef VOICECOMMAND_H
#define VOICECOMMAND_H

#include<string>

class VoiceCommand
{
    public:
        VoiceCommand();
        virtual ~VoiceCommand();
        VoiceCommand(const std::string& phrase, const std::string& command);

        const std::string& GetPhrase() const;
        const std::string& GetCommand() const;

    private:
        std::string m_Phrase;
        std::string m_Command;
};

#endif // VOICECOMMAND_H
