//Comments:
// Can you make the colors a const int, rather than hardcoding them in the draw functions?
// 45/45

#include "stdafx.h"
#include "GameMgr.h"

int xRectb, yRectb, xIncb, xRectc, yRectc, xIncc;
const int XBALLINC = 0;
static bool MoveL = false;
static bool MoveR = false;
static bool MoveL2 = false;
static bool MoveR2 = false;
int cBlocks;
bool gameOver; // is the game over?
static int deadblocks;
FALLINGBOX Box[MAXBLOCKS];
int currentspeed = FALLSPEED;


void GameReset() {
	gameOver = false;
	cBlocks = 0;
	for (int i=0; i<MAXBLOCKS; i++){
		Box[i].xRect = BLANK; //XBALLSTART + rand() % (windowWidthMin - BALLWIDTH - 16);
		Box[i].yRect = BLANK; //YBALLSTART;
		Box[i].yInc = FALLSPEED;
	}
	xRectb = XBALLSTART + 230;
	xRectc = XBALLSTART + 200;
	yRectb = YBALLSTART + 750;
	yRectc = YBALLSTART + 750;
	xIncb = XBALLINC;
	xIncc = XBALLINC;
	deadblocks = 0;
	currentspeed = FALLSPEED;
} //start the game over

void GameDrawBoard(HWND hwnd, HDC hdc) {
	int i = 0;
	bool gamespeedup = false;
	//static int deadblocks=0;
	//static bool delay = false;
	GameDrawBall(hwnd, hdc);
	GameDrawBall2(hwnd, hdc);
	RECT r;
	GetClientRect(hwnd, &r);
	bool newbox = rand() % 5;//increase amount of blocks on screen
	if (newbox == 0 && cBlocks<MAXBLOCKS) {
		while (Box[i].yRect !=BLANK && i < MAXBLOCKS){
			//finds blank block 
			i++;
	}
		
	if (i != MAXBLOCKS)  //if not the last box then draw it
		{//each block is its own variable
		Box[i].xRect = XBALLSTART + rand() % (windowWidthMin - BALLWIDTH - 16);
		Box[i].yRect = YBALLSTART;
		Box[i].yInc = currentspeed;
		Box[i].xInc = rand() % 3 - 1;//horizontal -1 to 1 movement
		cBlocks++;
	}

	if(deadblocks < 25)
		gamespeedup = false;
	if(deadblocks > 25 && deadblocks <28)
		gamespeedup = true;
	if (deadblocks > 28 && deadblocks < 50)
		gamespeedup = false;
	if (deadblocks > 50 && deadblocks < 55)
		gamespeedup = true;
	else 
		gamespeedup = false;
	switch(gamespeedup){ //determines level speeds
	case FALSE: 
		break;
	case TRUE:
		for(int i =0;i<MAXBLOCKS;i++){
			Box[i].yInc = currentspeed + INCSPEED;
		if(Box[i].yRect != -1)
			Box[i].yInc = currentspeed + INCSPEED;
		}
		currentspeed += INCSPEED;
		break;
	}
}


	for (i=0; i<MAXBLOCKS; i++){
		if(Box[i].yRect > (r.bottom - r.top) + 5) {
			Box[i].yRect = BLANK;//count how many blocks go out of the window
			cBlocks--;//spawn a new block
			deadblocks++;//counts blocks that have left the window
		}
		if (Box[i].yRect != BLANK) {
			Box[i].yRect += Box[i].yInc;
			Box[i].xRect += Box[i].xInc;
			GameDrawBox(hwnd, hdc, Box[i]);
		}
	}
		
	
}

void GameCheckWinner(HWND hwnd) {} //Is there a winner?

void GameDrawBox(HWND hwnd, HDC hdc, FALLINGBOX b) {
	int i = rand()%90;
	int j = rand()%30;
	int k = rand()%70;
	HPEN pen = CreatePen(PS_SOLID, 3 , RGB(i,j,k));
	HGDIOBJ penOld;
	penOld = SelectObject(hdc, pen);
	Rectangle(hdc, b.xRect, b.yRect, b.xRect+BALLWIDTH, b.yRect+BALLHEIGHT);
	SelectObject(hdc,penOld);
	DeleteObject(pen);
}

void GameDrawBall(HWND hwnd, HDC hdc) {
	RECT s;
	GetClientRect(hwnd, &s);
	
	if(MoveL){
		xIncb = XBALLINC - 5;
	}
	else if (MoveR){
		xIncb = XBALLINC + 5;
	}
	//else
		//xIncb = XBALLINC;
	
	
	if (xRectb > (s.right - s.left) - BALLWIDTH - BORDER) //when ball hits edge of the window
		if(xIncb > 0)
			xIncb = 0;
	if (xRectb < s.left + BORDER)
		if(xIncb < 0)
			xIncb = 0;

	xRectb += xIncb;
	MoveL=false;
	MoveR=false;
	
	HPEN pen = CreatePen(PS_SOLID, 7 , RGB(50,0,50));
	HGDIOBJ penOld;
	penOld = SelectObject(hdc, pen);
	RoundRect(hdc, xRectb, yRectb,
          xRectb+BALLWIDTH, yRectb+BALLHEIGHT, 2, 2);
	SelectObject(hdc,penOld);
	DeleteObject(pen);
}

void GameDrawBall2(HWND hwnd, HDC hdc) {
	RECT l;
	GetClientRect(hwnd, &l);
	
	if(MoveL2){
		xIncc = XBALLINC - 5;
	}
	else if (MoveR2){
		xIncc = XBALLINC + 5;
	}
	//else
		//xIncb = XBALLINC;
	
	
	if (xRectc > (l.right - l.left) - BALLWIDTH - BORDER) //when ball hits edge of the window
		if(xIncc > 0)
			xIncc = 0;
	if (xRectc < l.left + BORDER)
		if(xIncc < 0)
			xIncc = 0;

	xRectc += xIncc;
	MoveL2=false;
	MoveR2=false;
	
	HPEN pen = CreatePen(PS_SOLID, 7 , RGB(0,0,150));
	HGDIOBJ penOld;
	penOld = SelectObject(hdc, pen);
	RoundRect(hdc, xRectc, yRectc,
          xRectc+BALLWIDTH, yRectc+BALLHEIGHT, 2, 2);
	SelectObject(hdc,penOld);
	DeleteObject(pen);
}

void MoveLeft(){
	MoveL = true;
}

void MoveRight(){
	MoveR = true;
}

void MoveLeft2(){
	MoveL2 = true;
}

void MoveRight2(){
	MoveR2 = true;
}