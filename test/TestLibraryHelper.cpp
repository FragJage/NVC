#include "TestLibraryHelper.h"

using namespace std;

TestLibraryHelper::TestLibraryHelper() : TestClass("LibraryHelper", this)
{
	addTest("Load", &TestLibraryHelper::Load);
	addTest("GetProc", &TestLibraryHelper::GetProc);
	addTest("Free", &TestLibraryHelper::Free);
}

TestLibraryHelper::~TestLibraryHelper()
{
}

bool TestLibraryHelper::Load()
{
    bool ret;


    ret = m_MyLib.Load("UnExists");
    assert(false==ret);
    assert("Unable to load UnExists" == m_MyLib.GetLibraryError().substr(0,23));

    ret = m_MyLib.Load("Internal");
    assert(true==ret);

    return true;
}

bool TestLibraryHelper::GetProc()
{
    PROCHANDLE procHandle;
    LibraryHelper emptyLib;


    procHandle = emptyLib.GetProc("UnExists");
    assert(nullptr==procHandle);
    assert("You must load a library before search an entry point." == emptyLib.GetLibraryError());

    procHandle = m_MyLib.GetProc("UnExists");
    assert(nullptr==procHandle);
    assert("Unable to find UnExists in Internal." == m_MyLib.GetLibraryError().substr(0,36));

    procHandle = m_MyLib.GetProc("NvcPluginVersion");
    assert(nullptr!=procHandle);

    return true;
}

bool TestLibraryHelper::Free()
{
    m_MyLib.Free();
    return true;
}
