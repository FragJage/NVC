#include <sstream>
#include <algorithm>        //for find_if
#include <cstring>          //for memcpy
#include "IPluginImport.h"

namespace NVC
{

class IPlugin : public IPluginImport
{
	public:
	    enum LogLevel {LVL_FATAL, LVL_ERROR, LVL_WARNING, LVL_INFO, LVL_VERBOSE, LVL_DEBUG, LVL_TRACE};
		IPlugin(PLUGIN_CALLBACK_FUNC NVCCallback)
		{
		    m_NVCCallback = NVCCallback;
		}

		virtual ~IPlugin()
		{
		}

        virtual void Execute(unsigned int functionNumber, PluginParameterHelper& params)=0;
        virtual PluginListItem* GetListItems(unsigned int listNumber, unsigned int skip)=0;

        void SetPluginInformations(const char* name, const char* description, int majorVersion, int minorVersion)
        {
            m_PluginInformations.Name = name;
            m_PluginInformations.Description = description;
            m_PluginInformations.MajorVersion = majorVersion;
            m_PluginInformations.MinorVersion = minorVersion;
        }

        void AddPluginFunction(unsigned int number, const char* name, const char* description)
        {
            PluginFunctionDesc myFunction;
            myFunction.Number = number;
            myFunction.Name = name;
            myFunction.Description = description;
            m_PluginFunctions.emplace_back(myFunction);
        }

        void AddPluginFunction(unsigned int number, const char* name, const char* description, PluginFunctionParameterDesc* parameter)
        {
            AddPluginFunction(number, name, description, {parameter});
        }

        void AddPluginFunction(unsigned int number, const char* name, const char* description, std::initializer_list<PluginFunctionParameterDesc*> parameterList)
        {
            PluginFunctionDesc myFunction;
            myFunction.Number = number;
            myFunction.Name = name;
            myFunction.Description = description;
            for(PluginFunctionParameterDesc* param : parameterList)
            {
                myFunction.FunctionParameters.push_back(param);
            }
            m_PluginFunctions.emplace_back(myFunction);

        }

        void AddPluginList(unsigned int number, const char* name, const char* description, PluginParameterKind kind)
        {
            m_PluginLists.emplace_back(number, name, description, kind);
        }

        PluginListItem* AddListItems(std::string code, std::string description, PluginListItem* listItem)
        {
            PluginListItem* pItem = new PluginListItem();

            pItem->Code = new char[code.size()+1];
            pItem->Code[code.size()]=0;
            memcpy(pItem->Code,code.c_str(),code.size());

            pItem->Description = new char[description.size()+1];
            pItem->Description[description.size()]=0;
            memcpy(pItem->Description,description.c_str(),description.size());

            pItem->Next = nullptr;

            if(listItem != nullptr)
                listItem->Next = pItem;

            return pItem;
        }

        PluginListItem* AddListItems(int code, std::string description, PluginListItem* listItem)
        {
            std::ostringstream oss;
            oss << code;
            return AddListItems(oss.str(), description, listItem);
        }

		void* ExecuteInternal(InternalFunctionKind internalFunction, PluginInternalParameters* params)
		{
            PluginParameterHelper localParams;

            if(params != nullptr)
                localParams.FromPublic(params->PublicParameters);

		    switch(internalFunction)
		    {
                case InternalFunctionKind::GetInformations :
                    return &m_PluginInformations;

                case InternalFunctionKind::GetFunction :
                    return GetFunction(params->Function);

                case InternalFunctionKind::GetFunctionParameter :
                    return GetFunctionParameter(params->Function, params->Parameter);

                case InternalFunctionKind::Execute :
                    Execute(params->Function, localParams);
                    return nullptr;

                case InternalFunctionKind::GetList :
                    return GetList(params->Function);

                case InternalFunctionKind::GetListItems :
                    return GetListItems(params->Function, params->Parameter);

                case InternalFunctionKind::FreeListItems :
                    FreeListItems((PluginListItem*) params->Function);
                    return nullptr;
		    }
		    return nullptr;
		}

		int Callback_GetFunctionId(const char* param)
		{
		    return (int)NVCExecute(CallbackFunctionKind::CallbackFunctionId, 0, param);
		}

		PluginFunctionParameterDesc* Callback_GetFunctionParameter(int funcId, int parameterNb)
		{
		    std::ostringstream oss;
		    oss << parameterNb;
		    return (PluginFunctionParameterDesc*)NVCExecute(CallbackFunctionKind::CallbackFunctionParameter, funcId, oss.str().c_str());
		}

		void Callback_Execute(int funcId, const char* param)
		{
		    NVCExecute(CallbackFunctionKind::CallbackExecute, funcId, param);
		}

		void Callback_Log(LogLevel level, std::string msg)
		{
		    NVCExecute(CallbackFunctionKind::CallbackLog, level, msg.c_str());
		}

    private:
		void* NVCExecute(int func, int no, const char* param)
		{
		    return m_NVCCallback(m_PluginInformations.Name, func, no, param);
		}

		PluginFunctionDesc* GetFunction(unsigned int i)
		{
		    if(i<1) return nullptr;
		    if(i>m_PluginFunctions.size()) return nullptr;
		    return &(m_PluginFunctions[i-1]);
		}

		PluginListDesc* GetList(unsigned int i)
		{
		    if(i<1) return nullptr;
		    if(i>m_PluginLists.size()) return nullptr;
		    return &(m_PluginLists[i-1]);
		}

		PluginFunctionParameterDesc* GetFunctionParameter(unsigned int functionNumber, unsigned int i)
		{
		    if(i<1) return nullptr;

		    std::vector<PluginFunctionDesc>::iterator itEnd = m_PluginFunctions.end();
		    std::vector<PluginFunctionDesc>::iterator it = find_if(m_PluginFunctions.begin(), itEnd, [functionNumber] (const PluginFunctionDesc& f) { return f.Number == functionNumber; } );
		    if(it==itEnd) return nullptr;

		    if(i>it->FunctionParameters.size()) return nullptr;
		    return it->FunctionParameters[i-1];
		}

		void FreeListItems(PluginListItem* listItem)
		{
		    PluginListItem* current;
		    PluginListItem* next = listItem;

		    while(next != nullptr)
            {
                current = next;
                next = current->Next;

                delete [] current->Code;
                delete [] current->Description;
                delete current;
            }
		}

        PluginInformations m_PluginInformations;
        std::vector<PluginFunctionDesc> m_PluginFunctions;
        std::vector<PluginListDesc> m_PluginLists;
        PLUGIN_CALLBACK_FUNC m_NVCCallback;
};

}

#define DECLARE_PLUGIN(T) extern "C"{__declspec(dllexport) NVC::IPluginImport* NewInstance(NVC::PLUGIN_CALLBACK_FUNC f){ return new T(f);}}
extern "C"
{
	__declspec(dllexport) int NvcPluginVersion() { return NVC_PLUGIN_VERSION; }
	__declspec(dllexport) void DeleteInstance(NVC::IPluginImport* x) { delete x; }
	__declspec(dllexport) void *ExecuteInstance(NVC::IPluginImport* x, NVC::InternalFunctionKind internalFunction, NVC::PluginInternalParameters* params) { return x->ExecuteInternal(internalFunction, params); }
}

