#include <sstream>
#include "json/json.h"
#include "HttpRequest/HttpRequest.h"
#include "HttpRequest/HttpResponse.h"
#include "KodiApi.h"

using namespace std;

KodiApi::KodiApi()
{
    m_ActivePlayer = -1;
    m_MaxListMovies= -1;
}

KodiApi::~KodiApi()
{
}

void KodiApi::SetServer(string server, int port)
{
    ostringstream oss;

    oss << server;

    if(port!=80)
        oss << ":" << port;

    m_Server = oss.str();
}

bool KodiApi::Ping()
{
    return (SendCommand("JSONRPC.Ping")!="");
}

string KodiApi::GetVersion()
{
    string response = SendCommand("JSONRPC.Version");
    ostringstream oss;
    Json::Reader reader;
    Json::Value root;


    if(!reader.parse(response, root))
        throw logic_error("Failed to parse JSONRPC.Version response : "+reader.getFormattedErrorMessages());

    oss << root["result"]["version"]["major"].asString();
    oss << ".";
    oss << root["result"]["version"]["minor"].asString();
    oss << ".";
    oss << root["result"]["version"]["patch"].asString();

    return oss.str();
}

void KodiApi::PlayPause()
{
    SendToActivePlayer("Player.PlayPause");
}

void KodiApi::Stop()
{
    SendToActivePlayer("Player.Stop");
    m_ActivePlayer = -1;
}

void KodiApi::Mute(bool active)
{
    if(active)
        SendCommand("Application.SetMute", "\"mute\":true");
    else
        SendCommand("Application.SetMute", "\"mute\":false");
}

int KodiApi::GetVolume()
{
    string response = SendCommand("Application.GetProperties", "\"properties\":[\"volume\"]");
    Json::Reader reader;
    Json::Value root;


    if(!reader.parse(response, root))
        throw logic_error("Failed to parse Application.GetProperties [volume] response : "+reader.getFormattedErrorMessages());

    return root["result"]["volume"].asInt();
}


void KodiApi::SetVolume(int volume)
{
    ostringstream oss;

    oss << "\"volume\":" << volume;
    SendCommand("Application.SetVolume", oss.str());
}

void KodiApi::SetPosition(int hours, int minutes, int secondes)
{
    ostringstream oss;

    oss << "\"value\":{";
    oss << "\"milliseconds\":0,";
    oss << "\"seconds\":" << secondes << ",";
    oss << "\"minutes\":" << minutes << ",";
    oss << "\"hours\":" << hours << "}";
    SendToActivePlayer("Player.Seek", oss.str());
}

std::string KodiApi::GetPosition()
{
    string response = SendToActivePlayer("Player.GetProperties", "\"properties\":[\"time\"]");
    ostringstream oss;
    Json::Reader reader;
    Json::Value root;


    if(!reader.parse(response, root))
        throw logic_error("Failed to parse Application.GetProperties [time] response : "+reader.getFormattedErrorMessages());

    oss << root["result"]["time"]["hours"].asInt() << ":";
    oss << root["result"]["time"]["minutes"].asInt() << ":";
    oss << root["result"]["time"]["seconds"].asInt();
    return oss.str();
}

void KodiApi::Seek(eSeekWord word)
{
    ostringstream oss;
    string predef;


    switch(word)
    {
        case smallforward :
            predef = "smallforward";
            break;
        case smallbackward :
            predef = "smallbackward";
            break;
        case bigforward :
            predef = "bigforward";
            break;
        case bigbackward :
            predef = "bigbackward";
            break;
    }

    oss << "\"value\":\"" << predef << "\"";
    SendToActivePlayer("Player.Seek", oss.str());
}

map<int, string> KodiApi::MovieList(int istart, int iend)
{
    ostringstream oss;
    string response;
    map<int, string> moviesList;

    Json::Reader reader;
    Json::Value root;
    Json::Value jsValue;

    if((istart>0)&&(istart>=m_MaxListMovies))
        return moviesList;

    oss << "\"limits\":{\"start\":" << istart << ",\"end\":" << iend << "},";
    oss << "\"sort\":{\"order\":\"ascending\",\"method\":\"label\",\"ignorearticle\":false}";
    response = SendCommand("VideoLibrary.GetMovies", oss.str());

    if(!reader.parse(response, root))
        throw logic_error("Failed to parse VideoLibrary.GetMovies response : "+reader.getFormattedErrorMessages());

    if(istart==0)
        m_MaxListMovies = root["result"]["limits"]["total"].asInt();

    jsValue = root["result"];
    jsValue = jsValue["movies"];
    if(jsValue.isArray())
    {
        for( Json::ValueIterator itr = jsValue.begin() ; itr != jsValue.end() ; itr++ )
        {
            moviesList[(*itr)["movieid"].asInt()] = (*itr)["label"].asString();
        }
    }

    return moviesList;
}

void KodiApi::MoviePlay(int movieId)
{
    ostringstream oss;


    oss << "\"item\":{\"movieid\":" << movieId << "}";
    SendCommand("Player.Open", oss.str());
    m_ActivePlayer = GetActivePlayer();
}

int KodiApi::GetActivePlayer()
{
    string response;
    Json::Reader reader;
    Json::Value root;


    response = SendCommand("Player.GetActivePlayers");

    if(!reader.parse(response, root))
        throw logic_error("Failed to parse VideoLibrary.GetMovies response : "+reader.getFormattedErrorMessages());

    return root["result"][0]["playerid"].asInt();
}

string KodiApi::SendToActivePlayer(string method, string params)
{
    return SendCommand(method, params, true);
}

string KodiApi::SendCommand(string method, string params, bool toActivePlayer)
{
    HttpRequest httpQry;
    HttpResponse httpResp;
    string response;
    ostringstream oss;


    if((toActivePlayer==true)&&(m_ActivePlayer==-1))
        m_ActivePlayer = GetActivePlayer();

    oss << "http://" << m_Server;
    oss << "/jsonrpc?request={\"jsonrpc\":\"2.0\",\"method\":\"" << method << "\",";

    if((params != "")||(toActivePlayer == true))
    {
        oss << "\"params\":{";
        if(params != "") oss << params;
        if((params != "")&&(toActivePlayer == true)) oss << ",";
        if(toActivePlayer == true) oss << "\"playerid\":" << m_ActivePlayer;
        oss << "},";
    }
    oss << "\"id\":1}";

    httpQry.SetHttpMethod(HttpRequest::HTTP_GET);
    httpQry.SetQueryString(oss.str());
    //httpQry.SetTimeout(20000000);
    response = httpQry.Execute();

    httpResp.Parse(response);
    return httpResp.GetBody();
}
