#ifndef PLUGINFACTORY_H
#define PLUGINFACTORY_H

#include <string>
#include <map>
#include <SimpleLog/SimpleLog.h>
#include "PluginDetails.h"


namespace NVC
{

struct PluginFunctionParameter
{
    std::string Name;
    PluginParameterKind Kind;
    int AssociedList;

    PluginFunctionParameter(std::string name, PluginParameterKind kind, int associedList)
    {
        Name = name;
        Kind = kind;
        AssociedList = associedList;
    }
};

struct PluginFunction
{
    PluginDetails* Plugin;
    unsigned int Number;
    std::string Name;
    std::vector<PluginFunctionParameterDesc*> Parameters;

    PluginFunction(PluginDetails* plugin, std::string name, unsigned int number, std::vector<PluginFunctionParameterDesc*> parameters)
    {
        Plugin = plugin;
        Name = name;
        Number = number;
        Parameters = parameters;
    }
};

struct PluginList
{
    PluginDetails* Plugin;
    unsigned int Number;
    std::string Name;

    PluginList(PluginDetails* plugin, std::string name, unsigned int number)
    {
        Plugin = plugin;
        Name = name;
        Number = number;
    }
};

class PluginFactory
{
    public:
        enum PluginErrorKind {Ok, PluginNotFound, CommandNotFound, MandatoryParameter, WrongTypeParameter};
        PluginFactory(SimpleLog* log);
        virtual ~PluginFactory();

        void SetCallback(PLUGIN_CALLBACK_FUNC callback);
        bool Add(const std::string& pluginName);
        int GetFunctionId(const std::string& functionName);
        const std::vector<PluginFunctionParameterDesc*>& GetFunctionParameters(int functionId);
        void Execute(int functionId, PluginParameterHelper& params);
        void Execute(int functionId, const char* params);
        //PluginErrorKind Test(const std::string& pluginName, const std::string& command, std::initializer_list<PluginFunctionParameterDesc*> parameterList);
        //PluginErrorKind Execute(const std::string& pluginName, const std::string& command, PluginParameterHelper& params);
        std::string GetLastPluginError();
        const std::vector<std::string>& GetList(const std::string& listName);
        std::string GetListCode(int listIndex, const std::string& value);

    protected:

    private:
        std::string lowerFilter(const std::string& command);
        //PluginDetails* GetPlugin(const std::string& pluginName);
        std::vector<PluginFunction> m_PluginFunctions;
        std::vector<PluginList> m_PluginLists;
        std::map<std::string, std::vector<std::string>> m_ContentLists;
        std::map<std::string, std::map<std::string, std::string>> m_SearchLists;
        std::vector<std::string> m_BlankLists;
        std::map<std::string, PluginDetails> m_PluginsDetails;
        PLUGIN_CALLBACK_FUNC m_Callback;
        std::string m_LastError;
        SimpleLog* m_Log;
};

}
#endif // PLUGINFACTORY_H
