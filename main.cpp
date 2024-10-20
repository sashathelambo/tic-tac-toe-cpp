#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <conio.h>
#include <cstdlib>
#include <ctime>
#include <memory>
#include "validator.hpp"
//https://gist.github.com/JBlond/2fea43a3049b38287e5e9cefc87b2124

using namespace std;

// Global flags for exiting and returning to menu
bool exitFlag = false;
bool returnToMenuFlag = false;

// Global board
unique_ptr<vector<string>> board = make_unique<vector<string>>(9);

// Global variables for player and computer input
int playerInput;
int computerInput;

// Global variable for current player
string currentPlayer;

// Global variable for computer opponent flag
bool isComputerOpponent;

// Function to set console text color without using windows.h
void setColor(int color) {
    switch (color) {
        case 12: cout << "\033[31m"; break; // Red for X
        case 9: cout << "\033[34m"; break; // Blue for O
        case 7: cout << "\033[0m"; break; // Default color for numbers
        case 10: cout << "\033[32m"; break; // Green for welcome message
        case 11: cout << "\033[36m"; break; // Cyan for menu message
        case 14: cout << "\033[33m"; break; // Yellow for player prompt and draw message
    }
}

// Function to display the Tic-Tac-Toe board
void displayBoard() {
    for (int i = 0; i < 9; i += 3) {
        for (int j = 0; j < 3; ++j) {
            setColor((*board)[i + j] == "X" ? 12 : (*board)[i + j] == "O" ? 9 : 7);
            cout << (*board)[i + j];
            setColor(7); // Reset to default color
            if (j < 2) cout << " | ";
        }
        cout << endl;
        if (i < 6) cout << "---------" << endl;
    }
}

// Function to check if a player has won
bool checkWin(const string& marker) {
    int winPatterns[8][3] = {
        {0, 1, 2}, {3, 4, 5}, {6, 7, 8}, // rows
        {0, 3, 6}, {1, 4, 7}, {2, 5, 8}, // columns
        {0, 4, 8}, {2, 4, 6}             // diagonals
    };
    for (const auto& pattern : winPatterns) {
        if ((*board)[pattern[0]] == marker && (*board)[pattern[1]] == marker && (*board)[pattern[2]] == marker) {
            return true;
        }
    }
    return false;
}

// Function to check if the game is a draw
bool isDraw() {
    for (const auto& cell : *board) {
        if (cell != "X" && cell != "O") {
            return false;
        }
    }
    return true;
}

// Function to reset the board
void resetBoard() {
    for (int i = 0; i < 9; ++i) {
        (*board)[i] = to_string(i + 1);
    }
}

// Function to get player input
int getPlayerInput() {
    while (true) {
        if (_kbhit()) {
            char ch = _getch();
            if (ch == 27) { // ESC key
                exitFlag = true;
                setColor(12);
                cout << "ESC clicked. Exiting the game..." << endl;
                setColor(7);
                return -1;
            }
            if (ch == 'm' || ch == 'M') { // Menu key
                returnToMenuFlag = true;
                setColor(11);
                cout << "Menu clicked. Returning to menu..." << endl;
                setColor(7);
                return -1;
            }
            string input(1, ch);
            if (Validator::isValidInput(input)) {
                int position = stoi(input);
                if (!Validator::isCellTaken(*board, position)) {
                    cout << ch << endl; // Echo the valid input
                    return position;
                } else {
                    setColor(12);
                    cout << "\nCell already taken. Please choose another position: ";
                    setColor(7);
                }
            } else {
                setColor(12);
                cout << "\nInvalid input. Please enter a number between 1 and 9: ";
                setColor(7);
            }
        }
    }
}

// Function to get computer input
int getComputerInput() {
    srand(time(0));
    int pos;
    do {
        pos = rand() % 9 + 1;
    } while (Validator::isCellTaken(*board, pos));
    cout << pos << endl; // Echo the computer's move
    return pos;
}

// Function to play the game
void playGame() {
    resetBoard();

    setColor(10); // Green color for welcome message
    cout << "Welcome to Tic-Tac-Toe! Player 1 is X, Player 2 is O. Let's begin!" << endl;
    setColor(11); // Light cyan color for exit message
    cout << "Press ESC at any time to exit the game." << endl;
    cout << "Press 'M' at any time to return to the menu." << endl;
    setColor(7); // Reset to default color
    displayBoard();

    currentPlayer = "X";

    while (!exitFlag && !returnToMenuFlag) {
        bool validMove = false;

        while (!validMove && !exitFlag && !returnToMenuFlag) {
            setColor(14);
            cout << "Player " << (currentPlayer == "X" ? "1 (X)" : "2 (O)") << ", choose a position (1-9): ";
            setColor(7);

            // Display available cells
            cout << "Available cells: ";
            for (int i = 0; i < 9; ++i) {
                if (!Validator::isCellTaken(*board, i + 1)) {
                    cout << (i + 1) << " ";
                }
            }
            cout << endl;

            if (currentPlayer == "X" || !isComputerOpponent) {
                playerInput = getPlayerInput();
            } else {
                playerInput = getComputerInput();
            }

            if (playerInput == -1) {
                break;
            }

            if (!Validator::isCellTaken(*board, playerInput)) {
                validMove = true;
            } else {
                setColor(12);
                cout << "Cell already taken. Please choose another position." << endl;
                setColor(7);
            }
        }

        if (exitFlag || returnToMenuFlag) break;

        (*board)[playerInput - 1] = currentPlayer;
        displayBoard();

        if (checkWin(currentPlayer)) {
            setColor(10); // Green color for win message
            cout << "Player " << (currentPlayer == "X" ? "1 (X)" : "2 (O)") << " wins!" << endl;
            setColor(7); // Reset to default color
            break;
        }

        if (isDraw()) {
            setColor(14); // Yellow color for draw message
            cout << "It's a draw!" << endl;
            setColor(7); // Reset to default color
            break;
        }

        currentPlayer = (currentPlayer == "X") ? "O" : "X";
    }

    if (!exitFlag && !returnToMenuFlag) {
        setColor(11); // Cyan color for exit message
        cout << "Press any key to continue..." << endl;
        setColor(7); // Reset to default color
        _getch(); // Wait for any key press
    }
}

// Function to show the menu
void showMenu() {
    char choice;

    setColor(11); // Cyan color for the prompt
    cout << "Do you want to play against the computer or another player?" << endl;
    cout << "1. Play against the computer" << endl;
    cout << "2. Play against another player" << endl;
    cout << "Enter your choice (1 or 2, ESC to exit or M to go back to menu): ";
    setColor(7); // Reset to default color
    while (true) {
        if (_kbhit()) {
            char ch = _getch();
            if (ch == 27) { // ESC key
                exitFlag = true;
                setColor(12); // Red color for exit message
                cout << "ESC clicked. Exiting the game..." << endl;
                setColor(7); // Reset to default color
                return;
            }
            if (ch == '1') {
                isComputerOpponent = true;
                break;
            } else if (ch == '2') {
                isComputerOpponent = false;
                break;
            } else if (ch == 'm' || ch == 'M') {
                returnToMenuFlag = true;
                setColor(11); // Cyan color for menu message
                cout << "Menu clicked. Returning to menu..." << endl;
                setColor(7); // Reset to default color
                return;
            } else {
                setColor(12); // Red color for error message
                cout << "Invalid choice. Please enter 1, 2, or M or ESC: ";
                setColor(7); // Reset to default color
            }
        }
    }

    while (!exitFlag && !returnToMenuFlag) {
        if (!exitFlag && !returnToMenuFlag) {
            playGame();
        }

        if (!exitFlag && !returnToMenuFlag) {
            setColor(11); // Cyan color for play again prompt
            cout << "Do you want to play again? (y for yes, n for no, and m for menu, ESC to exit): ";
            setColor(7); // Reset to default color
            char playAgain;
            while (true) {
                if (_kbhit()) {
                    playAgain = _getch();
                    if (playAgain == 'n' || playAgain == 'N') {
                        exitFlag = true;
                        break;
                    } else if (playAgain == 'm' || playAgain == 'M') {
                        returnToMenuFlag = true;
                        break;
                    } else if (playAgain == 27) { // ESC key
                        setColor(12); // Red color for exit message
                        cout << "ESC clicked. Exiting the game..." << endl;
                        setColor(7); // Reset to default color
                        exitFlag = true;
                        break;
                    } else if (playAgain == 'y' || playAgain == 'Y') {
                        break;
                    } else {
                        setColor(12); // Red color for error message
                        cout << "Invalid choice. Please enter y, n, m, or ESC: ";
                        setColor(7); // Reset to default color
                    }
                }
            }
        }
    }
}

int main() {
    while (!exitFlag) {
        returnToMenuFlag = false;
        showMenu();
    }
    return 0;
}
