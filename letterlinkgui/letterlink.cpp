#include "letterlink.h"

// Get a reference to a word at a specific index
std::string& LetterLink::getWord(int index) {
    return wordset[index];
}

// Set a character in a word at a specific position
void LetterLink::setCharAt(int wordIndex, int charIndex, char newChar) {
    if (wordIndex >= 0 && wordIndex < wordset.size() &&
        charIndex >= 0 && charIndex < wordset[wordIndex].length()) {
        wordset[wordIndex][charIndex] = newChar;
    }
}

void LetterLink::insertWordAtPosition(const std::string& newWord, int position) {
    if (position >= 0 && position <= wordset.size()) {  // Ensure valid position
        wordset.insert(wordset.begin() + position, newWord);
        std::cout << "New word inserted at position " << position << ": " << newWord << std::endl;
    }
    else {
        std::cout << "Invalid position for insertion!" << std::endl;
    }
}

void LetterLink::removeWordAtPosition(int position) {
    if (position >= 0 && position < wordset.size()) {  // Ensure valid position
        wordset.erase(wordset.begin() + position);
        std::cout << "Word removed at position " << position << std::endl;
    }
    else {
        std::cout << "Invalid position for removal!" << std::endl;
    }
}

void LetterLink::showLinks(std::vector<std::string> links) {
    for (int i = 0; i < links.size(); i++) {
        std::cout << links[i];
        if (i < links.size() - 1) {
            std::cout << ", ";
        }
        if (i % 5 == 4) {
            std::cout << std::endl << "\t\t\t\t\t\t";
        }
    }
    std::cout << std::endl;
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

bool LetterLink::wordCheck(std::string& word, std::string& check, int& change) {
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

bool LetterLink::wordCheck(std::string& word, std::string& check) {
    static int dumby;
    dumby = 0;
    return wordCheck(word, check, dumby);
}

bool LetterLink::repeatWord(std::string word, std::vector<std::string>& chain) {
    for (int i = 0; i < (chain.size() - 1); i++) {
        if (word == chain[i]) {
            return true;
        }
    }

    return false;
}

bool LetterLink::mirrorCheck(std::vector<std::string>& wordset) {
    int mirror = 0;
    for (int i = 0; i < 5; i++) {
        if (wordset[0][i] == wordset[1][i]) {
            mirror++;
        }
    }

    return mirror > MAX_MIRROR_SIZE;
}

std::string LetterLink::findChain(std::vector<std::string>& wordlist, std::string& startword, const int LISTLEN, int& chainlen) {
    bool found_endword = false;
    std::string checkword, nextword;
    std::vector<std::string> chain, links;
    int lastchange = -1, change = 0, timeout = 0;

    chain.clear();
    chain.push_back(startword);

    std::cout << "\tChain created, starting with " << chain.front() << std::endl;

    while (!found_endword) {
        checkword = chain.back();
        std::cout << "\t\tChecking " << checkword << ", Chain length = " << chain.size() << std::endl;
        links.clear();

        for (int j = 0; j < LISTLEN; j++) {
            if (!wordCheck(checkword, wordlist[j])) {
                continue;
            }
            if (!repeatWord(wordlist[j], chain)) {
                links.push_back(wordlist[j]);
            }
        }

        std::cout << "\t\t\t\tLink length: " << links.size() << std::endl
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
            std::cout << "\t\t\tWord found: " << chain.back() << std::endl;
            std::cout << "\t\t\t\tChanged Letter -- " << lastchange << std::endl;
        }
        else {
            std::cout << "\tCHAIN COMPLETED\n";
            found_endword = true;
            break;
        }
    }

    chainlen = chain.size();
    return chain.back();
}

void LetterLink::getChain(std::vector<std::string>& wordlist, std::vector<std::string>& wordset) {
    const int LISTLEN = wordlist.size() - 1;
    int chainlen = 0, randomnum;
    bool goodchain = false;

    do {
        wordset.clear();
        std::cout << "Attempting to get chain\n";
        randomnum = RandomInt(LISTLEN);
        std::cout << "Random num = " << randomnum;
        wordset.push_back(wordlist[randomnum]);
        std::cout << "\nStarting Word: " << wordset.front() << std::endl;

        wordset.push_back(findChain(wordlist, wordset.front(), LISTLEN, chainlen));
        std::cout << "\nEnding Word: " << wordset.back() << std::endl;

        if (chainlen < 4 || mirrorCheck(wordset)) {
            std::cout << "Bad chain!\n";
        }
        else {
            std::cout << "Good chain!\n";
        }
    } while (chainlen < 4 || mirrorCheck(wordset));
}

LetterLink::LetterLink() {
    std::ifstream inputFile("connections.txt");
    if (!inputFile) {
        std::cerr << "Error opening file connections.txt" << std::endl;
        exit(1);
    }
    std::cout << "File opened.\n";

    std::string word;
    while (inputFile >> word) {
        wordlist.push_back(word);
    }

    inputFile.close();

    std::cout << "Wordlist generated.\n";

    getChain(wordlist, wordset);

    std::cout << "Wordset (" << wordset[0] << ", " << wordset[1] << ")\n";
}
