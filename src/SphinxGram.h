#ifndef SPHINXGRAM_H
#define SPHINXGRAM_H

#include<string>
#include<map>
#include "SphinxDict.h"

class SphinxGram
{
    public:
        SphinxGram();
        virtual ~SphinxGram();

        void AddDictionary(const std::string& fileName);
        void SetGrammarName(const std::string& grammarName);
        bool AddRule(const std::string& ruleName, const std::string& content);
        bool CompleteRule(const std::string& ruleName, const std::string& content);
        bool AddPublicRule(const std::string& ruleName, const std::string& content);
        void SaveGrammar(const std::string& fileName);
        void SaveDictionary(const std::string& fileName);
        int GetNotFoundWordCount();
        std::string GetGramError();

    protected:

    private:
        std::string lowerFilter(const std::string& command);
        bool addWords(const std::string& command);
        SphinxDict m_SphinxDict;
        std::string m_GrammarName;
        std::map<std::string, std::string> m_Rules;
        std::string m_PublicRuleName;
        std::string m_PublicRuleContent;
        std::string m_LastError;
};

#endif // SPHINXGRAM_H
