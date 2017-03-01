#include <windows.h>
#include <Servprov.h>
#include <sapi.h>

int main(int argc, char* argv[])
{
    ISpVoice * pVoice = NULL;

    if (FAILED(::CoInitialize(NULL)))
        return FALSE;

    HRESULT hr = CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_ALL, IID_ISpVoice, (void **)&pVoice);
    if( SUCCEEDED( hr ) )
    {
        hr = pVoice->Speak(L"Bonjour, je parle le fransais bien mieux que toi.", 0, NULL);
        hr = pVoice->Speak(L"Le son normal <pitch middle = '-10'/> mais avec une diminution du pitch.", SPF_IS_XML, NULL );
        hr = pVoice->Speak(L"Il est 13 heures cinquante et une.", 0, NULL);
        pVoice->Release();
        pVoice = NULL;
    }
    ::CoUninitialize();
    return TRUE;
}
