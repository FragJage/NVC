#include <iostream>
#include <fstream>
#include <sstream>
#include "StringTools/StringTools.h"
#include "SphinxGram.h"
#include "SphinxRecogniser.h"

using namespace std;

SphinxRecogniser::SphinxRecogniser(const string& languageFolder, const string& acousticFolder, const string& dictionaryFileName)
{
    m_LanguageFolder = languageFolder;
    m_AcousticFolder = languageFolder+"/"+acousticFolder;
    m_DictionaryFileName = languageFolder+"/"+dictionaryFileName;
    m_config = nullptr;
    m_ps = nullptr;
}

SphinxRecogniser::~SphinxRecogniser()
{
    ClearConfig();
}

void SphinxRecogniser::ClearConfig()
{
    if(m_ps!=nullptr)
        ps_free(m_ps);

    if(m_config!=nullptr)
        cmd_ln_free_r(m_config);
}

bool SphinxRecogniser::SetArea(const string& areaName, IVoiceControl* voiceControl)
{
    int nb=0;
    string phrase;
    string gramFilename = areaName+".gram";
    ostringstream oss;
    vector<string> neededList;
    SphinxGram sphinxGram;

    size_t posDeb;
    size_t posFin;
    bool isFound;


    const vector<VoiceCommand>* voiceCommands = voiceControl->GetVoiceCommands(areaName);
    if(voiceCommands==nullptr)
    {
        cout << "Area " << areaName << " not found" << endl;
        return false;
    }
    vector<VoiceCommand>::const_iterator it = voiceCommands->begin();
    vector<VoiceCommand>::const_iterator itEnd = voiceCommands->end();


    sphinxGram.AddDictionary(m_DictionaryFileName);
    sphinxGram.SetGrammarName(areaName);

    sphinxGram.AddRule("automateName", voiceControl->GetBotName());

    while(it != itEnd)
    {
        phrase = it->GetPhrase();
        if(!sphinxGram.AddRule("cmd"+StringTools::to_string(++nb), phrase))
            cout << sphinxGram.GetGramError() << endl;

        posFin = 0;
        do
        {
            posDeb = phrase.find("<", posFin);
            posFin = phrase.find(">", posDeb);
            isFound = ((posDeb!=string::npos)&&(posFin!=string::npos));
            if(isFound)
                neededList.push_back(phrase.substr(posDeb+1, posFin-posDeb-1));
        } while(isFound);
        ++it;
    }

    oss << "<automateName> (";
    for(int i = 1; i <= nb; i++)
    {
        oss << "<cmd" << i << ">";
        if(i!=nb) oss << " | ";
    }
    oss << ")";
    sphinxGram.AddPublicRule("command", oss.str());

    vector<string>::const_iterator itLst = neededList.begin();
    vector<string>::const_iterator itLstEnd = neededList.end();

    while(itLst != itLstEnd)
    {
        const vector<string> lst = voiceControl->GetList(*itLst);
        vector<string>::const_iterator itl = lst.begin();
        vector<string>::const_iterator itlEnd = lst.end();

        while(itl!=itlEnd)
        {
            if(!sphinxGram.CompleteRule(*itLst, *itl))
                cout << sphinxGram.GetGramError() << endl;
            ++itl;
        }
        ++itLst;
    }
    sphinxGram.SaveDictionary("Adapted.dic");
    sphinxGram.SaveGrammar(gramFilename);
    cout << "NotFoundWordCount : " << sphinxGram.GetNotFoundWordCount() << endl;

    ClearConfig();

    m_config = cmd_ln_init(nullptr, ps_args(), true,
		"-hmm", m_AcousticFolder.c_str(),
		"-jsgf", gramFilename.c_str(),
		"-dict", "Adapted.dic",
        //"-logfn", "nul",
		nullptr);

	if(m_config == nullptr)
	{
		cout << "Failed to create sphinx config object." << endl;
		return false;
	}

    m_ps = ps_init(m_config);
	if (m_ps == nullptr)
	{
		cout << "Failed to create voice recognizer." << endl;
		return false;
	}

    return true;
}

string SphinxRecogniser::ListenVoice()
{
	ad_rec_t *ad;
	uint8 utt_started, in_speech;
	int16 adbuf[2048];
	int32 k;
	char const *hyp;
    string strHyp="";


	if (m_ps == nullptr)
	{
		cout << "Voice recognizer not initialized." << endl;
		return "";
	}

    ad = ad_open();
	//ad = ad_open_dev(cmd_ln_str_r(config, "-adcdev"), (int)cmd_ln_float32_r(config, "-samprate"));
	if (ad == nullptr)
	{
		cout << "Failed to open audio device" << endl;
		return "";
	}

	if (ad_start_rec(ad) < 0)
	{
		cout << "Failed to start recording" << endl;
		return "";
	}

	if (ps_start_utt(m_ps) < 0)
	{
		cout << "Failed to start utterance" << endl;
		return "";
	}

	utt_started = false;
	cout << "Ready...." << endl;

	for (;;)
	{
		if ((k = ad_read(ad, adbuf, 2048)) < 0)
		{
			cout << "Failed to read audio" << endl;
			return "";
		}

		ps_process_raw(m_ps, adbuf, k, false, false);
		in_speech = ps_get_in_speech(m_ps);

		if ((!in_speech)&&(strHyp!=""))
            break;

		if (in_speech && !utt_started)
        {
			utt_started = true;
			cout << "Listening..." << endl;
		}

		if (!in_speech && utt_started)
        {
			ps_end_utt(m_ps);
			hyp = ps_get_hyp(m_ps, NULL);
			if (hyp != NULL)
            {
                strHyp += hyp;
			}

			if (ps_start_utt(m_ps) < 0)
			{
                cout << "Failed to restart utterance" << endl;
				return "";
			}
			utt_started = false;
			cout << "Ready" << endl;
		}
		Sleep(100);
	}

	ps_end_utt(m_ps);
	ad_close(ad);
    return strHyp;
}

int SphinxRecogniser::CheckWavHeader(char *header, int expected_sr)
{
	int sr;

	if (header[34] != 0x10) {
		printf("Input audio file has [%d] bits per sample instead of 16\n", header[34]);
		return 0;
	}
	if (header[20] != 0x1) {
		printf("Input audio file has compression [%d] and not required PCM\n", header[20]);
		return 0;
	}
	if (header[22] != 0x1) {
		printf("Input audio file has [%d] channels, expected single channel mono\n", header[22]);
		return 0;
	}
	sr = ((header[24] & 0xFF) | ((header[25] & 0xFF) << 8) | ((header[26] & 0xFF) << 16) | ((header[27] & 0xFF) << 24));
	if (sr != expected_sr) {
		printf("Input audio file has sample rate [%d], but decoder expects [%d]\n", sr, expected_sr);
		return 0;
	}
	return 1;
}

string SphinxRecogniser::ListenFile(const string& fileName)
{
	FILE *fh;
	int rv;
	int16 buf[512];
	int32 score;
	char const *hyp;
	const char* file = fileName.c_str();


    fh = fopen(file, "rb");
	if (fh == NULL)
	{
		cout << "Unable to open input file " << file << endl;
		return "";
	}

	if (strlen(file) > 4 && strcmp(file + strlen(file) - 4, ".wav") == 0)
	{
		char waveheader[44];
		fread(waveheader, 1, 44, fh);
		if (!CheckWavHeader(waveheader, (int)cmd_ln_float32_r(m_config, "-samprate")))
		{
			fclose(fh);
			printf("Failed to process file '%s' due to format mismatch.\n", file);
			return "";
		}
	}
	rv = ps_start_utt(m_ps);

	while (!feof(fh))
	{
		size_t nsamp;
		nsamp = fread(buf, 2, 512, fh);
		rv = ps_process_raw(m_ps, buf, nsamp, false, false);
	}

	rv = ps_end_utt(m_ps);
	hyp = ps_get_hyp(m_ps, &score);

	fclose(fh);

    return hyp;
}
