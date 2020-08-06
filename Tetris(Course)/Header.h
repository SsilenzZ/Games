#pragma once

#include <Windows.h>
#include <iostream>
#include <ctime>
#include <fstream>

#define ID_TIMER		100

#define MENU_BLOCK		101
#define MENU_BK			102
#define MENU_INFO		103

#define MENU_RESTART	104
#define MENU_HELP		105



#define MENU_LOWER		106
#define MENU_FASTER		107

#define MENU_PAUSE		108

#define MENU_RECORDS	109

#define ID_VIOLET		110
#define ID_WHITE		111
#define ID_GREEN		112
#define ID_GRAY			113
#define ID_BLACK		114
#define ID_CROSS		115

#define ID_BK_WHITE		116
#define ID_BK_VIOLET	117
#define ID_BK_GREEN		118
#define ID_BK_GRAY		119
#define ID_BK_BLUE		120
#define ID_BK_YELLOW	121

#define ID_INFO_BLUE	122
#define ID_INFO_WHITE	123
#define ID_INFO_GREEN	124
#define ID_INFO_GRAY	125
#define ID_INFO_VIOLET	126
#define ID_INFO_YELLOW	127

#define DialogChangeBlockColor	128
#define DialogChangeBkColor		129
#define DialogChangeInfoColor	130
#define DialogStart				131

#define ID_LV0	132
#define ID_LV1	133
#define ID_LV2	134
#define ID_LV3	135
#define ID_LV4	136
#define ID_LV5	137
#define ID_LV6	138
#define ID_LV7	139
#define ID_LV8	140
#define ID_LV9	141

#define IDM_TEXT 142

#define DialogRecords 143
#define DialogHelp	  144

struct Figure
{
	unsigned short int Form[4];
	short int
		x, y,
		figure, state;
};

struct Pole
{
	unsigned short int
		Xmax, Xmin, Ymax, Ymin, checkNumber,
		*field;
};

struct Players
{
	char * name;
	int score;
};

struct Game
{
	char*name;
	COLORREF colorInfo, colorLine;
	HANDLE blockBitMap, bkBitMap;
	short int level, score, NumberOfBitmap, NumberOfBkBitmap, NumberOfInfoColor,
		BlockWidth, BlockHeight, lengthLine;

	HDC hCompatibleDC, hBkDC;
};

void SetFigure(Figure & f, short int x, short int y, short int figure = -1, short int state = -1);

void ClearField(unsigned short int * field, short int size);

bool InsertToField(Pole & pole, Figure & figure);

bool move(Pole & pole, Figure & figure, short int action);

void Rotation(Pole & pole, Figure & figure);

void NextFigure(Figure & figure, short int x, short int y, bool newF = 1);

void checkLines(Pole & pole, short int y, Game & game);

bool CanIAct(const Pole & pole, const Figure & figure);

enum
{
	SquareFirst =	0b0000011001100000,

	LineFirst =		0b0010001000100010,
	LineSecond =	0b0000111100000000,

	GLeftFirst =	0b0000000101110000,
	GLeftSecond =	0b0010001000110000,
	GLeftThird =	0b0000011101000000,
	GLeftFourth =	0b0000011000100010,

	GRightFirst =	0b0000010001110000,
	GRightSecond =	0b0000011001000100,
	GRightThird =	0b0000111000100000,
	GRightFourth =	0b0010001001100000,

	TFirst =		0b0000001001110000,
	TSecond =		0b0000001000110010,
	TThird =		0b0000000001110010,
	TFourth =		0b0000001001100010,

	ZLeftFirst =	0b0000011000110000,
	ZLeftSecond =	0b0000001001100100,

	ZRightFirst =	0b0000001101100000,
	ZRightSecond =	0b0000010001100010,
};

enum
{
	Square = 1,
	GLeft,
	GRight,
	T,
	ZLeft,
	ZRight,
	Line,

	Up,
	Down,
	Left,
	Right
};
