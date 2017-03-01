#ifndef LIBRARYHELPER_H
#define LIBRARYHELPER_H

#include <string>

#ifdef WIN32
    #include <windows.h>
    #define LIBHANDLE HINSTANCE
    #define PROCHANDLE FARPROC WINAPI
#else
    #include <dlfcn.h>
    #define LIBHANDLE void *
    #define PROCHANDLE void *
#endif

class LibraryHelper
{
    public:
        LibraryHelper();
        LibraryHelper(const LibraryHelper& other);
        LibraryHelper(LibraryHelper&& other);
        virtual ~LibraryHelper();

        bool Load(const std::string& name);
        PROCHANDLE GetProc(const std::string& proc);
        template <typename T> T GetProc(const std::string& proc)
        {
            return reinterpret_cast<T>(GetProc(proc));
        }
        void Free();
        std::string GetLibraryError();

    protected:

    private:
        std::string GetSystemError();
        void SetLibraryError(const std::string& msg, bool addSystemError);
        LIBHANDLE m_Handle;
        std::string m_Name;
        std::string m_LastError;
};

#endif // LIBRARYHELPER_H
