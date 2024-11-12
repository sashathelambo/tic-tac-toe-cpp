#ifndef VALIDATOR_HPP
#define VALIDATOR_HPP

#include <string>
#include <cctype>
#include <vector>
#include <iostream>

using namespace std;

class Validator {
public:
    static bool isValidInput(const string& input) {
        if (input.length() != 1 || !isdigit(input[0])) {
            return false;
        }
        int num = stoi(input);
        return num >= 1 && num <= 9;
    }

    static bool isCellTaken(const vector<string>& board, int pos) {
        if (pos < 1 || pos > 9) {
            return true;
        }
        
        string cell = board[pos - 1];
        return (cell == "X" || cell == "O" || cell == "🔥" || cell == "🐝");
    }
};

#endif
