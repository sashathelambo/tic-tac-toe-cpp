# Tic-Tac-Toe Game

This is a simple command-line Tic-Tac-Toe game implemented in C++. The game allows you to play against another player or against the computer.

## Requirements

- C++ compiler (C++11 or later)
- Terminal or command prompt

## How to Play

1. Clone the repository or download the source code files.

2. Open a terminal or command prompt and navigate to the directory containing the source code files.

3. Compile the code using the following command:
   ```
   g++ -o tictactoe main.cpp
   ```

4. Run the compiled executable:
   ```
   ./tictactoe
   ```

5. The game will start, and you will be presented with a menu to choose between playing against the computer or another player.

6. Enter your choice by pressing '1' to play against the computer or '2' to play against another player.

7. The game board will be displayed, and you will be prompted to enter a position (1-9) to place your marker (X or O).

8. Take turns with the other player or the computer to place your markers on the board.

9. The game will continue until one player wins or the game ends in a draw.

10. After each game, you will be asked if you want to play again. Enter 'y' to play again, 'n' to exit the game, or 'm' to return to the menu.

## Controls

- Enter a number between 1 and 9 to place your marker on the corresponding position on the board.
- Press 'ESC' at any time to exit the game.
- Press 'M' at any time to return to the menu.

## Game Rules

- The game is played on a 3x3 grid.
- Player 1 is assigned the marker 'X', and Player 2 (or the computer) is assigned the marker 'O'.
- Players take turns placing their markers on an empty cell of the grid.
- The first player to get three of their markers in a row (horizontally, vertically, or diagonally) wins the game.
- If all cells are filled and no player has won, the game ends in a draw.

## File Structure

- `main.cpp`: Contains the main game logic and user interface.
- `validator.hpp`: Contains utility functions for input validation and checking game conditions.

## Customization

- You can modify the `setColor` function in `main.cpp` to change the colors of the game messages and board.
- The game board is represented by a vector of strings, and you can customize its appearance by modifying the `displayBoard` function in `main.cpp`.

Enjoy playing Tic-Tac-Toe!
