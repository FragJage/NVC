#ifndef KODIAPI_H
#define KODIAPI_H

#include<map>

class KodiApi
{
    public:
        enum eSeekWord {smallforward, smallbackward, bigforward, bigbackward};
        KodiApi();
        virtual ~KodiApi();

        void SetServer(std::string server, int port=80);

        bool Ping();
        std::string GetVersion();
        void PlayPause();
        void Stop();
        void Mute(bool active);
        int GetVolume();
        void SetVolume(int volume);
        void SetPosition(int hours, int minutes, int secondes);
        std::string GetPosition();
        void Seek(eSeekWord word);
        std::map<int, std::string> MovieList(int start, int end);
        void MoviePlay(int movieId);

    private:
        std::string SendToActivePlayer(std::string method, std::string params="");
        std::string SendCommand(std::string method, std::string params="", bool toActivePlayer=false);
        int GetActivePlayer();
        std::string m_Server;
        int m_ActivePlayer;

        int m_MaxListMovies;
};

#endif // KODIAPI_H
