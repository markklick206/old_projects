#include "stdafx.h"
#include "Falling Blocks.h"

//start the game
void GameReset();
//Draw the game board
void GameDrawBoard(HWND hwnd, HDC hdc);

void GameDrawBox(HWND hwnd, HDC hdc, FALLINGBOX b);

void GameDrawBall(HWND hwnd, HDC hdc);
void GameDrawBall2(HWND hwnd, HDC hdc);
void MoveLeft();

void MoveRight();

void MoveLeft2();

void MoveRight2();

