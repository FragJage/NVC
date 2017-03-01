#include <cassert>
#include "UnitTest/UnitTest.h"
#include "../src/LibraryHelper.h"

using namespace std;

class TestLibraryHelper : public TestClass<TestLibraryHelper>
{
public:
    TestLibraryHelper();
    ~TestLibraryHelper();
    bool Load();
    bool GetProc();
    bool Free();

private:
    LibraryHelper m_MyLib;
};
