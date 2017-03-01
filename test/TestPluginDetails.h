#include <cassert>
#include "UnitTest/UnitTest.h"
#include "../src/PluginDetails.h"

using namespace std;

class TestPluginDetails : public TestClass<TestPluginDetails>
{
public:
    TestPluginDetails();
    ~TestPluginDetails();
    bool Load();
    bool Free();
private:
    NVC::PluginDetails m_MyPlugin;
};
