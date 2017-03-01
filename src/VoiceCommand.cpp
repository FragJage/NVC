#include "VoiceCommand.h"

using namespace std;

VoiceCommand::VoiceCommand()
{
    //ctor
}

VoiceCommand::~VoiceCommand()
{
    //dtor
}

VoiceCommand::VoiceCommand(const string& phrase, const string& command)
{
    m_Phrase = phrase;
    m_Command = command;
}

const string& VoiceCommand::GetPhrase() const
{
    return m_Phrase;
}

const std::string& VoiceCommand::GetCommand() const
{
    return m_Command;
}
