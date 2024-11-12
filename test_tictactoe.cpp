#include <cassert>
#include <iostream>
#include <memory>
#include "validator.hpp"

// Test helper functions 
void print(const string& message) {
    cout << message << endl;
}

void assertThat(bool condition, const string& message) {
    if (!condition) {
        print("Test Failed: " + message);
        assert(false);
    }
}

// Test cases
void testValidatorInput() {
    print("\n=== Running Test: Validator Input ===");
    
    assertThat(Validator::isValidInput("1"), "1 should be valid input");
    assertThat(Validator::isValidInput("9"), "9 should be valid input");
    assertThat(!Validator::isValidInput("0"), "0 should be invalid input");
    assertThat(!Validator::isValidInput("10"), "10 should be invalid input");
    assertThat(!Validator::isValidInput("a"), "Letters should be invalid input");
    assertThat(!Validator::isValidInput(""), "Empty string should be invalid input");
}

void testCellTaken() {
    print("\n=== Running Test: Cell Taken ===");
    
    vector<string> board = {"1", "X", "O", "4", "5", "6", "7", "8", "9"};
    
    assertThat(!Validator::isCellTaken(board, 1), "Empty cell should not be taken");
    assertThat(Validator::isCellTaken(board, 2), "X cell should be taken");
    assertThat(Validator::isCellTaken(board, 3), "O cell should be taken");
    assertThat(Validator::isCellTaken(board, 0), "Position 0 should be invalid");
    assertThat(Validator::isCellTaken(board, 10), "Position 10 should be invalid");
}

void testBoardState() {
    print("\n=== Running Test: Board State ===");
    
    unique_ptr<vector<string>> board = make_unique<vector<string>>(9);
    for (int i = 0; i < 9; ++i) {
        (*board)[i] = to_string(i + 1);
    }
    
    // Test initial board state
    assertThat((*board)[0] == "1", "First cell should be 1");
    assertThat((*board)[8] == "9", "Last cell should be 9");
    
    // Test board updates
    (*board)[0] = "X";
    (*board)[4] = "O";
    assertThat((*board)[0] == "X", "Cell should update to X");
    assertThat((*board)[4] == "O", "Cell should update to O");
}

int main() {
    print("Starting Tic-Tac-Toe Tests...");
    
    try {
        testValidatorInput();
        testCellTaken();
        testBoardState();
        
        print("\nAll tests passed successfully!");
        return 0;
    } catch (const exception& e) {
        print("\nTest failed with exception: " + string(e.what()));
        return 1;
    }
}