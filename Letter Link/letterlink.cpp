#include "letterlink.h"

void LetterLink::setDifficulty(int difficulty) {
    switch (difficulty) {
    case 0: // easy
        MIN_WRITEABLE = 2;
        MAX_CHAIN_SIZE = 4;
        MAX_MIRROR_SIZE = 3;
        break;
    case 1: // normal
        MIN_WRITEABLE = 4;
        MAX_CHAIN_SIZE = 6;
        MAX_MIRROR_SIZE = 2;
        break;
    case 2: // hard
        MIN_WRITEABLE = 6;
        MAX_CHAIN_SIZE = 8;
        MAX_MIRROR_SIZE = 1;
        break;
    case 3: // impossible
        MIN_WRITEABLE = 8;
        MAX_CHAIN_SIZE = 10;
        MAX_MIRROR_SIZE = 0;
        break;
    default:
        // Handle invalid difficulty values if needed
        break;
    }
}

bool LetterLink::evalChain(vector<string>& guess, vector<int>& guessResponse) {
    bool valid = true;

    // Check if the guess vector is empty or has fewer than two words (invalid input)
    if (guess.size() < 2) {
        valid = false;
    }

    // Check that all middle words (excluding the first and last) are in the wordlist
    for (size_t i = 1; i < guess.size() - 1; ++i) {
        if (find(wordlist.begin(), wordlist.end(), guess[i]) == wordlist.end()) {
            guessResponse[i - 1] = 1;
            valid = false;
        }
    }

    // Check that each word in the guess differs by exactly one letter from the previous word
    for (size_t i = 1; i < guess.size() - 1; ++i) {
        int letterChangeCount = 0;
        for (size_t j = 0; j < guess[i].length(); ++j) {
            if (guess[i][j] != guess[i - 1][j]) {
                letterChangeCount++;
            }
        }

        if (letterChangeCount != 1) {
            valid = false;

            if (guessResponse[i - 1] == 0) {
                guessResponse[i - 1] = 2;
            }
        }
    }
    return valid;
}


void LetterLink::showLinks(vector<string> links) {
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

int LetterLink::RandomInt(int size) {
    static bool set_time = false;

    if (!set_time) {
        srand(time(0));
        set_time = true;
    }

    int random_number = rand() % size;

    return random_number;
}

bool LetterLink::wordCheck(string& word, string& check, int& change) {
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
        return true;
    }
    else {
        return false;
    }
}

bool LetterLink::wordCheck(string& word, string& check) {
    static int dumby;
    dumby = 0;
    return wordCheck(word, check, dumby);
}

bool LetterLink::repeatWord(string word, vector<string>& chain) {
    for (int i = 0; i < (chain.size() - 1); i++) {
        if (word == chain[i]) {
            return true;
        }
    }

    return false;
}

bool LetterLink::mirrorCheck(vector<string>& wordset) {
    int mirror = 0;
    for (int i = 0; i < 5; i++) {
        if (wordset[0][i] == wordset[1][i]) {
            mirror++;
        }
    }

    return mirror > MAX_MIRROR_SIZE;
}

string LetterLink::findChain(vector<string>& wordlist, string& startword, const int LISTLEN, int& chainlen) {
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
            if (!wordCheck(checkword, wordlist[j])) {
                continue;
            }
            if (!repeatWord(wordlist[j], chain)) {
                links.push_back(wordlist[j]);
            }
        }

        cout << "\t\t\t\tLink length: " << links.size() << endl
            << "\t\t\t\tWords in links: ";
        showLinks(links);

        if (links.size() > 0 && chain.size() < MAX_CHAIN_SIZE) {
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
            cout << "\tCHAIN COMPLETED\n";
            found_endword = true;
            break;
        }
    }

    autochain = chain;
    chainlen = chain.size();
    return chain.back();
}

void LetterLink::getChain(vector<string>& wordlist, vector<string>& wordset) {
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

LetterLink::LetterLink() {
    wordlist = secret::bible;

    cout << "Wordlist generated.\n";

    getChain(wordlist, wordset);

    cout << "Wordset (" << wordset[0] << ", " << wordset[1] << ")\n";
}
