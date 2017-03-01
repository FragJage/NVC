#include "TestPluginDetails.h"

using namespace std;

TestPluginDetails::TestPluginDetails() : TestClass("PluginDetails", this)
{
	addTest("Load", &TestPluginDetails::Load);
	addTest("Free", &TestPluginDetails::Free);
}

TestPluginDetails::~TestPluginDetails()
{
}

bool TestPluginDetails::Load()
{
    bool ret;


    ret = m_MyPlugin.Load("UnExists", nullptr);
    assert(false==ret);
    //assert("Unable to load UnExists" == m_MyPlugin.GetLibraryError().substr(0,23));

    ret = m_MyPlugin.Load("Internal", nullptr);
    assert(true==ret);

    return true;
}

bool TestPluginDetails::Free()
{
    m_MyPlugin.Free();
    return true;
}
