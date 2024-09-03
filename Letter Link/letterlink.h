#ifndef LETTERLINK_H
#define LETTERLINK_H


#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <wx/wx.h>

using namespace std;

class LetterLink {
private:
    const int MAX_CHAIN_SIZE = 6;
    const int MAX_MIRROR_SIZE = 1;
    vector<string> wordlist;
    vector<string> wordset;
    

    void showLinks(vector<string> links);
    int RandomInt(int size);
    bool wordCheck(string& word, string& check, int& change);
    bool wordCheck(string& word, string& check);
    bool repeatWord(string word, vector<string>& chain);
    bool mirrorCheck(vector<string>& wordset);
    string findChain(vector<string>& wordlist, string& startword, const int LISTLEN, int& chainlen);
    void getChain(vector<string>& wordlist, vector<string>& wordset);

public:
    LetterLink();  // Constructor that initializes the program
    vector<string> getWordset() const { return wordset; }
    vector<string> guess;
    vector<string> autochain;

    bool evalChain(vector<string>& guess);

    void newChain(){ getChain(wordlist, wordset); }
};


#endif // LETTERLINK_H
