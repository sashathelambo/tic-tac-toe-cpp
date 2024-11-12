#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <conio.h>
#include <cstdlib>
#include <ctime>
#include <memory>
#include "validator.hpp"
#include <chrono>
#include <thread>

using namespace std;

bool exitFlag = false;
bool returnToMenuFlag = false;

shared_ptr<vector<string>> board = make_shared<vector<string>>(9);

unique_ptr<int> playerInput = make_unique<int>();
unique_ptr<int> computerInput = make_unique<int>();

shared_ptr<string> currentPlayer = make_shared<string>();

unique_ptr<bool> isComputerOpponent = make_unique<bool>();

struct Score {
    int player1Score = 0;
    int player2Score = 0;
    int draws = 0;
};

shared_ptr<Score> gameScore = make_shared<Score>();

enum class Difficulty {
    EASY,
    MEDIUM, 
    HARD
};

unique_ptr<Difficulty> computerDifficulty = make_unique<Difficulty>(Difficulty::EASY);

struct GameStats {
    int totalGames = 0;
    int fastestWin = INT_MAX;
    int longestGame = 0;
    string lastGameResult;
};

shared_ptr<GameStats> gameStats = make_shared<GameStats>();

class GameTimer {
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point turnStartTime;
    bool isRunning = false;
    const int TURN_TIME_LIMIT = 10;

public:
    void start() {
        startTime = std::chrono::steady_clock::now();
        turnStartTime = startTime;
        isRunning = true;
    }

    void resetTurnTimer() {
        turnStartTime = std::chrono::steady_clock::now();
    }

    int getElapsedSeconds() {
        if (!isRunning) return 0;
        auto currentTime = std::chrono::steady_clock::now();
        return std::chrono::duration_cast<std::chrono::seconds>
            (currentTime - startTime).count();
    }

    int getRemainingTurnTime() {
        if (!isRunning) return TURN_TIME_LIMIT;
        auto currentTime = std::chrono::steady_clock::now();
        int elapsed = std::chrono::duration_cast<std::chrono::seconds>
            (currentTime - turnStartTime).count();
        return std::max(0, TURN_TIME_LIMIT - elapsed);
    }

    bool isTurnTimeUp() {
        return getRemainingTurnTime() <= 0;
    }

    void displayTime() {
        if (!isRunning) return;
        int gameSeconds = getElapsedSeconds();
        int gameMinutes = gameSeconds / 60;
        gameSeconds %= 60;
        
        int turnTimeLeft = getRemainingTurnTime();
        
        cout << "\033[36m";
        cout << "[Game: ";
        if (gameMinutes > 0) {
            cout << gameMinutes << "m ";
        }
        cout << gameSeconds << "s] ";
        
        if (turnTimeLeft <= 3) {
            cout << "\033[31m";
        }
        cout << "[Turn: " << turnTimeLeft << "s] ";
        cout << "\033[0m";
    }
};

unique_ptr<GameTimer> gameTimer = make_unique<GameTimer>();

enum class Archetype {
    NONE,
    SWARM,
    PYROMANCER
};

struct Player {
    string marker;
    Archetype archetype;
    bool hasUsedSpecial;
};

shared_ptr<Player> player1 = make_shared<Player>();
shared_ptr<Player> player2 = make_shared<Player>();

void initializePlayers();
bool checkSwarmWin(const string& marker);
void displayArchetypeMenu(const string& playerNum);
void handlePyromancerMove(const shared_ptr<Player>& currentPlayerPtr);

void displayBoard();
bool checkWin(const string& marker);
bool isDraw();
void resetBoard();
int getPlayerInput();
int getComputerInput();
void playGame();
void showMenu();
void displayScore();
int getEasyComputerMove();
int getMediumComputerMove();
int getHardComputerMove();
void updateGameStats(const string& result);

void displayBoard() {
    for (int i = 0; i < 9; i += 3) {
        for (int j = 0; j < 3; ++j) {
            if ((*board)[i + j] == "X") {
                cout << "\033[31m";
            } else if ((*board)[i + j] == "O") {
                cout << "\033[34m";
            } else {
                cout << "\033[0m";
            }
            cout << (*board)[i + j];
            cout << "\033[0m";
            if (j < 2) cout << " | ";
        }
        cout << endl;
        if (i < 6) cout << "---------" << endl;
    }
}

bool checkWin(const string& marker) {
    int winPatterns[8][3] = {
        {0, 1, 2}, {3, 4, 5}, {6, 7, 8},
        {0, 3, 6}, {1, 4, 7}, {2, 5, 8},
        {0, 4, 8}, {2, 4, 6}
    };
    for (const auto& pattern : winPatterns) {
        if ((*board)[pattern[0]] == marker && (*board)[pattern[1]] == marker && (*board)[pattern[2]] == marker) {
            return true;
        }
    }

    shared_ptr<Player> currentPlayerPtr = (*currentPlayer == "X") ? player1 : player2;
    if (currentPlayerPtr->archetype == Archetype::SWARM) {
        return checkSwarmWin(marker);
    }

    return false;
}

bool isDraw() {
    for (const auto& cell : *board) {
        if (cell != "X" && cell != "O") {
            return false;
        }
    }
    return true;
}

void resetBoard() {
    for (int i = 0; i < 9; ++i) {
        (*board)[i] = to_string(i + 1);
    }
}

int getPlayerInput() {
    auto startWait = std::chrono::steady_clock::now();
    
    while (true) {
        cout << "\033[2K\r";
        gameTimer->displayTime();
        cout << "Player " << (*currentPlayer == "X" ? "1 (X)" : "2 (O)") << ", choose a position (1-9): ";
        cout << "Available cells: ";
        for (int i = 0; i < 9; ++i) {
            if (!Validator::isCellTaken(*board, i + 1)) {
                cout << (i + 1) << " ";
            }
        }
        cout << flush;

        if (gameTimer->isTurnTimeUp()) {
            cout << "\n\033[31mTime's up! Turn skipped.\033[0m" << endl;
            return -2;
        }

        if (_kbhit()) {
            char ch = _getch();
            if (ch == 27) {
                exitFlag = true;
                cout << "\033[31m";
                cout << "ESC clicked. Exiting the game..." << endl;
                cout << "\033[0m";
                return -1;
            }
            if (ch == 'm' || ch == 'M') {
                returnToMenuFlag = true;
                cout << "\033[36m";
                cout << "Menu clicked. Returning to menu..." << endl;
                cout << "\033[0m";
                return -1;
            }
            string input(1, ch);
            if (Validator::isValidInput(input)) {
                int position = stoi(input);
                if (!Validator::isCellTaken(*board, position)) {
                    cout << ch << endl;
                    return position;
                } else {
                    cout << "\033[31m";
                    cout << "\nCell already taken. Please choose another position: ";
                    cout << "\033[0m";
                }
            } else {
                cout << "\033[31m";
                cout << "\nInvalid input. Please enter a number between 1 and 9: ";
                cout << "\033[0m";
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

int getComputerInput() {
    switch(*computerDifficulty) {
        case Difficulty::EASY:
            return getEasyComputerMove();
        case Difficulty::MEDIUM:
            return getMediumComputerMove();
        case Difficulty::HARD:
            return getHardComputerMove();
        default:
            return getEasyComputerMove();
    }
}

void playGame() {
    resetBoard();
    gameTimer->start();
    
    cout << "\033[32m";
    cout << "Welcome to Tic-Tac-Toe! Player 1 is X, Player 2 is O. Let's begin!" << endl;
    cout << "\033[36m";
    cout << "Press ESC at any time to exit the game." << endl;
    cout << "Press 'M' at any time to return to the menu." << endl;
    cout << "\033[0m";
    displayBoard();

    *currentPlayer = "X";

    while (!exitFlag && !returnToMenuFlag) {
        bool validMove = false;
        gameTimer->resetTurnTimer();

        shared_ptr<Player> currentPlayerPtr = (*currentPlayer == "X") ? player1 : player2;

        while (!validMove && !exitFlag && !returnToMenuFlag) {
            if (currentPlayerPtr->archetype == Archetype::PYROMANCER) {
                handlePyromancerMove(currentPlayerPtr);
            }

            if (*currentPlayer == "X" || !*isComputerOpponent) {
                *playerInput = getPlayerInput();
            } else {
                *playerInput = getComputerInput();
            }

            if (*playerInput == -1) {
                break;
            } else if (*playerInput == -2) {
                validMove = true;
                continue;
            }

            if (!Validator::isCellTaken(*board, *playerInput)) {
                validMove = true;
                (*board)[*playerInput - 1] = *currentPlayer;
            }
        }

        if (exitFlag || returnToMenuFlag) break;

        if (*playerInput != -2) {
            displayBoard();

            if (checkWin(*currentPlayer)) {
                cout << "\033[32m";
                cout << "Player " << (*currentPlayer == "X" ? "1 (X)" : "2 (O)") << " wins!" << endl;
                if (*currentPlayer == "X") gameScore->player1Score++;
                else gameScore->player2Score++;
                updateGameStats(*currentPlayer == "X" ? "Player 1 Won" : "Player 2 Won");
                displayScore();
                cout << "\033[0m";
                break;
            }

            if (isDraw()) {
                cout << "\033[33m";
                cout << "It's a draw!" << endl;
                gameScore->draws++;
                updateGameStats("Draw");
                displayScore();
                cout << "\033[0m";
                break;
            }
        }

        *currentPlayer = (*currentPlayer == "X") ? "O" : "X";
    }

    if (!exitFlag && !returnToMenuFlag) {
        cout << "\033[36m";
        cout << "Press any key to continue..." << endl;
        cout << "\033[0m";
        _getch();
    }
}

void showMenu() {
    cout << "\033[36m";
    cout << "Choose game mode:" << endl;
    cout << "1. Classic (vs Computer)" << endl;
    cout << "2. Classic (vs Player)" << endl;
    cout << "3. Battle Mode (vs Player)" << endl;
    cout << "Enter your choice (1-3, ESC to exit or M for menu): ";
    cout << "\033[0m";

    while (true) {
        if (_kbhit()) {
            char ch = _getch();
            if (ch == 27) {  // ESC key
                exitFlag = true;
                cout << "\033[31m";
                cout << "ESC clicked. Exiting the game..." << endl;
                cout << "\033[0m";
                return;
            }
            
            if (ch == '1') {
                *isComputerOpponent = true;
                player1->archetype = Archetype::NONE;
                player2->archetype = Archetype::NONE;
                
                cout << "\033[36m";
                cout << "\nSelect difficulty:" << endl;
                cout << "1. Easy" << endl;
                cout << "2. Medium" << endl;
                cout << "3. Hard" << endl;
                cout << "Enter your choice (1-3): ";
                cout << "\033[0m";
                
                while (true) {
                    if (_kbhit()) {
                        char diff = _getch();
                        if (diff == '1') {
                            *computerDifficulty = Difficulty::EASY;
                            cout << "1\nSelected Easy difficulty" << endl;
                            break;
                        } else if (diff == '2') {
                            *computerDifficulty = Difficulty::MEDIUM;
                            cout << "2\nSelected Medium difficulty" << endl;
                            break;
                        } else if (diff == '3') {
                            *computerDifficulty = Difficulty::HARD;
                            cout << "3\nSelected Hard difficulty" << endl;
                            break;
                        }
                    }
                }
                playGame();
                break;
            } else if (ch == '2') {
                cout << "2" << endl;
                *isComputerOpponent = false;
                player1->archetype = Archetype::NONE;
                player2->archetype = Archetype::NONE;
                playGame();
                break;
            } else if (ch == '3') {
                cout << "3" << endl;
                *isComputerOpponent = false;
                initializePlayers();
                if (!exitFlag && !returnToMenuFlag) {
                    playGame();
                }
                break;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

int main(int argc, char* argv[]) {
    if (argc > 1) {
        string boardState = argv[1];
        int move = getComputerInput();
        cout << move;
        return 0;
    }
    
    while (!exitFlag) {
        returnToMenuFlag = false;
        showMenu();
    }
    return 0;
}

void displayScore() {
    cout << "\033[36m";
    cout << "\nScore Board:" << endl;
    cout << "Player 1 (X): " << gameScore->player1Score << endl;
    cout << "Player 2 (O): " << gameScore->player2Score << endl;
    cout << "Draws: " << gameScore->draws << endl;
    cout << "\nGame Statistics:" << endl;
    cout << "Total Games: " << gameStats->totalGames << endl;
    if (gameStats->fastestWin != INT_MAX) {
        cout << "Fastest Win: " << gameStats->fastestWin << "s" << endl;
    }
    cout << "Longest Game: " << gameStats->longestGame << "s" << endl;
    cout << "Last Game: " << gameStats->lastGameResult << endl;
    cout << "\033[0m";
}

int getEasyComputerMove() {
    srand(time(0));
    int pos;
    do {
        pos = rand() % 9 + 1;
    } while (Validator::isCellTaken(*board, pos));
    return pos;
}

int getMediumComputerMove() {
    for (int i = 1; i <= 9; i++) {
        if (!Validator::isCellTaken(*board, i)) {
            (*board)[i-1] = "O";
            if (checkWin("O")) {
                (*board)[i-1] = to_string(i);
                return i;
            }
            (*board)[i-1] = to_string(i);
        }
    }
    
    for (int i = 1; i <= 9; i++) {
        if (!Validator::isCellTaken(*board, i)) {
            (*board)[i-1] = "X";
            if (checkWin("X")) {
                (*board)[i-1] = to_string(i);
                return i;
            }
            (*board)[i-1] = to_string(i);
        }
    }
    
    return getEasyComputerMove();
}

int getHardComputerMove() {
    if (!Validator::isCellTaken(*board, 5)) {
        return 5;
    }
    
    int corners[] = {1, 3, 7, 9};
    for (int corner : corners) {
        if (!Validator::isCellTaken(*board, corner)) {
            return corner;
        }
    }
    
    return getMediumComputerMove();
}

void updateGameStats(const string& result) {
    gameStats->totalGames++;
    gameStats->lastGameResult = result;
    
    int gameTime = gameTimer->getElapsedSeconds();
    if (result != "Draw" && gameTime < gameStats->fastestWin) {
        gameStats->fastestWin = gameTime;
    }
    if (gameTime > gameStats->longestGame) {
        gameStats->longestGame = gameTime;
    }
}

struct Move {
    int position;
    string player;
    int timeSpent;
};

shared_ptr<vector<Move>> moveHistory = make_shared<vector<Move>>();

void recordMove(int position, const string& player, int timeSpent) {
    moveHistory->push_back({position, player, timeSpent});
}

void replayGame() {
    cout << "\033[32m";
    cout << "Replaying last game..." << endl;
    cout << "\033[0m";
    
    resetBoard();
    displayBoard();
    
    for (const auto& move : *moveHistory) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        (*board)[move.position - 1] = move.player;
        cout << "\033[36m";
        cout << "Player " << move.player << " moved to position " << move.position;
        cout << " (took " << move.timeSpent << "s)" << endl;
        cout << "\033[0m";
        displayBoard();
    }
}

void initializePlayers() {
    player1->marker = "X";
    player2->marker = "O";
    player1->hasUsedSpecial = false;
    player2->hasUsedSpecial = false;
    
    displayArchetypeMenu("1");
    displayArchetypeMenu("2");
}

bool checkSwarmWin(const string& marker) {
    return ((*board)[0] == marker && (*board)[2] == marker && 
            (*board)[6] == marker && (*board)[8] == marker);
}

void displayArchetypeMenu(const string& playerNum) {
    while (true) {
        cout << "\033[2K\r";  // Clear the current line
        cout << "\033[36m";
        cout << "\nPlayer " << playerNum << ", choose your archetype:" << endl;
        cout << "1. Swarm - A hive mind that can win by controlling all corners" << endl;
        cout << "2. Pyromancer - Can destroy all marks once per game" << endl;
        cout << "Enter your choice (1-2, ESC to exit or M for menu): ";
        cout << "\033[0m";

        if (_kbhit()) {
            char choice = _getch();
            if (choice == 27) {  // ESC key
                exitFlag = true;
                cout << "\033[31m";
                cout << "ESC clicked. Exiting the game..." << endl;
                cout << "\033[0m";
                return;
            } else if (choice == 'm' || choice == 'M') {
                returnToMenuFlag = true;
                cout << "\033[36m";
                cout << "Menu clicked. Returning to menu..." << endl;
                cout << "\033[0m";
                return;
            } else if (choice == '1' || choice == '2') {
                cout << choice << endl;  // Echo the choice
                
                if (choice == '1') {
                    if (playerNum == "1") {
                        player1->archetype = Archetype::SWARM;
                    } else {
                        player2->archetype = Archetype::SWARM;
                    }
                    cout << "\033[32mSelected Swarm\033[0m" << endl;
                } else {
                    if (playerNum == "1") {
                        player1->archetype = Archetype::PYROMANCER;
                    } else {
                        player2->archetype = Archetype::PYROMANCER;
                    }
                    cout << "\033[32mSelected Pyromancer\033[0m" << endl;
                }
                
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                return;
            } else {
                cout << "\033[31m";
                cout << "\nInvalid choice. Please enter 1 or 2: ";
                cout << "\033[0m";
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

void handlePyromancerMove(const shared_ptr<Player>& currentPlayerPtr) {
    if (currentPlayerPtr->hasUsedSpecial) {
        cout << "\033[31mYou've already used your special move!\033[0m" << endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        return;
    }

    while (true) {
        cout << "\033[2K\r";  // Clear the current line
        cout << "\033[36m";
        cout << "Choose your move type:" << endl;
        cout << "1. Regular move" << endl;
        cout << "2. Special move (Clear the board) - Can only be used once!" << endl;
        cout << "Enter your choice (1-2, ESC to exit or M for menu): ";
        cout << "\033[0m";

        if (_kbhit()) {
            char choice = _getch();
            if (choice == 27) {  // ESC key
                exitFlag = true;
                cout << "\033[31m";
                cout << "ESC clicked. Exiting the game..." << endl;
                cout << "\033[0m";
                return;
            } else if (choice == 'm' || choice == 'M') {
                returnToMenuFlag = true;
                cout << "\033[36m";
                cout << "Menu clicked. Returning to menu..." << endl;
                cout << "\033[0m";
                return;
            } else if (choice == '1' || choice == '2') {
                cout << choice << endl;

                if (choice == '1') {
                    return;
                } else {
                    resetBoard();
                    currentPlayerPtr->hasUsedSpecial = true;
                    cout << "\033[32mBoard cleared!\033[0m" << endl;
                    displayBoard();
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                    return;
                }
            } else {
                cout << "\033[31m";
                cout << "\nInvalid choice. Please enter 1 or 2: ";
                cout << "\033[0m";
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}
