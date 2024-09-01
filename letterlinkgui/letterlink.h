#ifndef LETTERLINK_H
#define LETTERLINK_H

#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>

class LetterLink {
private:
    const int MAX_CHAIN_SIZE = 6;
    const int MAX_MIRROR_SIZE = 1;
    std::vector<std::string> wordlist;
    std::vector<std::string> wordset;

    void showLinks(std::vector<std::string> links);
    int RandomInt(int size);
    bool wordCheck(std::string& word, std::string& check, int& change);
    bool wordCheck(std::string& word, std::string& check);
    bool repeatWord(std::string word, std::vector<std::string>& chain);
    bool mirrorCheck(std::vector<std::string>& wordset);
    std::string findChain(std::vector<std::string>& wordlist, std::string& startword, const int LISTLEN, int& chainlen);
    void getChain(std::vector<std::string>& wordlist, std::vector<std::string>& wordset);

public:
    LetterLink();
    void insertWordAtPosition(const std::string& newWord, int position);
    void removeWordAtPosition(int position);

    std::string& getWord(int index);
    void setCharAt(int wordIndex, int charIndex, char newChar);
    const std::vector<std::string>& getWordset() const { return wordset; }

};

#endif // LETTERLINK_H
