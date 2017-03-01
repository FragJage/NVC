#ifndef SPHINXDICT_H
#define SPHINXDICT_H

#include<string>
#include<map>

#include <windows.h>

class SphinxDict
{
    public:
        SphinxDict();
        virtual ~SphinxDict();

        void LoadOriginal(std::string fileName);
        void SaveAdapted(std::string fileName);
        bool AddWord(std::string word);
        int GetNotFoundWordCount();

    protected:

    private:
        std::multimap<std::string, std::string> m_OriginalDict;
        std::multimap<std::string, std::string> m_AdaptedDict;
        int m_NotFoundWordCount;

};

#endif // SPHINXDICT_H
