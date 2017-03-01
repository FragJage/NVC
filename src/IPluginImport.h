#ifndef PLUGINIMPORT_H
#define PLUGINIMPORT_H

#include <string>
#include <sstream>
#include <vector>
#include <map>

namespace NVC
{

#define NVC_PLUGIN_VERSION 100

typedef void* (*PLUGIN_CALLBACK_FUNC)(const char*, unsigned int, unsigned int, const char*);

enum PluginParameterKind { NVCString, NVCInteger };
enum InternalFunctionKind {GetInformations, GetFunction, GetFunctionParameter, Execute, GetList, GetListItems, FreeListItems};
enum CallbackFunctionKind {CallbackFunctionId, CallbackFunctionParameter, CallbackExecute, CallbackLog};

struct PluginInformations
{
    const char* Name;
    const char* Description;
    unsigned int MajorVersion;
    unsigned int MinorVersion;
};

struct PluginFunctionParameterDesc
{
    void Set(const char* name, const char* description, PluginParameterKind kind, int associedList=-1)
    {
        Name = name;
        Description = description;
        Kind = kind;
        AssociedList = associedList;
    }

    const char* Name;
    const char* Description;
    PluginParameterKind Kind;
    int AssociedList;
    int Reserved;
};

struct PluginFunctionDesc
{
    unsigned int Number;
    const char* Name;
    const char* Description;
    std::vector<PluginFunctionParameterDesc*> FunctionParameters;
};

struct PluginListDesc
{
    unsigned int Number;
    const char* Name;
    const char* Description;
    PluginParameterKind Kind;

    PluginListDesc(unsigned int number, const char* name, const char* description, PluginParameterKind kind)
    {
        Number = number;
        Name = name;
        Description = description;
        Kind = kind;
    }
};

struct PluginListItem
{
    char* Code;
    char* Description;
    PluginListItem* Next;
};

struct PluginInternalParameters
{
    unsigned int Function;
    unsigned int Parameter;
    const char* PublicParameters;
};

class PluginParameterHelper
{
	public:
        PluginParameterHelper()
        {
        }

        virtual ~PluginParameterHelper()
        {
            Clear();
        }

        void Add(PluginFunctionParameterDesc* functionParameter, std::string value)
        {
            if(functionParameter==nullptr) return;
            m_ParameterList[functionParameter->Name] = value;
        }

        void Add(PluginFunctionParameterDesc* functionParameter, int value)
        {
            if(functionParameter==nullptr) return;
            std::ostringstream sval;
            sval << value;
            m_ParameterList[functionParameter->Name] = sval.str();
        }

        std::string Get(PluginFunctionParameterDesc* functionParameter)
        {
            if(functionParameter==nullptr) return "";
            std::map<std::string, std::string>::iterator it = m_ParameterList.find(functionParameter->Name);
            if(it==m_ParameterList.end())
                return "";
            return it->second;
        }

        template<typename T>
        T Get(PluginFunctionParameterDesc* functionParameter)
        {
            T tvalue;
            std::istringstream svalue(Get(functionParameter));

            svalue >> tvalue;
            return tvalue;
        }

        void Clear()
        {
            m_ParameterList.clear();
        }

        const char* ToPublic()
        {
            std::map<std::string, std::string>::iterator it = m_ParameterList.begin();
            std::map<std::string, std::string>::iterator itEnd = m_ParameterList.end();
            std::ostringstream stringList;

            while(it != itEnd)
            {
                stringList << it->first << ":" << it->second << "\n";
                ++it;
            }
            m_PublicString = stringList.str();
            return m_PublicString.c_str();
        }

        void FromPublic(const char *publicString)
        {
            if(publicString==nullptr) return;

            std::istringstream f(publicString);
            std::string line;
            size_t pos;
            while (std::getline(f, line))
            {
                pos = line.find_first_of(':');
                if(pos!=std::string::npos)
                    m_ParameterList[line.substr(0, pos)] = line.substr(pos+1, line.length()-pos);
            }
        }

    private:
        std::string m_PublicString;
        std::map<std::string, std::string> m_ParameterList;
};

class IPluginImport
{
	public:
		IPluginImport(){}
		virtual ~IPluginImport(){}
		virtual void* ExecuteInternal(InternalFunctionKind internalFunction, PluginInternalParameters* params)=0;
};

}

#endif // PLUGINIMPORT_H
