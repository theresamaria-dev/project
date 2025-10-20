#include <stdio.h>
#include <stdlib.h>
#include "game.h"


//if the level is easy
int easyBot(char** array) {
    int col;
    do {
        col = rand() % 7; //choose a column
    } while(array[0][col] != '.'); //if the top of the column is not empty then the whole column is filled so stop the loop
    return col; //to return the column chosen by the bot
}


//if the level is hard
int hardBot(char** array, int rows, int cols) {
    // the bot tries to win
    for(int i=0; i<cols; i++) {
        if(array[0][i] == '.') {
            int row = 5;
            while(row>=0 && array[row][i] != '.') { 
                row--;
            }
            if(row >=0) {
                array[row][i] = 'B'; 
                if(verify(array, 'B', rows, cols)) { 
                    array[row][i] = '.';
                    return i;
                }
                array[row][i] = '.'; 
            }
        }
    }
    
    // if A can win, try to block it
    for(int i=0; i<cols; i++) {
        if(array[0][i] == '.') { // if the column is not filled
            int row =5;
            while(row>=0 && array[row][i] != '.') { //try to find an empty space
                row--;
            }
            if(row >=0) {
                array[row][i] = 'A'; 
                if(verify(array, 'A', rows, cols)) {  
                    array[row][i] = '.';
                    return i;
                }
                array[row][i] = '.'; 
            }
        }
    }
    
    // if the bot and the player cannot win immediately, then generate a random move
    int col;
    do {
        col = rand() % 7;
    } while (array[0][col] != '.');
    return col;
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
    for(int i=6; i>=rows-3; i--) {
      for(int j=0; j<cols-3; j++) {
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
int main() {
    const int rows = 6;
    const int cols = 7;
    int count =0;
  
    printf("Welcome to Connect Four\n");
    char space;
    printf("Player A: ");
    char c;
    scanf("%c", &c);
    while(c!='A'){
     printf("Invalid character enter again:");
     scanf("%c",&c);}
    scanf("%c",&space);
  
    printf("\nPlayer B: ");
    char c2;
    scanf("%c", &c2);
    while(c2!='B'){
     printf("Invalid character enter again:");
     scanf("%c",&c2);}
  
    char** array;
    array = (char**) malloc(rows*sizeof(char*));
    for(int i=0; i<=rows; i++) {
    array[i] = (char*) malloc(cols*sizeof(char));
    for(int j=0; j<=cols; j++) {
      array[i][j] = '.';
    }
  }
  
  print(array, rows, cols);

   // while no player won, keep on asking the players to enter a column 
  while(!(verify(array, c2, rows, cols)) && !(verify(array, c, rows, cols)) && count<=rows*cols) {
    printf("\nPlayer A, choose a column(1-7): ");
    int x;
    scanf("%d", &x);
    if(inRange(x){
    replace (array, x, c, rows);}
        else { 
            printf("Invalid column choose again:"); 
            scanf("%d",&x);
            replace (array, x, c, rows);
    }
    print(array, rows, cols);
    count++;

      //if player A and B did not win yet, ask player B again to enter a column
    if(!(verify(array, c2, rows, cols)) && !(verify(array, c, rows, cols))) {
      printf("\nPlayer B, choose a column(1-7): ");
      int y;
      scanf("%d",&y);
        if(inRange(y){
             replace (array, y, c2, rows);}
        else { 
              printf("Invalid column choose again:");
              scanf("%d",&y);
              replace (array, y, c2, rows);
    }
      print(array, rows, cols);
      count++;
    }
  }

  // check if a player won
  if(verify(array, c, rows, cols)) {
    printf("\nPlayer A wins!");
  }
  else {
    printf("\nPlayer B wins!");
  }
    for(int i = 0; i<=rows; i++){
        free(array[i]);
    }
    free(array);
  
}
