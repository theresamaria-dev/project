#ifndef GAME_H
#define GAME_H

#include<stdio.h>
int verifyH(char** array, int rows, int cols, char c);
int verifyV(char** array, int rows, int cols, char c);
int verifyDown(char** array, int rows, int cols, char c);
int verifyUp(char** array, int rows, int cols, char c);
int verify(char** array, char c, int rows, int cols);
int easyBot(char** array);
int mediumBot(char** array, int rows, int cols);
int hardBot(char** array, int rows, int cols);
void replace(char** array, int x, char c, int rows);
void print(char** array, int rows, int cols);
int inRange(int a,int min,int max);

#endif

