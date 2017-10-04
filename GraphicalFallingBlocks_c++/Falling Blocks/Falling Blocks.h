#pragma once

#include "resource.h"
#include <ctime>
#include <cstdlib>

const int MAXBLOCKS = 30;
const int BLANK = -1;

const int XBALLSTART = 0;
const int YBALLSTART = -40;
const int FALLSPEED = 5;
const int INCSPEED = 1;
const int BALLWIDTH = 20;
const int BALLHEIGHT = 20;

const int BORDER = 3;
const int windowHeightMin = 800;
const int windowWidthMin = 500;
const int windowHeightMax = 800;
const int windowWidthMax = 500;
const int ID_TIMER=8000;


class FALLINGBOX {
public:
	int xRect, yRect; //Rectangle to draw ball in
	int yInc; //how much to move the ball on each timer tick
	int xInc;
	
};