#include <sstream>
#include "PluginDetails.h"

using namespace std;
using namespace NVC;

PluginDetails::PluginDetails() : LibraryHelper()
{
    m_Name = "";
    m_LastError = "";
    m_PluginInstance = nullptr;
}

PluginDetails::PluginDetails(const PluginDetails& other) : LibraryHelper(other)
{
    m_Name = string(other.m_Name);
    m_LastError = string(other.m_LastError);
    m_PluginInstance  = other.m_PluginInstance;
    m_NewInstanceFunc = other.m_NewInstanceFunc;
    m_DelInstanceFunc = other.m_DelInstanceFunc;
    m_ExeInstanceFunc = other.m_ExeInstanceFunc;
}

PluginDetails::PluginDetails(PluginDetails&& other) : LibraryHelper(move(other))
{
    m_Name = move(other.m_Name);
    m_LastError = move(other.m_LastError);
    m_PluginInstance = other.m_PluginInstance;
    m_NewInstanceFunc = other.m_NewInstanceFunc;
    m_DelInstanceFunc = other.m_DelInstanceFunc;
    m_ExeInstanceFunc = other.m_ExeInstanceFunc;

    other.m_PluginInstance  = nullptr;
    other.m_NewInstanceFunc = nullptr;
    other.m_DelInstanceFunc = nullptr;
    other.m_ExeInstanceFunc = nullptr;
}

PluginDetails::~PluginDetails()
{
    Free();
}

void PluginDetails::Free()
{
    if(m_PluginInstance != nullptr)
    {
        try
        {
            m_DelInstanceFunc(m_PluginInstance);
        }
        catch(const exception& e)
        {

        }
    }
    LibraryHelper::Free();

    m_Name = "";
    m_LastError = "";
    m_PluginInstance = nullptr;
}

bool PluginDetails::Load(const string& name, PLUGIN_CALLBACK_FUNC callback)
{
    Free();

    if(!LibraryHelper::Load(name))
    {
        m_LastError = LibraryHelper::GetLibraryError();
        return false;
    }

    unsigned int version;
    PLUGIN_VERSION_FUNC funcVersion;
    funcVersion = LibraryHelper::GetProc<PLUGIN_VERSION_FUNC>("NvcPluginVersion");
    if(funcVersion == nullptr)
    {
        ostringstream msg;
        msg << name << " n'est pas un plugin pour NVC, la fonction NvcPluginVersion est introuvable.";
        m_LastError = msg.str();
        LibraryHelper::Free();
        return false;
    }

    version = funcVersion();
    if(version != NVC_PLUGIN_VERSION)
    {
        ostringstream msg;
        msg << "La version du plugin " << name << " (" << version << ") n'est pas compatible avec votre version de NVC (Attendu " << NVC_PLUGIN_VERSION << ").";
        m_LastError = msg.str();
        LibraryHelper::Free();
        return false;
    }

    m_NewInstanceFunc = LibraryHelper::GetProc<PLUGIN_NEWINSTANCE_FUNC>("NewInstance");
    if(m_NewInstanceFunc == nullptr)
    {
        ostringstream msg;
        msg << "Le plugin " << name << " est corrompu, la fonction NewInstance est introuvable.";
        m_LastError = msg.str();
        LibraryHelper::Free();
        return false;
    }

    m_DelInstanceFunc = LibraryHelper::GetProc<PLUGIN_DELETEINSTANCE_FUNC>("DeleteInstance");
    if(m_DelInstanceFunc == nullptr)
    {
        ostringstream msg;
        msg << "Le plugin " << name << " est corrompu, la fonction DeleteInstance est introuvable.";
        m_LastError = msg.str();
        LibraryHelper::Free();
        return false;
    }

    m_ExeInstanceFunc = LibraryHelper::GetProc<PLUGIN_EXECUTEINSTANCE_FUNC>("ExecuteInstance");
    if(m_ExeInstanceFunc == nullptr)
    {
        ostringstream msg;
        msg << "Le plugin " << name << " est corrompu, la fonction ExecuteInstance est introuvable.";
        m_LastError = msg.str();
        LibraryHelper::Free();
        return false;
    }

    try
    {
        m_PluginInstance = m_NewInstanceFunc(callback);
    }
    catch(const exception& e)
    {
        ostringstream msg;
        msg << "L'instanciation du plugin " << name << " a déclenché une exception : " << e.what();
        m_LastError = msg.str();
        LibraryHelper::Free();
        return false;
    }

    if(m_PluginInstance == nullptr)
    {
        ostringstream msg;
        msg << "L'instanciation du plugin " << name << " a échouée.";
        m_LastError = msg.str();
        LibraryHelper::Free();
        return false;
    }

    m_Name = name;
    return true;
}

PluginInformations* PluginDetails::GetPluginInformations()
{
    return (PluginInformations*) ExecuteInternal(InternalFunctionKind::GetInformations, nullptr);
}

PluginFunctionDesc* PluginDetails::GetFunction(unsigned int i)
{
    PluginInternalParameters param;

    param.Function = i;
    return (PluginFunctionDesc*) ExecuteInternal(InternalFunctionKind::GetFunction, &param);
}

PluginFunctionDesc* PluginDetails::GetFunction(const std::string& functionName)
{
    unsigned int i=0;
    PluginInternalParameters param;
    PluginFunctionDesc* pluginFunction;

    do
    {
        i++;
        param.Function = i;
        pluginFunction = (PluginFunctionDesc*) ExecuteInternal(InternalFunctionKind::GetFunction, &param);
    } while((pluginFunction!=nullptr)&&(pluginFunction->Name!=functionName));
    return pluginFunction;
}

PluginFunctionParameterDesc* PluginDetails::GetFunctionParameter(unsigned int functionNumber, unsigned int i)
{
    PluginInternalParameters param;

    param.Function = functionNumber;
    param.Parameter = i;
    return (PluginFunctionParameterDesc*) ExecuteInternal(InternalFunctionKind::GetFunctionParameter, &param);
}

PluginListDesc* PluginDetails::GetList(unsigned int i)
{
    PluginInternalParameters param;

    param.Function = i;
    return (PluginListDesc*) ExecuteInternal(InternalFunctionKind::GetList, &param);
}

map<string, string> PluginDetails::GetListItems(unsigned int i)
{
    PluginInternalParameters param;
    PluginListItem *pItem;
    PluginListItem *pCurrent;
    map<string, string> listItems;

    param.Parameter = 0;

    do
    {
        param.Function = i;
        pItem = (PluginListItem*) ExecuteInternal(InternalFunctionKind::GetListItems, &param);

        pCurrent = pItem;
        while(pCurrent != nullptr)
        {
            param.Parameter++;
            listItems[pCurrent->Description] = pCurrent->Code;
            pCurrent = pCurrent->Next;
        }

        if(pItem != nullptr)
        {
            param.Function = (int)pItem;
            ExecuteInternal(InternalFunctionKind::FreeListItems, &param);
        }
    } while(pItem!=nullptr);

    return listItems;
}

void PluginDetails::Execute(unsigned int functionNumber, PluginParameterHelper& params)
{
    PluginInternalParameters param;

    param.Function = functionNumber;
    param.PublicParameters = params.ToPublic();

    ExecuteInternal(InternalFunctionKind::Execute, &param);
}

void PluginDetails::Execute(unsigned int functionNumber)
{
    PluginInternalParameters param;

    param.Function = functionNumber;
    param.PublicParameters = nullptr;

    ExecuteInternal(InternalFunctionKind::Execute, &param);
}

void PluginDetails::Execute(unsigned int functionNumber, const char* params)
{
    PluginInternalParameters param;

    param.Function = functionNumber;
    param.PublicParameters = params;

    ExecuteInternal(InternalFunctionKind::Execute, &param);
}


string PluginDetails::GetPluginError()
{
    string error = m_LastError;
    m_LastError = "";
    return error;
}

void* PluginDetails::ExecuteInternal(InternalFunctionKind functionKind, PluginInternalParameters* param)
{
    void* ptr = nullptr;

    if(m_PluginInstance == nullptr)
        return nullptr;

    try
    {
        ptr = m_ExeInstanceFunc(m_PluginInstance, functionKind, param);
    }
    catch(const exception& e)
    {

    }

    return ptr;
}
