#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "game.h"
#include <limits.h>
#define MAX_VALUE 100000


int getLowestEmptyRow(char** board, int rows, int col) {
    for (int r = rows - 1; r >= 0; r--) {
        if (board[r][col] == '.') return r;
    }
    return -1;
}

int isValid(char** board, int col) {
    if(board[0][col] == '.') return 1;
    return 0;
}

int windowEval(char window[4], char bot, char opponent) { 
    //this function checks for dangerous zones and assigns weights accordingly
    int b = 0, opp = 0, empty = 0; 
    for (int i = 0; i < 4; i++) {
        if (window[i] == bot){ b++; }
        else if (window[i] == opponent) {opp++; }
        else {empty++; }
    }
    if (b == 4){ return MAX_VALUE; }
    if (b == 3 && empty == 1) {return MAX_VALUE/2; }
    if (b == 2 && empty == 2) {return MAX_VALUE/4;} 

    if (opp == 4) {return -MAX_VALUE;} 
    if (opp == 3 && empty == 1){ return -MAX_VALUE/2;  }
    if (opp == 2 && empty == 2) {return -MAX_VALUE/4;  }

    return 0;
}

int evaluateBoard(char** board, int rows, int cols, char bot, char opp) {
    // give each position a weight depending on the number of possibilties it gives the bot to perform
    int score = 0;
    int weights[rows][cols] = { 
        {3, 4, 5, 7, 5, 4, 3},
        {4, 6, 8,13, 8, 6, 4}, 
        {4, 6, 8,13, 8, 6, 4}, 
        {3, 5, 7, 9, 7, 5, 3},
        {2, 4, 6, 8, 6, 4, 2},
        {1, 2, 3, 4, 3, 2, 1}
    };// note that the scores can be tuned and watch how the bot reacts with each change
//add the weights to the score if its a botmove the score is positive if its a humanmove the score is negative
    for(int r = 0; r < rows; r++) {
        for(int c = 0; c < cols; c++) {
            if(board[r][c] == bot) {score += weights[r][c]; }
            else if(board[r][c] == opp){ score -= weights[r][c]; }
        }
    }

    char window[4];
    // check window directions
    //horizontal
    for(int r = 0; r < rows; r++) {
        for(int c = 0; c < cols - 3; c++) {
            for(int k = 0; k < 4; k++) {window[k] = board[r][c + k]; }
            score += windowEval(window, bot, opp); 
        }
    }
   // vertical
    for(int c = 0; c < cols; c++) {
        for(int r = 0; r < rows - 3; r++) {
            for(int k = 0; k < 4; k++){ window[k] = board[r + k][c]; }
            score += windowEval(window, bot, opp); 
        }

    }
   //diagonally up
    for(int r = 3; r < rows; r++) {
        for(int c = 0; c < cols - 3; c++) {
            for(int k = 0; k < 4; k++){ window[k] = board[r - k][c + k]; }
            score += windowEval(window, bot, opp); 
        }

    }
// diagonally down
    for(int r = 0; r < rows - 3; r++) {
        for(int c = 0; c < cols - 3; c++) {
            for(int k = 0; k < 4; k++){ window[k] = board[r + k][c + k]; }
            score += windowEval(window, bot, opp); 
        }
    }

    return score;
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
int minimax(char** board, int rows, int cols, int depth, int alpha, int beta, int turn, char bot , char opp) { //alpha and beta are used for pruning
    int score = evaluateBoard(board, rows, cols , bot, opp);
    if(depth == 0 || score == MAX_VALUE || score == -MAX_VALUE) { //if we already have a winner then return the score of the winner
        return score;
    }
    //if turn = 1 then it is the bot's turn, otherwise it is the player's turn
    //the bot should try to maximize the score, so when turn = 1 maximizing score, turn = 0 minimizing score
    if(turn == 1) {
        int maxScore = INT_MIN; //find the maximum evaluation score that makes the bot win
        for (int i=0; i<cols; i++) {
            if(isValid(board, i)) {
                int r = getLowestEmptyRow(board, rows, c);
                board[r][c] = bot;
                int s = minimax(board, rows, cols, depth -1, alpha, beta, 0 , bot, opp); //recurse
                board[r][c] = '.';

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
                int r = getLowestEmptyRow(board, rows, c);
                board[r][c] = opp;
                int s = minimax(board, rows, cols, depth -1, alpha, beta, 1); //recurse
                board[r][c] = '.';

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


int hardBot(char** array, int rows, int cols) { 
 int bestScore = -MAX_VALUE;
 int bestcol = -1;
 int depth = 6; // if depth set too small the bot becomes stupid if too large the bot becomes slow
 int center = cols/2; // to be used in calculating the center moves since center based moves have higher possibilties and work well with minimax
 char bot = 'B';
 char opp = 'A';

 for (int offset = 0; offset < cols; offset++) {
        int col;
        if (offset % 2 == 0) {
            col = center - (offset / 2);// for even
        } 
        else {
            col = center + (offset / 2 + 1);// for odd
    }
     if (col < 0 || col >= cols) continue;
     if (!isValid(array, col)) continue;
    int r = getLowestEmptyRow(array, rows, col);
    if (r == -1) continue;
    array[r][col] = bot;
    score = minimax(board,rows,cols,depth -1,MAX_VALUE,-MAX_VALUE,1,bot,opp);
    array[r][col] = '.';
    if (score>bestscore){ 
         bestscore = score; 
         bestcol=col; 
    }
            
 }
 return bestcol+1;
}


//if the level is medium
int mediumBot(char** array, int rows, int cols) {
    
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
