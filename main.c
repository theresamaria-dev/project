#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "game.h"
#include <ctype.h>
#include <limits.h>

int main() {
    srand(time(NULL));
    const int rows = 6;
    const int cols = 7;
    int count = 0;
    int choice;
    char mode;
    char c2;
    char c;
  
    printf("Welcome to Connect Four\n");

    while (1) {
        printf("Would you like to play against human or bot? (1 for human, 2 for bot): ");
        if (scanf("%d", &choice) != 1 || (choice != 1 && choice != 2)) {
            printf("Invalid input. Please enter 1 or 2.\n");
            while (getchar() != '\n');
            continue;
        }
        break;
    }
    while (getchar() != '\n');
    
    if (choice == 2) {
        while (1) {
            printf("Easy, Medium, or hard mode? (E for Easy, M for Medium, H for Hard): ");
            if (scanf(" %c", &mode) != 1) {
                printf("Invalid input. Please enter E or H.\n");
                while (getchar() != '\n');
                continue;
            }
            mode = toupper(mode); //convert character to uppercase
            if (mode != 'E' && mode != 'M' && mode != 'H') {
                printf("Invalid mode. Please enter E, M, or H.\n");
                while (getchar() != '\n');
                continue;
            }
            break;
        }
        while (getchar() != '\n');
    }

    while (1) {
        printf("Player A: \n");
        if (scanf(" %c", &c) != 1) {
            printf("Invalid input. Enter 'A'.\n");
            while (getchar() != '\n');
            continue;
        }
        if (c != 'A') {
            printf("Invalid character, please enter 'A'.\n");
            continue;
        }
        break;
    }
    while (getchar() != '\n');

    if (choice == 1) {
        while (1) {
            printf("\nPlayer B: \n");
            if (scanf(" %c", &c2) != 1) {
                printf("Invalid input. Enter 'B'.\n");
                while (getchar() != '\n');
                continue;
            }
            if (c2 != 'B') {
                printf("Invalid character, please enter 'B'.\n");
                continue;
            }
            break;
        }
        while (getchar() != '\n');
    } else {
        c2 = 'B';
    }

    char** array;
    array = (char**) malloc(rows * sizeof(char*));
    for (int i = 0; i < rows; i++) {
        array[i] = (char*) malloc(cols * sizeof(char));
        for (int j = 0; j < cols; j++) {
            array[i][j] = '.';
        }
    }
  
    print(array, rows, cols);

    // while no player won, keep on asking the players to enter a column 
    while (!(verify(array, c2, rows, cols)) && !(verify(array, c, rows, cols)) && count <= rows * cols) {
        printf("\nPlayer A, choose a column(1-7): \n");
        int x;
        while (1) { // if the user enters invalid input
            if (scanf("%d", &x) != 1 || !inRange(x, 1, 7) || array[0][x - 1] != '.') {
                printf("Invalid input, choose again.\n");
                while (getchar() != '\n'); 
                continue;  
            }
            break;
        }
            
        replace(array, x, c, rows);
        print(array, rows, cols);
        count++;

        // if player A and B did not win yet, ask player B again to enter a column
        int y;
        if (!verify(array, c2, rows, cols) && !verify(array, c, rows, cols) && count <= rows*cols) {
            printf("\nPlayer B, choose a column(1-7): \n");
            if (choice == 1) {
                while (1) {
                    if (scanf("%d", &y) != 1 || !inRange(y, 1, 7) || array[0][y - 1] != '.') {
                        printf("Invalid input, choose again.\n");
                        while (getchar() != '\n'); 
                        continue;
                    }
                    break;
                }
            } else {
                if (mode == 'E') { 
                    y = easyBot(array);
                } else if (mode == 'M') { 
                    y = mediumBot(array, rows, cols);
                } 
                else {
                    y = hardBot(array, rows, cols);
                }
            }
              
            replace(array, y, c2, rows);
            print(array, rows, cols);
            count++;
        }
    }

    // check if a player won
    if (verify(array, c, rows, cols)) {
        printf("\nPlayer A wins!");
    } else if (verify(array, c2, rows, cols)){
        printf("\nPlayer B wins!");
    }
    else {
        printf("\nIt's a draw!\n");
    }

    for (int i = 0; i < rows; i++) {
        free(array[i]);
    }
    free(array);
}
