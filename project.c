#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "game.h"
#include <ctype.h>
#include <limits.h>
#include <pthread.h>   // NEW

#define MAX_VALUE 100000
#define ROWS 6
#define COLS 7

static long long minimax_nodes = 0;

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
    // time complexity: O(rows * cols)
    // give each position a weight depending on the number of possibilties it gives the bot to perform
    int score = 0;
    int weights[ROWS][COLS] = { 
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
    for (int i = rows - 1; i >= 0; i--) {
        if(board[i][col] != '.') {  //find the first non-empty slot and remove the letter
            board[i][col] = '.';
            break;
        }
    }
}

int isBoardFull(char** board, int rows, int cols) {
    for (int c = 0; c < cols; c++) {
        if (board[0][c] == '.') {
            // If the top cell of a column is empty, the board is not full
            return 0;
        }
    }
    return 1;   // no empty top cells so the board is full
}

//the minimax function
int minimax(char** board, int rows, int cols, int depth, int alpha, int beta, int turn, char bot , char opp) { //alpha and beta are used for pruning
    minimax_nodes++;  // count this node
    int score = evaluateBoard(board, rows, cols , bot, opp); // O(rows*cols)
    // Stop if:
    //  - maximum search depth reached
    //  - a winning position is detected for either side
    //  - the board is full (draw, no further moves)
    if(depth == 0 || score == MAX_VALUE || score == -MAX_VALUE || isBoardFull(board, rows,cols)) {  //O(1)
        return score;
    }
    //if turn = 1 then it is the bot's turn, otherwise it is the player's turn
    //the bot should try to maximize the score, so when turn = 1 maximizing score, turn = 0 minimizing score
    if(turn == 1) {
        int maxScore = INT_MIN; //find the maximum evaluation score that makes the bot win
        for (int i=0; i<cols; i++) {
            if(isValid(board, i)) {
                int r = getLowestEmptyRow(board, rows, i);
                board[r][i] = bot; //try the bot's move
                // the time complexity of minimax is O(b^d)
                // b = number of valid moves, d = depth
                // the overall time complexity is O(7^depth) = O(7^6)
                int s = minimax(board, rows, cols, depth -1, alpha, beta, 0 , bot, opp); //recurse to evaluate the bot's move
                board[r][i] = '.'; //undo move

                if(s > maxScore) {
                    maxScore = s; // update maxScore
                }
                if(s > alpha) alpha = s; //for pruning
                if(beta <= alpha) break;
            }
        }
        return maxScore;
    }

    else {
        int minScore = INT_MAX; //find the minimum evaluation score that prevents the player from winning
        for (int i=0; i<cols; i++) {
            if(isValid(board, i)) {
                int r = getLowestEmptyRow(board, rows, i);
                board[r][i] = opp; //try the opponent's move
                // same time complexity for the recursive call for the bot
                // O(7^depth)
                int s = minimax(board, rows, cols, depth -1, alpha, beta, 1 , bot, opp); //recurse to evaluate the opponent's move
                board[r][i] = '.'; //undo move

                if(s < minScore) {
                    minScore = s; //update minScore
                }
                if(s < beta) beta = s; //for pruning
                if(beta <= alpha) break;
            }
        }
        return minScore;
    }
}

void resetMinimaxNodes(void) {
    minimax_nodes = 0;
}

long long getMinimaxNodes(void) {
    return minimax_nodes;
}

//if the level is easy
int easyBot(char** array) {
    int col;
    do {
        col = rand() % 7; //choose a column
    } while(array[0][col] != '.'); //if the top of the column is not empty then the whole column is filled so stop the loop
    return col+1; //to return the column chosen by the bot
}

typedef struct {
    char** board;
    int rows;
    int cols;
    int col;
    int depth;
    char bot;
    char opp;
    int score;
} ThreadArgs;

void* threadMinimax(void* arg) {
    ThreadArgs* t = (ThreadArgs*)arg;
    int rows = t->rows;
    int cols = t->cols;
    int col  = t->col;
    char bot = t->bot;
    char opp = t->opp;
    int depth = t->depth;

    // Allocate a copy of the board for this thread
    char** copy = (char**)malloc(rows * sizeof(char*));
    if (!copy) {
        t->score = -MAX_VALUE;
        pthread_exit(NULL);
    }

    for (int r = 0; r < rows; r++) {
        copy[r] = (char*)malloc(cols * sizeof(char));
        if (!copy[r]) {
            // cleanup on failure
            for (int k = 0; k < r; k++) free(copy[k]);
            free(copy);
            t->score = -MAX_VALUE;
            pthread_exit(NULL);
        }
        for (int c = 0; c < cols; c++) {
            copy[r][c] = t->board[r][c];
        }
    }

    int r = getLowestEmptyRow(copy, rows, col);
    if (r == -1) {
        t->score = -MAX_VALUE;
    } else {
        copy[r][col] = bot;
        t->score = minimax(copy, rows, cols, depth - 1, INT_MIN, INT_MAX, 0, bot, opp);
        copy[r][col] = '.';
    }

    // Free local copy
    for (int i = 0; i < rows; i++) {
        free(copy[i]);
    }
    free(copy);
    pthread_exit(NULL);
}
//the multithread hardBotParallel function
int hardBotParallel(char** array, int rows, int cols) { 
    int bestScore = -MAX_VALUE;
    int bestcol   = -1;
    char bot = 'B';
    char opp = 'A';

    // Count how many moves played so far (for dynamic depth)
    int count = 0;
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            if (array[r][c] != '.') count++;
        }
    }

    // If bot is playing very early, prefer center quickly
    if (count == 1 && isValid(array, 4)) return 5;
   

    // Dynamic depth based on game phase
    int depth;
    if (count <= 10) {
        depth = 7;   // early game
    } else {
        depth = 6;   // mid and late game
    }

    // 1) Check for immediate winning move (sequential)
    for (int i = 0; i < cols; i++) {
        if (isValid(array, i)) {
            int r = getLowestEmptyRow(array, rows, i);
            array[r][i] = bot;
            if (verify(array, bot, rows, cols)) {  // instant win
                array[r][i] = '.';
                return i + 1;
            }
            array[r][i] = '.';
        }
    }

    // 2) Check for immediate blocking move (sequential)
    for (int i = 0; i < cols; i++) {
        if (isValid(array, i)) {
            int r = getLowestEmptyRow(array, rows, i);
            array[r][i] = opp;
            if (verify(array, opp, rows, cols)) {  // block opponent
                array[r][i] = '.';
                return i + 1;
            }
            array[r][i] = '.';
        }
    }

    // 3) Collect candidate columns in center-first order
    int center = cols / 2;
    int candidates[COLS];
    int numCandidates = 0;

    for (int offset = 0; offset < cols; offset++) {
        int col;
        if (offset % 2 == 0) {
            col = center - (offset / 2);         // even offsets: go left
        } else {
            col = center + (offset / 2 + 1);     // odd offsets: go right
        }

        if (col < 0 || col >= cols) continue;
        if (!isValid(array, col)) continue;

        int r = getLowestEmptyRow(array, rows, col);
        if (r == -1) continue;

        candidates[numCandidates++] = col;
    }

    if (numCandidates == 0) {
        // No valid move (should not normally happen if checked properly)
        return 1;
    }

    // 4) Create one thread per candidate column
    pthread_t threads[numCandidates];
    ThreadArgs args[numCandidates];

    for (int i = 0; i < numCandidates; i++) {
        args[i].board = array;
        args[i].rows  = rows;
        args[i].cols  = cols;
        args[i].col   = candidates[i];
        args[i].depth = depth;
        args[i].bot   = bot;
        args[i].opp   = opp;
        args[i].score = -MAX_VALUE;

        pthread_create(&threads[i], NULL, threadMinimax, &args[i]);
    }

    // 5) Join threads and pick best score
    for (int i = 0; i < numCandidates; i++) {
        pthread_join(threads[i], NULL);
        if (args[i].score > bestScore) {
            bestScore = args[i].score;
            bestcol   = args[i].col;
        }
    }

    return bestcol + 1;
}
//With multithreading we reduce effective runtime to O(bᵈ / T) where T is the number of threads, b is the braching factor and d is the depth power

//the hard bot function

int hardBot(char** array, int rows, int cols) { 
 int bestScore = -MAX_VALUE;
 int score;
 int bestcol = -1;
 int depth = 6; // if depth set too small the bot becomes stupid if too large the bot becomes slow
 int center = cols/2; // to be used in calculating the center moves since center based moves have higher possibilties and work well with minimax
 char bot = 'B';
 char opp = 'A';

     int count = 0;
    for(int r=0; r<rows; r++){
        for(int c=0; c<cols; c++){
            if(array[r][c] != '.') count++;
        }
    }
    if(count == 1 && isValid(array, 4)) return 5;
   

 for (int i = 0; i < cols; i++) {
        if (isValid(array, i)) {
            int r = getLowestEmptyRow(array, rows, i);
            array[r][i] = bot;
            if (verify(array, bot, rows, cols)) {  // instant win
                array[r][i] = '.';
                return i + 1;
            }
            array[r][i] = '.';
        }
    }

    for (int i = 0; i < cols; i++) {
        if (isValid(array, i)) {
            int r = getLowestEmptyRow(array, rows, i);
            array[r][i] = opp;
            if (verify(array, opp, rows, cols)) {  
                array[r][i] = '.';
                return i + 1;  // block opponent
            }
            array[r][i] = '.';
        }
    }

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
    score = minimax(array,rows,cols,depth -1,INT_MIN,INT_MAX,0,bot,opp);
    array[r][col] = '.';
    if (score>bestScore){ 
         bestScore = score; 
         bestcol=col; 
    }
            
 }
 return bestcol+1;
}

int findWinningMove(char** array, int rows, int cols, char player) {
    for (int col = 0; col < cols; col++) {
        if (array[0][col] == '.') {
            int row = rows - 1;
            while (row >= 0 && array[row][col] != '.') {
                row--;
            }
            if (row >= 0) {
                array[row][col] = player;
                if (verify(array, player, rows, cols)) {
                    array[row][col] = '.';
                    return col + 1;   // columns are 1-based in the rest of the code
                }
                array[row][col] = '.';
            }
        }
    }
    return -1;  // no winning move
}


//if the level is medium
int mediumBot(char** array, int rows, int cols) {
    // 1) bot tries to win
    int winCol = findWinningMove(array, rows, cols, 'B');
    if (winCol != -1) return winCol;

    // 2) bot tries to block player
    int blockCol = findWinningMove(array, rows, cols, 'A');
    if (blockCol != -1) return blockCol;

    // 3) otherwise random move
    int col;
    do {
        col = rand() % cols;
    } while (array[0][col] != '.');
    return col + 1;
}
// ─────────────────────────────────────────────────────────────
// Time Complexity Analysis (mediumBot)
//
// mediumBot() calls findWinningMove() twice (once for bot, once for player).
//
// Inside findWinningMove():
// -Outer loop over all columns      → O(cols)
// -For each column:
//      - Loop to find lowest empty row      → O(rows)
//      - verify() call to check if move wins:
//            • verifyH, verifyV, verifyDown, verifyUp each scan the board
//            • Overall verify() cost = O(rows * cols)
//      - Total per column  = O(rows * cols)
// -Total per findWinningMove()   = O(cols * rows * cols)
//                                           = O(rows * cols²)
//
// mediumBot():
// -1 × findWinningMove('B') → O(rows * cols²)
// -1 × findWinningMove('A') → O(rows * cols²)
// -Random move selection     → O(cols) (negligible)
//
// Final worst-case complexity:
//      O(rows * cols²) + O(rows * cols²) + O(cols)
//    = O(rows * cols²)
// ─────────────────────────────────────────────────────────────


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
