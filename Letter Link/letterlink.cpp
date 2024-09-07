#include "letterlink.h"
 
void LetterLink::setDifficulty(int difficulty) {
    cout << "Setting difficulty to " << difficulty << ": ";
    switch (difficulty) {
    case 0: // easy
        MIN_WRITEABLE = 2;
        MAX_CHAIN_SIZE = 4;
        MAX_MIRROR_SIZE = 2;
        NOT_SAME_CHANGE = 1;
        cout << "Easy mode\n";
        break;
    case 1: // normal
        MIN_WRITEABLE = 4;
        MAX_CHAIN_SIZE = 6;
        MAX_MIRROR_SIZE = 2;
        NOT_SAME_CHANGE = 2;
        cout << "Normal mode\n";
        break;
    case 2: // hard
        MIN_WRITEABLE = 6;
        MAX_CHAIN_SIZE = 8;
        MAX_MIRROR_SIZE = 1;
        NOT_SAME_CHANGE = 4;
        cout << "Hard mode\n";
        break;
    case 3: // impossible
        MIN_WRITEABLE = 8;
        MAX_CHAIN_SIZE = 10;
        MAX_MIRROR_SIZE = 0;
        NOT_SAME_CHANGE = 8;
        cout << "Impossible mode\n";
        break;
    default:
        cout << "Invalid difficulty\n";
        break;
    }
}


bool LetterLink::evalChain(vector<string>& guess, vector<int>& evalBefore, vector<int>& evalAfter) {
    bool valid = true, emptyBefore = false, emptyAfter = false;

    int letterChangeBefore = 0, letterChangeAfter = 0,
        guessSize = guess.size() - 1, guessSizeFull = guess.size();

    // Check if the guess vector is empty or has fewer than two words (invalid input)
    if (guess.size() < 2) {
        cout << "Invalid guess size: less than 2 words\n";
        valid = false;
    }

    for (int i = 1; i < guessSize; i++) {
        if (guess[i] == "" || guess[i][0] == ' ') {
            cout << "Empty or invalid word at position " << i << ": \"" << guess[i] << "\"\n";
            evalBefore[i - 1] = 0;
            evalAfter[i - 1] = 0;
            valid = false;
            continue;
        }
        cout << "Evaluating word: \"" << guess[i] << "\"\n";

        // Check that all middle words (excluding the first and last) are in the wordlist
        if (guess[i].length() < 5 || find(wordlist.begin(), wordlist.end(), guess[i]) == wordlist.end()) {
            cout << "Word \"" << guess[i] << "\" not found in wordlist\n";
            evalBefore[i - 1] = 2;
            evalAfter[i - 1] = 2;
            valid = false;
            continue;
        }
    }
    // Check that each word in the guess differs by exactly one letter from the previous word
    for (int i = 1; i < guessSize; i++) {
        letterChangeBefore = 0;
        letterChangeAfter = 0;
        emptyBefore = false;
        emptyAfter = false;

        if (evalBefore[i - 1] != 1) {
            continue;
        }

        if (i > 1) {
            if (evalBefore[i - 2] != 1) {
                evalBefore[i - 1] = 3;
                goto AFTER;
            }
        }

        for (int j = 0; j < guess[i].length(); j++) {
            if (guess[i][j] != guess[i - 1][j] && i > 0 && guess[i - 1].length() == 5) {
                letterChangeBefore++;
            }

        }

        if (letterChangeBefore != 1) {
            cout << "Word \"" << guess[i] << "\" changes " << letterChangeBefore << " letters from previous word \"" << guess[i - 1] << "\"\n";
            valid = false;
            evalBefore[i - 1] = 3;
        }

        AFTER:

        if (i < guessSize) {
            if (evalAfter[i] != 1) {
                evalAfter[i - 1] = 3;
                continue;
            }
        }

        for (int j = 0; j < guess[i].length(); j++) {
            if (guess[i][j] != guess[i + 1][j] && i < guessSize && guess[i + 1].length() == 5) {
                letterChangeAfter++;
            }
        }

        if (letterChangeAfter != 1) {
            cout << "Word \"" << guess[i] << "\" changes " << letterChangeBefore << " letters from previous word \"" << guess[i - 1] << "\"\n";
            valid = false;
            evalAfter[i - 1] = 3;
        }
    }

    cout << "Chain evaluation result: " << (valid ? "Valid" : "Invalid") << "\n";
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
            } while (change == lastchange && timeout < NOT_SAME_CHANGE);

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

