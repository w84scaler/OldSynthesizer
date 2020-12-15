#pragma once

#include "resource.h"

#define WHITE_WIDTH 60
#define BLACK_WIDTH 40
#define WHITE_HEIGHT 250
#define BLACK_HEIGHT 150

#define KEY_TOP 50
#define KEY_LEFT 50

#define INST_PIANO 3228
#define INST_GUITAR 3229
#define INST_DRUM 3230

#define OCT_CONTRA 3232
#define OCT_GREATE 3233
#define OCT_SMALL 3234
#define OCT_FIRST 3235
#define OCT_SECOND 3236
#define OCT_THIRD 3237

#define MENU_HELP 3238

typedef struct
{
	HBRUSH hbrWhite, hbrBlack, hbrGray;
	bool isWhitePress[19];
	bool isBlackPress[13];
	RECT whiteRect[19];
	RECT blackRect[13];
} KeyStruct, * PKeyStruct;

VOID InitMenu(HWND hWnd);