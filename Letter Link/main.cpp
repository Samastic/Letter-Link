#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>

using namespace std;

/*
_____|_C_|_M_|
Easy | 4 | 4 |
Med  | 8 | 2 |
Hard | 16| 0 |
*/

const int MAX_CHAIN_SIZE = 8;
const int MAX_MIRROR_SIZE = 2;

void showLinks(vector<string> links) {
    for (int i = 0; i < links.size(); i++) {
        cout << links[i];
        if (i < links.size() - 1) {
            cout << ", ";
        }
        if (i % 5 == 4) {
            cout << endl << "\t\t\t\t\t\t";
        }
    }
    cout << endl;
}

int RandomInt(int size) {
    static bool set_time = false;

    if (!set_time) {
        srand(time(0));
        set_time = true;
    }
    
    int random_number = rand() % size;

    //cout << "RANDOM NUMBER = " << random_number;

    return random_number;
}

bool wordCheck(string& word, string& check, int& change) {
    int match = 0;
    
    for (int i = 0; i < 5; i++) {
        if (word[i] == check[i]) {
            match++;
        }
        else {
            change = i;
        }
    }

    if (match == 4) { 
        //cout << "\t\t\tConnection found!\n";
        return true; 
    }
    else { return false; }

}

bool wordCheck(string& word, string& check) {
    static int dumby;
    dumby = 0;
    return wordCheck(word, check, dumby);
}

bool repeatWord(string word, vector<string>& chain) {
    for (int i = 0; i < (chain.size() - 1); i++) {
        if (word == chain[i]) {
            return true;
        }
    }

    //cout << "\t\t\tNot a repeat!\n";
    return false;
}

bool mirrorCheck(vector<string>& wordset) {
    int mirror = 0;
    for (int i = 0; i < 5; i++) {
        if (wordset[0][i] == wordset[1][i]) {
            mirror++;
        }
    }

    if (mirror > MAX_MIRROR_SIZE) {
        return true;
    }

    return false;
}

string findChain(vector<string>& wordlist, string& startword,  const int LISTLEN, int &chainlen) {
    bool found_endword = false;
    string checkword, nextword;
    vector<string> chain, links;
    int lastchange = -1, change = 0, timeout = 0;

    
    chain.clear();
    chain.push_back(startword);

    cout << "\tChain created, starting with " << chain.front() << endl;

    while (!found_endword) {
        checkword = chain.back();
        cout << "\t\tChecking " << checkword << ", Chain length = " << chain.size() << endl;
        links.clear();

        for (int j = 0; j < LISTLEN; j++) {
            //cout << "\t\t\tChecking " << checkword << " - "  << wordlist[j]  << ", " << j << " / " << LISTLEN << endl;
            if (!wordCheck(checkword, wordlist[j])) {
                continue;
            }
            if (!repeatWord(wordlist[j], chain)) {
                links.push_back(wordlist[j]);
            }
        }

        cout << "\t\t\t\tLink length: " << links.size() << endl;
        if (links.size() > 0 && chain.size() < MAX_CHAIN_SIZE) {
            cout << "\t\t\t\tWords in links: ";
            showLinks(links);
            
            timeout = 0;
            do { 
                nextword = links[RandomInt(links.size())];
                wordCheck(checkword, nextword, change);
                timeout++;
            } while (change == lastchange && timeout < 11);

            chain.push_back(nextword);
            lastchange = change;
            cout << "\t\t\tWord found: " << chain.back() << endl;
            cout << "\t\t\t\tChanged Letter -- " << lastchange << endl;
        }
        else {
            cout << "\tCHAIN COMPETED\n";
            found_endword = true;
            break;
        }
    }

    chainlen = chain.size();
    return chain.back();
}

void getChain(vector<string>& wordlist, vector<string>& wordset) {
    const int LISTLEN = wordlist.size() - 1;
    int chainlen = 0, randomnum;
    bool goodchain = false;
    
    do {
        wordset.clear();
        cout << "Attempting to get chain\n";
        randomnum = RandomInt(LISTLEN);
        cout << "Random num = " << randomnum;
        wordset.push_back(wordlist[randomnum]);
        cout << "\nStarting Word: " << wordset.front() << endl;

        wordset.push_back(findChain(wordlist, wordset.front(), LISTLEN, chainlen));
        cout << "\nEnding Word: " << wordset.back() << endl;

        if (chainlen < 4 || mirrorCheck(wordset)) {
            cout << "Bad chain!\n";
        }
        else {
            cout << "Good chain!\n";
        }
    } while (chainlen < 4 || mirrorCheck(wordset));
}

int main() {
    ifstream inputFile("connections.txt");
    if (!inputFile) {
        cerr << "Error opening file connections.txt" << endl;
        return 1;
    }
    cout << "File opened.\n";

    vector<string> wordlist, wordset;
    string word;
    while (inputFile >> word) {
        wordlist.push_back(word);
    }

    inputFile.close();

    cout << "Wordlist generated.\n";

    getChain(wordlist, wordset);

    cout << "Wordset (" << wordset[0] << ", " << wordset[1] << ")\n";


}
