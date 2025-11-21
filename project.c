#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "game.h"
#define MAX_VALUE 100000


int isValid(char** board, int col) {
    if(board[0][col] == '.') return 1;
    return 0;
}

int evaluateBoard (char** board, int rows, int cols) {
    if(verify(char** board, 'B', rows, cols)) {
        return MAX_VALUE;
    }
    if(verify(char** board, 'A', rows, cols)) {
        return -MAX_VALUE;
    }
    return 0;
}

void undo(char** board, int rows, int col) {
    for (int i=0; i<rows, i++) {
        if(board[i][col] != '.') {  //find the first non-empty slot and remove the letter
            board[i][col] = '.';
            break;
        }
    }
}

//the minimax function
int minimax(char** board, int rows, int cols, int depth, int alpha, int beta, int turn) { //alpha and beta are used for pruning
    int score = evaluateBoard(board, rows, cols);
    if(score == 0 || score == MAX_VALUE || score == -MAX_VALUE) { //if we already have a winner then return the score of the winner
        return score;
    }
    //if turn = 1 then it is the bot's turn, otherwise it is the player's turn
    //the bot should try to maximize the score, so when turn = 1 maximizing score, turn = 0 minimizing score
    if(turn == 1) {
        int maxScore = INT_MIN; //find the maximum evaluation score that makes the bot win
        for (int i=0; i<cols; i++) {
            if(isValid(board, i)) {
                replace(board, i, 'B', rows);
                int s = minimax(board, rows, cols, depth -1, alpha, beta, 0); //recurse
                undo(board, rows, i);

                if(s > maxScore) {
                    maxScore = s;
                }
                if(s > alpha) alpha = s;
                if(beta <= alpha) break;
            }
        }
        return maxScore;
    }

    else {
        int minScore = INT_MAX; //find the minimum evaluation score that prevents the player from winning
        for (int i=0; i<cols; i++) {
            if(isValid(board, i)) {
                replace(board, i, 'A', rows);
                int s = minimax(board, rows, cols, depth -1, alpha, beta, 1); //recurse
                undo(board, rows, i);

                if(s < minScore) {
                    minScore = s;
                }
                if(s < beta) beta = s;
                if(beta <= alpha) break;
            }
        }
        return minScore;
    }
}

//the hard bot function

//if the level is easy
int easyBot(char** array) {
    int col;
    do {
        col = rand() % 7; //choose a column
    } while(array[0][col] != '.'); //if the top of the column is not empty then the whole column is filled so stop the loop
    return col+1; //to return the column chosen by the bot
}


//if the level is medium
int hardBot(char** array, int rows, int cols) {
    
    // Time Complexity Analysis:
    // Outer loop runs 'cols' times → O(cols)
    // Finding lowest empty row in worst case scans all rows → O(rows)
    // verify() is called once per column → O(T_verify)
    // Overall time: O(cols * (rows + T_verify))

    // If verify() scans the whole board (rows * cols):
    // Total = O(cols * (rows + rows*cols)) = O(rows * cols^2)
    
    // the bot tries to win
    for(int i=0; i<cols; i++) {
        if(array[0][i] == '.') { //if the column is not filled
            int row = rows -1;
            while(row>=0 && array[row][i] != '.') { //decrement the row until you find an empty slot
                row--;
            }
            if(row >=0) {
                array[row][i] = 'B'; //try to put B at array[row][i]
                if(verify(array, 'B', rows, cols)) { //if this move can let the bot win, return column i
                    array[row][i] = '.';
                    return i+1;
                }
                array[row][i] = '.'; //else reset the char at array[row][i] to '.'
            }
        }
    }
    
    // if A can win, try to block it
    for(int i=0; i<cols; i++) {
        if(array[0][i] == '.') { // if the column is not filled
            int row = rows-1;
            while(row>=0 && array[row][i] != '.') { //try to find an empty slot
                row--;
            }
            if(row >=0) {
                array[row][i] = 'A'; //try to put A at array[row][i]
                if(verify(array, 'A', rows, cols)) { //if the player can win, then return column i so the bot can block it
                    array[row][i] = '.';
                    return i+1;
                }
                array[row][i] = '.'; //else reset array[row][i] to '.'
            }
        }
    }
    
    // if the bot and the player cannot win immediately, then generate a random move
    int col;
    do {
        col = rand() % cols;
    } while (array[0][col] != '.');
    return col+1;
}

//check if there are 4 identical symbols horizontally
int verifyH(char** array, int rows, int cols, char c) {
    for(int i=0; i<rows; i++) { //loop through each row
      for(int j=0; j<cols-3; j++) { //stop at col-3 to avoid overflow
        if(array[i][j]==c && array[i][j+1]==c && array[i][j+2]==c && array[i][j+3]==c) {
          return 1; //a horizontal win is found
        }
      }
    }
    return 0; //no horizontal win
}

//check if there are 4 identical symbols vertically
int verifyV(char** array, int rows, int cols, char c) {
    for(int i=0; i<rows-3; i++) { //stop at row-3 to avoid overflow
      for(int j=0; j<cols; j++) { //loop through each column
        if(array[i][j]==c && array[i+1][j]==c && array[i+2][j]==c && array[i+3][j]==c){
          return 1; // a vertical win is found
        }
      }
    }
    return 0; //no vertical win
}

//check if there are 4 identical symbols in a diagonal line from top left to bottom right
int verifyDown(char** array, int rows, int cols, char c) { 
    for(int i=0; i<rows-3; i++) { 
      for(int j=0; j<cols-3; j++) {
        if(array[i][j]==c && array[i+1][j+1]==c && array[i+2][j+2]==c && array[i+3][j+3]==c) {
          return 1;// a diagonal win is found
        }
      }
    }
    return 0;//no diagonal win
}

//check if there are 4 identical symbols in a diagonal line from bottom left to top right
int verifyUp(char** array, int rows, int cols, char c) {
     for(int i = 3; i < rows; i++) {          
        for(int j = 0; j < cols - 3; j++) {
        if(array[i][j]==c && array[i-1][j+1]==c && array[i-2][j+2]==c && array[i-3][j+3]==c) {
          return 1;// a diagonal win is found
        }
      }
    }
    return 0;// no diagonal win
}

//verify if a player won
int verify(char** array, char c, int rows, int cols) { 
  if(verifyH(array, rows, cols, c) || verifyV(array, rows, cols, c) || verifyDown(array, rows, cols, c) || verifyUp(array, rows, cols, c)) {
    return 1;
  }
  return 0;
}

// replace the dot with A or B
void replace(char** array, int x, char c, int rows) { 
  x=x-1;
  for (int i=rows-1; i>=0; i--) {
    if (array[i][x] == '.') {
      array[i][x] = c;
      break;
    }
  }
}

// print the grid
void print(char** array, int rows, int cols) { 
  for(int i=0; i<rows; i++) {
    for(int j=0; j<cols; j++) {
      printf("%c ", array[i][j]);
    }
    printf("\n");
  }
   printf("1 2 3 4 5 6 7");
}
int inRange(int a,int min,int max){
    return (a>=min && a<=max);
}
