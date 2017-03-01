#include "LibraryHelper.h"

using namespace std;

LibraryHelper::LibraryHelper()
{
    m_Handle = nullptr;
    m_Name = "";
    m_LastError = "";
}

LibraryHelper::LibraryHelper(const LibraryHelper& other)
{
    m_Handle = other.m_Handle;
    m_Name = other.m_Name;
    m_LastError = other.m_LastError;
}

LibraryHelper::LibraryHelper(LibraryHelper&& other)
{
    m_Handle = other.m_Handle;
    m_Name = move(other.m_Name);
    m_LastError = move(other.m_LastError);
    other.m_Handle = nullptr;
}

LibraryHelper::~LibraryHelper()
{
    Free();
}

bool LibraryHelper::Load(const string& name)
{
    string filename;

    if(m_Handle!=nullptr) Free();
    #ifdef WIN32
        filename = name+".dll";
        m_Handle = LoadLibrary(filename.c_str());
    #else
        filename = name+".so";
        m_Handle = dlopen(filename.c_str(), RTLD_LAZY)
    #endif
    m_Name = name;
    if(m_Handle == nullptr)
        SetLibraryError("Unable to load "+filename+".", true);
    return (m_Handle != nullptr);
}

PROCHANDLE LibraryHelper::GetProc(const string& proc)
{
    PROCHANDLE procHandle;

    if(m_Handle==nullptr)
    {
        SetLibraryError("You must load a library before search an entry point.", false);
        return nullptr;
    }
    #ifdef WIN32
        procHandle = GetProcAddress(m_Handle, proc.c_str());
    #else
        procHandle = dlsym(m_Handle, proc.c_str());
    #endif

    if(procHandle == nullptr)
        SetLibraryError("Unable to find "+proc+" in "+m_Name+".", true);
    return procHandle;
}

string LibraryHelper::GetLibraryError()
{
    string error = m_LastError;
    m_LastError = "";
    return error;
}

std::string LibraryHelper::GetSystemError()
{
    #ifdef WIN32
        DWORD errorMessageID = GetLastError();
        if(errorMessageID == 0)
            return string();

        LPSTR messageBuffer = nullptr;
        size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                 NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

        string message(messageBuffer, size);
        LocalFree(messageBuffer);

        return message;
    #else
        return dlerror();
    #endif
}

void LibraryHelper::SetLibraryError(const string& msg, bool addSystemError)
{
    m_LastError = msg;
    if(addSystemError)
        m_LastError += " "+GetSystemError();
}

void LibraryHelper::Free()
{
    if(m_Handle == nullptr) return;
    #ifdef WIN32
        FreeLibrary(m_Handle);
    #else
        dlclose(m_Handle);
    #endif
    m_Handle = nullptr;
    m_Name = "";
}
