#include <iostream>

#include <algorithm>
#include "PluginFactory.h"

using namespace std;
using namespace NVC;

PluginFactory::PluginFactory(SimpleLog* log)
{
    m_Callback = nullptr;
    m_LastError = "";
    m_Log = log;
}

PluginFactory::~PluginFactory()
{
    std::map<std::string, PluginDetails>::iterator it = m_PluginsDetails.begin();
    std::map<std::string, PluginDetails>::iterator itEnd = m_PluginsDetails.end();

    while(it!=itEnd)
    {
        it->second.Free();
        ++it;
    }
}

void PluginFactory::SetCallback(PLUGIN_CALLBACK_FUNC callback)
{
    m_Callback = callback;
}

bool PluginFactory::Add(const std::string& pluginName)
{
    unsigned int i;
    PluginDetails pluginDetails;
    PluginDetails* plugin;
    PluginFunctionDesc* pluginFunctionDesc;
    PluginFunctionParameterDesc* parameterDesc;
    PluginListDesc* pluginListDesc;
    map<unsigned int, int> number2order;


    LOG_DEBUG(m_Log) << "*** ENTER ***";
    LOG_VERBOSE(m_Log) << "Load plugin : " << pluginName;

    if(!pluginDetails.Load(pluginName, m_Callback))
    {
        m_LastError = pluginDetails.GetPluginError();
        LOG_DEBUG(m_Log) << "*** EXIT with error ***";
        return false;
    }

    m_PluginsDetails.emplace(pluginName, move(pluginDetails));
    plugin = &(m_PluginsDetails[pluginName]);

    i = 1;
    while((pluginListDesc = plugin->GetList(i))!=nullptr)
    {
        number2order[pluginListDesc->Number] = m_PluginLists.size();
        m_PluginLists.emplace_back(plugin, pluginName+"."+pluginListDesc->Name, pluginListDesc->Number);
        LOG_VERBOSE(m_Log) << "Add list : " << m_PluginLists.size()-1 << " - " << pluginName+"."+pluginListDesc->Name;
        i++;
    }

    i = 1;
    while((pluginFunctionDesc = plugin->GetFunction(i))!=nullptr)
    {
        unsigned int j = 1;
        vector<PluginFunctionParameterDesc*> parameters;
        while((parameterDesc = plugin->GetFunctionParameter(pluginFunctionDesc->Number,j))!=nullptr)
        {
            if(parameterDesc->AssociedList != -1)
                parameterDesc->Reserved = number2order[parameterDesc->AssociedList];
            parameters.push_back(parameterDesc);
            j++;
        }
        m_PluginFunctions.emplace_back(plugin, pluginName+"."+pluginFunctionDesc->Name, pluginFunctionDesc->Number, parameters);
        LOG_VERBOSE(m_Log) << "Add function : " << m_PluginFunctions.size()-1 << " - " << pluginName+"."+pluginFunctionDesc->Name << " " << parameters.size() << " params";
        i++;
    }

    LOG_DEBUG(m_Log) << "*** EXIT with succes ***";
    return true;
}

int PluginFactory::GetFunctionId(const string& functionName)
{
    vector<PluginFunction>::iterator itBeg = m_PluginFunctions.begin();
    vector<PluginFunction>::iterator itEnd = m_PluginFunctions.end();
    vector<PluginFunction>::iterator it = find_if(itBeg, itEnd, [&] (PluginFunction const& f) { return f.Name == functionName; });
    if(it==itEnd)
    {
        LOG_ERROR(m_Log) << "Function " << functionName << " not found";
        return -1;
    }
    return distance(itBeg, it);
}

const vector<PluginFunctionParameterDesc*>& PluginFactory::GetFunctionParameters(int functionId)
{
    return m_PluginFunctions[functionId].Parameters;
}

void PluginFactory::Execute(int functionId, PluginParameterHelper& params)
{
    m_PluginFunctions[functionId].Plugin->Execute(m_PluginFunctions[functionId].Number, params);
}

void PluginFactory::Execute(int functionId, const char* params)
{
    m_PluginFunctions[functionId].Plugin->Execute(m_PluginFunctions[functionId].Number, params);
}

const vector<string>& PluginFactory::GetList(const string& listName)
{
    map<string, vector<string>>::const_iterator it1 = m_ContentLists.find(listName);
    if(it1!=m_ContentLists.end())
        return it1->second;

    vector<PluginList>::iterator itBeg = m_PluginLists.begin();
    vector<PluginList>::iterator itEnd = m_PluginLists.end();
    vector<PluginList>::iterator it2 = find_if(itBeg, itEnd, [&] (PluginList const& l) { return l.Name == listName; });

    if(it2==itEnd)
    {
        LOG_ERROR(m_Log) << "List " << listName << " not found";
        return m_BlankLists;
    }

    map<string, string> tmp = it2->Plugin->GetListItems(it2->Number);
    map<string, string>::iterator tmpIt = tmp.begin();
    map<string, string>::iterator tmpEnd = tmp.end();
    while(tmpIt != tmpEnd)
    {
        m_ContentLists[listName].push_back(tmpIt->first);
        m_SearchLists[listName].emplace(lowerFilter(tmpIt->first), tmpIt->second);
        ++tmpIt;
    }

    return m_ContentLists[listName];
}

string PluginFactory::GetListCode(int listIndex, const string& value)
{
    string listName = m_PluginLists[listIndex].Name;
    map<string, map<string, string>>::const_iterator it1 = m_SearchLists.find(listName);
    if(it1==m_SearchLists.end())
    {
        LOG_ERROR(m_Log) << "List " << listName << " not found or not read";
        return "";
    }

    const map<string, string>::const_iterator it2 = it1->second.find(lowerFilter(value));
    if(it2==it1->second.end())
    {
        LOG_ERROR(m_Log) << value << " not found in list " << listName;
        return "";
    }

    return it2->second;
}

string PluginFactory::lowerFilter(const string& command)
{
    char c;
    ostringstream cmdOss;


    for(size_t i=0; i<command.size(); i++)
    {
        c = command[i];
        switch(c)
        {
            case '?' :
            case '!' :
            case '/' :
            case '\'' :
            case '-' :
            case ',' :
            case ';' :
            case ':' :
            case '.' :
            case ' ' :
                c = ' ';
                break;
            default :
                cmdOss << tolower(c);
        }
    }

    return cmdOss.str();
}

/*
PluginDetails* PluginFactory::GetPlugin(const std::string& pluginName)
{
    std::map<std::string, PluginDetails>::iterator it = m_PluginsDetails.find(pluginName);
    if(it==m_PluginsDetails.end())
        return nullptr;

    return &(it->second);
}

PluginFactory::PluginErrorKind PluginFactory::Test(const std::string& pluginName, const std::string& command, std::initializer_list<PluginFunctionParameterDesc*> parameterList)
{
    PluginDetails* plugin;
    PluginFunctionDesc* func;

    plugin = GetPlugin(pluginName);
    if(plugin == nullptr)
        return PluginErrorKind::PluginNotFound;

    func = plugin->GetFunction(command);
    if(func == nullptr)
        return PluginErrorKind::CommandNotFound;


    map<string, PluginFunctionParameterDesc*> paramsTest;

    for(PluginFunctionParameterDesc* param : parameterList)
        paramsTest[param->Name] = param;


    map<string, PluginFunctionParameterDesc*>::iterator it;
    map<string, PluginFunctionParameterDesc*>::iterator itEnd = paramsTest.end();
    PluginFunctionParameterDesc* pfp;
    unsigned short i = 1;
    PluginErrorKind error = PluginErrorKind::Ok;

    while((pfp = plugin->GetFunctionParameter(func->Number, i))!=nullptr)
    {
        it = paramsTest.find(pfp->Name);
        if(it == itEnd)
        {
            error = PluginErrorKind::MandatoryParameter;
            m_LastError = pfp->Name;
            break;
        }
        if(it->second->Kind != pfp->Kind)
        {
            error = PluginErrorKind::WrongTypeParameter;
            m_LastError = pfp->Name;
            break;
        }
        i++;
    }

    return error;
}

PluginFactory::PluginErrorKind PluginFactory::Execute(const std::string& pluginName, const std::string& command, PluginParameterHelper& params)
{
    PluginDetails* plugin;
    PluginFunctionDesc* func;

    plugin = GetPlugin(pluginName);
    if(plugin == nullptr)
        return PluginErrorKind::PluginNotFound;

    func = plugin->GetFunction(command);
    if(func == nullptr)
        return PluginErrorKind::CommandNotFound;

    plugin->Execute(func->Number, params);

    return PluginErrorKind::Ok;
}
*/
std::string PluginFactory::GetLastPluginError()
{
    string error = m_LastError;
    m_LastError = "";
    return error;
}
