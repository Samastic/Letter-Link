#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

// Function to generate all possible words by replacing each letter with a wildcard '_'
vector<string> generatePatterns(const string& word) {
    vector<string> patterns;
    for (size_t i = 0; i < word.length(); ++i) {
        string pattern = word;
        pattern[i] = '_'; // Replace the ith character with '_'
        patterns.push_back(pattern);
    }
    return patterns;
}

// Function to find all chain connections of length greater than or equal to 3
void findChainConnections(const vector<string>& words, int wordLength) {
    // Store patterns and corresponding words
    vector<vector<string>> chains;

    for (size_t i = 0; i < words.size(); ++i) {
        vector<string> chain;
        chain.push_back(words[i]);

        for (size_t j = 0; j < words.size(); ++j) {
            if (i != j && words[j].length() == wordLength) {
                // Check if the words differ by exactly one letter
                int diff = 0;
                for (size_t k = 0; k < wordLength; ++k) {
                    if (words[i][k] != words[j][k]) {
                        diff++;
                    }
                }
                if (diff == 1) {
                    chain.push_back(words[j]);
                }
            }
        }

        // If a chain of length 3 or more is found, add it to chains
        if (chain.size() >= 3) {
            chains.push_back(chain);
            cout << "Chain found of length " << chain.size() << ": ";
            for (const string& word : chain) {
                cout << word << " ";
            }
            cout << endl;
        }
    }

    cout << "Total chains found for " << wordLength << "-letter words: " << chains.size() << endl;
}

int main() {
    ifstream inputFile("connections.txt");
    if (!inputFile) {
        cerr << "Error opening file connections.txt" << endl;
        return 1;
    }

    vector<string> words;
    string word;
    while (inputFile >> word) {
        words.push_back(word);
    }

    inputFile.close();

    // Separate words by length
    vector<string> fourLetterWords;
    vector<string> fiveLetterWords;
    vector<string> sixLetterWords;

    for (const string& word : words) {
        if (word.length() == 4) {
            fourLetterWords.push_back(word);
        }
        else if (word.length() == 5) {
            fiveLetterWords.push_back(word);
        }
        else if (word.length() == 6) {
            sixLetterWords.push_back(word);
        }
    }

    // Find chain connections for each length
    cout << setw(40) << setfill('=') << "" << endl;
    cout << "Chains for 4-letter words:" << endl;
    cout << setw(40) << setfill('=') << "" << endl;
    findChainConnections(fourLetterWords, 4);

    cout << setw(40) << setfill('=') << "" << endl;
    cout << "Chains for 5-letter words:" << endl;
    cout << setw(40) << setfill('=') << "" << endl;
    findChainConnections(fiveLetterWords, 5);

    cout << setw(40) << setfill('=') << "" << endl;
    cout << "Chains for 6-letter words:" << endl;
    cout << setw(40) << setfill('=') << "" << endl;
    findChainConnections(sixLetterWords, 6);

    return 0;
}
