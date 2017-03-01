#include <windows.h>

#ifdef BUILD_DLL
extern "C" __declspec(dllexport) BOOL APIENTRY DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH:
            // attach to process
            // return FALSE to fail DLL load
            break;

        case DLL_PROCESS_DETACH:
            // detach from process
            break;

        case DLL_THREAD_ATTACH:
            // attach to thread
            break;

        case DLL_THREAD_DETACH:
            // detach from thread
            break;
    }
    return TRUE; // succesful
}
#else
#include <iostream>
#include <map>
#include "KodiApi.h"

using namespace std;

int main()
{
    KodiApi kodiApi;
    int Volume;
    map<int, string> moviesList;
    map<int, string>::const_iterator it;

    //kodiApi.SetServer("192.168.0.10", 8080);
    kodiApi.SetServer("192.168.0.15");

    cout << "Ping : ";
    if(kodiApi.Ping())
        cout << "OK :-)" << endl;
    else
        cout << "KO :-(" << endl;

    cout << "Version : " << kodiApi.GetVersion() << endl;

    Volume = kodiApi.GetVolume();
    cout << "Volume : " << Volume << endl;
    kodiApi.SetVolume(Volume+5);
    cout << "Volume : " << kodiApi.GetVolume() << endl;
    kodiApi.SetVolume(Volume);
    cout << "Volume : " << kodiApi.GetVolume() << endl;

    moviesList = kodiApi.MovieList(0, 10);
    for(it=moviesList.begin(); it!=moviesList.end(); ++it)
        cout << it->first << " = " << it->second << endl;

    cout << "Play movie..." << endl;
    kodiApi.MoviePlay(28);
    cout << "Wait key" << endl;
    cin.get();

    cout << "Pause Movie..." << endl;
    kodiApi.PlayPause();
    cout << "Wait key" << endl;
    cin.get();

    cout << "Start Movie..." << endl;
    kodiApi.PlayPause();
    cout << "Wait key" << endl;
    cin.get();

    cout << "Position : ";
    cout << kodiApi.GetPosition() << endl;

    cout << "smallforward : ";
    kodiApi.Seek(KodiApi::smallforward);
    cout << kodiApi.GetPosition() << endl;

    cout << "bigforward : ";
    kodiApi.Seek(KodiApi::bigforward);
    cout << kodiApi.GetPosition() << endl;

    cout << "Set to 1:03 => ";
    kodiApi.SetPosition(1, 3, 0);
    cout << kodiApi.GetPosition() << endl;

    cout << "smallbackward : ";
    kodiApi.Seek(KodiApi::smallbackward);
    cout << kodiApi.GetPosition() << endl;

    cout << "bigbackward : ";
    kodiApi.Seek(KodiApi::bigbackward);
    cout << kodiApi.GetPosition() << endl;

    cout << "Stop Movie..." << endl;
    kodiApi.Stop();
    cout << "Ok" << endl;
}
#endif
