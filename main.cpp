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

// Flags for game state
bool exitFlag = false;
bool returnToMenuFlag = false;

// Game board
unique_ptr<vector<string>> board = make_unique<vector<string>>(9);

// Game state variables
string currentPlayer;
bool isComputerOpponent = false;
bool isBattleMode = false;

// Archetype system
enum class Archetype { None, Swarm, Pyromancer };
Archetype player1Archetype = Archetype::None;
Archetype player2Archetype = Archetype::None;
bool pyromancerUsedSpecial = false;

// Add these variables after the other game state variables
struct GameStats {
    int gamesPlayed = 0;
    int player1Wins = 0;
    int player2Wins = 0;
    int ties = 0;
} gameStats;

// Add after other declarations, before the first function
void displayGameReport();

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
    // Check normal win conditions
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

    // Check Swarm win condition (all corners)
    Archetype currentArchetype = (currentPlayer == "X") ? player1Archetype : player2Archetype;
    if (currentArchetype == Archetype::Swarm) {
        if ((*board)[0] == marker && (*board)[2] == marker && 
            (*board)[6] == marker && (*board)[8] == marker) {
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

// Function to handle Pyromancer's special move
void pyromancerSpecialMove() {
    resetBoard();
    pyromancerUsedSpecial = true;
    setColor(10);
    cout << "Pyromancer used their special move! All marks have been destroyed!" << endl;
    setColor(7);
}

// Function to get player input
int getPlayerInput() {
    Archetype currentArchetype = (currentPlayer == "X") ? player1Archetype : player2Archetype;
    
    if (currentArchetype == Archetype::Pyromancer && !pyromancerUsedSpecial) {
        setColor(11);
        cout << "\nPyromancer's turn! Choose your action:" << endl;
        cout << "1. Make a regular move" << endl;
        cout << "2. Use special move (destroy all marks)" << endl;
        setColor(7);
        
        while (true) {
            if (_kbhit()) {
                char choice = _getch();
                if (choice == '2') {
                    pyromancerSpecialMove();
                    return -2; // Special code for Pyromancer special move
                } else if (choice == '1') {
                    break; // Continue with regular move
                }
            }
        }
    }

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

// Function to select archetype
Archetype selectArchetype() {
    setColor(11);
    cout << "\nSelect your archetype:" << endl;
    cout << "1. Normal - Standard tic-tac-toe gameplay" << endl;
    cout << "2. Swarm - Can win by controlling all 4 corners" << endl;
    cout << "3. Pyromancer - Can clear all marks from the board once per game" << endl;
    setColor(7);

    while (true) {
        if (_kbhit()) {
            char ch = _getch();
            if (ch == '1') return Archetype::None;
            if (ch == '2') return Archetype::Swarm;
            if (ch == '3') return Archetype::Pyromancer;
        }
    }
}

// Function to play the game
void playGame() {
    resetBoard();
    pyromancerUsedSpecial = false;

    setColor(10);
    cout << "Welcome to " << (isBattleMode ? "Battle " : "") << "Tic-Tac-Toe!" << endl;
    
    // Select archetypes if in battle mode
    if (isBattleMode) {
        cout << "\nPlayer 1 (X), ";
        player1Archetype = selectArchetype();
        if (!isComputerOpponent) {
            cout << "\nPlayer 2 (O), ";
            player2Archetype = selectArchetype();
        }
    } else {
        player1Archetype = Archetype::None;
        player2Archetype = Archetype::None;
    }

    cout << "\nPlayer 1 is X, Player 2 is O. Let's begin!" << endl;
    setColor(11);
    cout << "Press ESC at any time to exit the game." << endl;
    cout << "Press 'M' at any time to return to the menu." << endl;
    setColor(7);
    displayBoard();

    currentPlayer = "X";

    while (!exitFlag && !returnToMenuFlag) {
        bool validMove = false;
        int playerInput;

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

            playerInput = (currentPlayer == "X" || !isComputerOpponent) ? 
                         getPlayerInput() : getComputerInput();

            if (playerInput == -1) {
                break;
            }
            if (playerInput == -2) { // Pyromancer special move
                validMove = true;
                continue;
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

        if (playerInput != -2) { // Only update board if not Pyromancer special move
            (*board)[playerInput - 1] = currentPlayer;
        }
        displayBoard();

        if (checkWin(currentPlayer)) {
            setColor(10);
            cout << "Player " << (currentPlayer == "X" ? "1 (X)" : "2 (O)") << " wins!" << endl;
            setColor(7);
            gameStats.gamesPlayed++;
            if (currentPlayer == "X") {
                gameStats.player1Wins++;
            } else {
                gameStats.player2Wins++;
            }
            break;
        }

        if (isDraw()) {
            setColor(14);
            cout << "It's a draw!" << endl;
            setColor(7);
            gameStats.gamesPlayed++;
            gameStats.ties++;
            break;
        }

        currentPlayer = (currentPlayer == "X") ? "O" : "X";
    }

    if (!exitFlag && !returnToMenuFlag) {
        setColor(11);
        cout << "Press any key to continue..." << endl;
        setColor(7);
        _getch();
    }
}

// Add this function declaration before showMenu()
void displayGameReport() {
    setColor(11);
    cout << "\n=== Game Session Report ===" << endl;
    cout << "Total games played: " << gameStats.gamesPlayed << endl;
    cout << "Player 1 (X) wins: " << gameStats.player1Wins << endl;
    cout << "Player 2 (O) wins: " << gameStats.player2Wins << endl;
    cout << "Ties: " << gameStats.ties << endl;
    cout << "=========================" << endl;
    setColor(7);
}

// Function to show the menu
void showMenu() {
    setColor(11);
    cout << "Select game mode:" << endl;
    cout << "1. Classic Tic-Tac-Toe" << endl;
    cout << "2. Battle Tic-Tac-Toe" << endl;
    cout << "Enter your choice (1 or 2, ESC to exit): ";
    setColor(7);

    while (true) {
        if (_kbhit()) {
            char ch = _getch();
            if (ch == 27) {
                exitFlag = true;
                setColor(12);
                cout << "ESC clicked. Exiting the game..." << endl;
                setColor(7);
                return;
            }
            if (ch == '1') {
                isBattleMode = false;
                break;
            } else if (ch == '2') {
                isBattleMode = true;
                break;
            }
        }
    }

    setColor(11);
    cout << "\nDo you want to play against the computer or another player?" << endl;
    cout << "1. Play against the computer" << endl;
    cout << "2. Play against another player" << endl;
    cout << "Enter your choice (1 or 2, ESC to exit or M to go back to menu): ";
    setColor(7);
    while (true) {
        if (_kbhit()) {
            char ch = _getch();
            if (ch == 27) {
                exitFlag = true;
                setColor(12);
                cout << "ESC clicked. Exiting the game..." << endl;
                setColor(7);
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
                setColor(11);
                cout << "Menu clicked. Returning to menu..." << endl;
                setColor(7);
                return;
            } else {
                setColor(12);
                cout << "Invalid choice. Please enter 1, 2, or M or ESC: ";
                setColor(7);
            }
        }
    }

    while (!exitFlag && !returnToMenuFlag) {
        if (!exitFlag && !returnToMenuFlag) {
            playGame();
        }

        if (!exitFlag && !returnToMenuFlag) {
            // Display current game statistics
            displayGameReport();
            
            setColor(11);
            cout << "Do you want to play again? (y for yes, n for no, and m for menu, ESC to exit): ";
            setColor(7);
            
            while (true) {
                if (_kbhit()) {
                    char playAgain = _getch();
                    if (playAgain == 'n' || playAgain == 'N') {
                        exitFlag = true;
                        break;
                    } else if (playAgain == 'm' || playAgain == 'M') {
                        returnToMenuFlag = true;
                        break;
                    } else if (playAgain == 27) {
                        setColor(12);
                        cout << "ESC clicked. Exiting the game..." << endl;
                        setColor(7);
                        exitFlag = true;
                        break;
                    } else if (playAgain == 'y' || playAgain == 'Y') {
                        break;
                    } else {
                        setColor(12);
                        cout << "Invalid choice. Please enter y, n, m, or ESC: ";
                        setColor(7);
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
    // Display final game report before exiting
    displayGameReport();
    return 0;
}