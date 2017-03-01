#ifndef PLUGINDETAILS_H
#define PLUGINDETAILS_H

#include <map>
#include "LibraryHelper.h"
#include "IPluginImport.h"

namespace NVC
{

typedef unsigned int (*PLUGIN_VERSION_FUNC)();
typedef IPluginImport* (*PLUGIN_NEWINSTANCE_FUNC)(PLUGIN_CALLBACK_FUNC);
typedef void (*PLUGIN_DELETEINSTANCE_FUNC)(IPluginImport*);
typedef void* (*PLUGIN_EXECUTEINSTANCE_FUNC)(IPluginImport*, InternalFunctionKind internalFunction, PluginInternalParameters* params);

class PluginDetails : private LibraryHelper
{
    public:
        PluginDetails();
        PluginDetails(const PluginDetails& other);
        PluginDetails(PluginDetails&& other);
        virtual ~PluginDetails();

        bool Load(const std::string& name, PLUGIN_CALLBACK_FUNC callback);
        void Free();
        PluginInformations* GetPluginInformations();
        PluginFunctionDesc* GetFunction(unsigned int i);
        PluginFunctionDesc* GetFunction(const std::string& functionName);
        PluginListDesc* GetList(unsigned int i);
        std::map<std::string, std::string> GetListItems(unsigned int i);
        PluginFunctionParameterDesc* GetFunctionParameter(unsigned int functionNumber, unsigned int i);
        void Execute(unsigned int functionNumber, PluginParameterHelper& params);
        void Execute(unsigned int functionNumber, const char* params);
        void Execute(unsigned int functionNumber);
        std::string GetPluginError();

    protected:

    private:
        void* ExecuteInternal(InternalFunctionKind functionKind, PluginInternalParameters* params);

        std::string m_Name;
        std::string m_LastError;
        IPluginImport* m_PluginInstance;
        PLUGIN_NEWINSTANCE_FUNC m_NewInstanceFunc;
        PLUGIN_DELETEINSTANCE_FUNC m_DelInstanceFunc;
        PLUGIN_EXECUTEINSTANCE_FUNC m_ExeInstanceFunc;
};

}
#endif // PLUGINDETAILS_H
