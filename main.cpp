#include <iostream>
#include <cstdint>
#include <cstdio>
#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib> 
#include <ctime>
#include <fstream>
#include <random>

using namespace std;

/**
 * Displays the game board.
 * @param board The game board represented as a 2D array.
 * @param rows The number of rows in the board.
 */
void displayBoard(char board[][7], int rows) {
    cout << "-----------------------------" << endl;
    for (int row = rows - 1; row >= 0; row--) {
        for (int col = 0; col < 7; col++) {
            cout << "| " << board[row][col] << " ";
        }
        cout << "| " << endl << "-----------------------------" << endl;
    }
    cout << "  1   2   3   4   5   6   7" << endl;
}

/**
 * Checks if there is a winning line on the board.
 * @param board The game board represented as a 2D array.
 * @param rows The number of rows in the board.
 * @param playerCode The character code representing the player.
 * @return True if a winning line is found, otherwise false.
 */
bool hasWinningLine(char board[][7], int rows, int playerCode) {
    for (int i = 0; i < 6; i++) { // Iterate through rows
        for (int j = 0; j < 7; j++) { // Iterate through columns
            if (board[i][j] == playerCode) {
                // Vertical check
                if (i <= 2 && board[i + 1][j] == playerCode && board[i + 2][j] == playerCode && board[i + 3][j] == playerCode) {
                    return true;
                }
                // Horizontal check
                if (j <= 3 && board[i][j + 1] == playerCode && board[i][j + 2] == playerCode && board[i][j + 3] == playerCode) {
                    return true;
                }
                // Diagonal right check
                if (i <= 2 && j <= 3 && board[i + 1][j + 1] == playerCode && board[i + 2][j + 2] == playerCode && board[i + 3][j + 3] == playerCode) {
                    return true;
                }
                // Diagonal left check
                if (i <= 2 && j >= 3 && board[i + 1][j - 1] == playerCode && board[i + 2][j - 2] == playerCode && board[i + 3][j - 3] == playerCode) {
                    return true;
                }
            }
        }
    }
    return false;
}

/**
 * Checks if the board is full.
 * @param columnCounts Array where each element represents the number of pieces in each column.
 * @param size The number of columns in the board.
 * @return True if the board is full, otherwise false.
 */
bool isBoardFull(int columnCounts[], int size) {
    int pieces = 0;
    for (int i = 0; i < size; i++) {
        pieces += columnCounts[i];
    }
    return pieces >= 42; // Maximum number of pieces (6 rows * 7 columns)
}

/**
 * Determines if there is a winning move available.
 * @param board The game board represented as a 2D array.
 * @param playerCode The character code representing the player.
 * @param column Reference to store the column of the winning move.
 * @return True if a winning move is found, otherwise false.
 */
bool findWinningMove(char board[][7], int playerCode, short& column) {
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 7; j++) {
            if (board[i][j] == playerCode) {
                // Vertical check
                if (i <= 2 && board[i + 3][j] == ' ' && board[i + 2][j] == playerCode && board[i + 1][j] == playerCode) {
                    column = j;
                    return true;
                }
                // Horizontal checks
                if (j <= 3 && board[i][j + 3] == ' ' && board[i][j + 2] == playerCode && board[i][j + 1] == playerCode) {
                    if (i == 0 || (i > 0 && board[i - 1][j + 3] != ' ')) {
                        column = j + 3;
                        return true;
                    }
                }
                if (j <= 4 && j >= 1 && board[i][j - 1] == ' ' && board[i][j + 2] == playerCode && board[i][j + 1] == playerCode) {
                    if (i == 0 || (i > 0 && board[i - 1][j - 3] != ' ')) {
                        column = j - 1;
                        return true;
                    }
                }
                // Diagonal right check
                if (i <= 2 && j <= 3 && board[i + 3][j + 3] == ' ' && board[i + 2][j + 2] == playerCode && board[i + 1][j + 1] == playerCode && board[i + 2][j + 3] != ' ') {
                    column = j + 3;
                    return true;
                }
                // Diagonal left check
                if (i <= 2 && j >= 3 && board[i + 3][j - 3] == ' ' && board[i + 2][j - 2] == playerCode && board[i + 1][j - 1] == playerCode && board[i + 2][j - 3] != ' ') {
                    column = j - 3;
                    return true;
                }
            }
        }
    }
    return false;
}

/**
 * Applies strategy 7 to determine the best move.
 * @param board The game board represented as a 2D array.
 * @param column Reference to store the chosen column.
 * @return True if a move is found, otherwise false.
 */
bool applyStrategy7(char board[][7], short& column) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 4; j++) {
            if (board[i][j] == 'O') {
                if (board[i + 1][j + 1] == ' ') {
                    column = j + 1;
                    return true;
                }
                else if (board[i + 1][j + 1] == 'O') {
                    if (board[i + 2][j + 1] == ' ') {
                        column = j + 1;
                        return true;
                    }
                    else if (board[i + 2][j + 1] == 'O') {
                        if (board[i + 2][j] == ' ') {
                            column = j;
                            return true;
                        }
                        else if (board[i + 2][j] == 'O') {
                            if (board[i + 2][j + 2] == ' ' && board[i][j + 2] != ' ') {
                                column = j + 2;
                                return true;
                            }
                        }
                    }
                }
            }
        }
    }
    return false;
}

/**
 * Determines if there is a special winning move available.
 * @param board The game board represented as a 2D array.
 * @param playerCode The character code representing the player.
 * @param column Reference to store the column of the winning move.
 * @return True if a special winning move is found, otherwise false.
 */
bool findSpecialWinningMove(char board[][7], int playerCode, short& column) {
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 7; j++) {
            if (board[i][j] == playerCode) {
                // Horizontal special check
                if (j <= 3) {
                    if (board[i][j + 1] == ' ' && board[i][j + 2] == playerCode && board[i][j + 3] == playerCode && (i == 0 || board[i - 1][j + 1] != ' ')) {
                        column = j + 1;
                        return true;
                    }
                    if (board[i][j + 1] == playerCode && board[i][j + 2] == ' ' && board[i][j + 3] == playerCode && (i == 0 || board[i - 1][j + 2] != ' ')) {
                        column = j + 2;
                        return true;
                    }
                }
                // Diagonal right special check
                if (i <= 2 && j <= 3) {
                    if (board[i + 1][j + 1] == ' ' && board[i + 2][j + 2] == playerCode && board[i + 3][j + 3] == playerCode && board[i][j + 1] != ' ') {
                        column = j + 1;
                        return true;
                    }
                }
                // Diagonal left special check
                if (i <= 2 && j >= 3) {
                    if (board[i + 1][j - 1] == ' ' && board[i + 2][j - 2] == playerCode && board[i + 3][j - 3] == playerCode && board[i][j - 1] != ' ') {
                        column = j - 1;
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

/**
 * Makes a move for the computer player.
 * @param board The game board represented as a 2D array.
 * @param columnCounts Array where each element represents the number of pieces in each column.
 */
void makeComputerMove(char board[][7], int columnCounts[]) {
    short column = -1;

    if (findWinningMove(board, 'O', column) || findWinningMove(board, 'X', column) ||
        findSpecialWinningMove(board, 'O', column) || findSpecialWinningMove(board, 'X', column) ||
        applyStrategy7(board, column)) {
        // Successful move found
    }
    else {
        // Random move
        srand(static_cast<unsigned>(time(0)));
        do {
            column = rand() % 7;
        } while (columnCounts[column] >= 6);
    }

    for (int i = 0; i < 6; i++) {
        if (board[i][column] == ' ') {
            board[i][column] = 'O';
            break;
        }
    }
    columnCounts[column]++;
}

/**
 * Main function to run the Connect Four game.
 * @return Exit status code.
 */
int main() {
    // Initialization
    char board[6][7] = { ' ' };
    int columnCounts[7] = { 0 };
    int turns = 0;
    bool gameOver = false;
    char playerInput;
    int chosenColumn;

    // Display initial game board
    displayBoard(board, 6);

    while (!gameOver) {
        // Player move
        cout << "Enter column (1-7) for your move: ";
        cin >> playerInput;
        chosenColumn = playerInput - '1';

        if (chosenColumn < 0 || chosenColumn >= 7 || columnCounts[chosenColumn] >= 6) {
            cout << "Invalid move. Try again." << endl;
            continue;
        }

        for (int i = 0; i < 6; i++) {
            if (board[i][chosenColumn] == ' ') {
                board[i][chosenColumn] = 'X';
                break;
            }
        }
        columnCounts[chosenColumn]++;
        turns++;

        // Check if player wins
        if (hasWinningLine(board, 6, 'X')) {
            displayBoard(board, 6);
            cout << "Congratulations! You win!" << endl;
            break;
        }

        // Check if board is full
        if (isBoardFull(columnCounts, 7)) {
            displayBoard(board, 6);
            cout << "It's a draw!" << endl;
            break;
        }

        // Computer move
        makeComputerMove(board, columnCounts);
        turns++;

        // Check if computer wins
        if (hasWinningLine(board, 6, 'O')) {
            displayBoard(board, 6);
            cout << "Computer wins!" << endl;
            break;
        }

        // Display the updated game board
        displayBoard(board, 6);
    }

    return 0;
}
