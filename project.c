#include <stdio.h>
#include <stdlib.h>

//functions prorotypes
int verifyH(char** array, int rows, int cols, char c);
int verifyV(char** array, int rows, int cols, char c);
int verifyDown(char** array, int rows, int cols, char c);
int verifyUp(char** array, int rows, int cols, char c);

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

int main() {
  const int rows = 6;
  const int cols = 7;
   int count =0;
  
  printf("Welcome to Connect Four\n");
  char space;
  printf("Player A: ");
  char c;
  scanf("%c", &c);
  scanf("%c",&space);
  
  printf("Player B: ");
  char c2;
  scanf("%c", &c2);
  
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
    replace (array, x, c, rows);
    print(array, rows, cols);
    count++;

      //if player A and B did not win yet, ask player B again to enter a column
    if(!(verify(array, c2, rows, cols)) && !(verify(array, c, rows, cols))) {
      printf("\nPlayer B, choose a column(1-7): ");
      int y;
      scanf("%d",&y);
      replace (array, y, c2, rows);
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
