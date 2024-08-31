#ifndef LETTERLINK_H
#define LETTERLINK_H

#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>


using namespace std;

class LetterLink {
private:
    const int MAX_CHAIN_SIZE = 8;
    const int MAX_MIRROR_SIZE = 2;
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
};

#endif // LETTERLINK_H
