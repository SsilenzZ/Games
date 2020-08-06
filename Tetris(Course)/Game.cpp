#define _CRT_SECURE_NO_WARNINGS

#include "Header.h"

HINSTANCE hInst;
ATOM		MyRegisterClass(HINSTANCE hInstance);
BOOL		InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
VOID CALLBACK TimerProc(HWND hWnd, UINT iMsg, UINT iTimerID, DWORD dwTime);

BOOL CALLBACK ChangeBlockColor(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK ChangeBkColor(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK ChangeInfoColor(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK Start(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK Records(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK Help(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam);

int WindowWidth = 460, WindowHeight = 660;
char szAppName[] = "Resource";
char ClassName[] = "Class";
Figure figure; Figure nextFigure; Pole pole; Game game; Players players[10];
bool pause = false, endGame = false;

void ShowRow(unsigned short int row, short int width,short int y, HDC hdc, HDC hCompatibleDC, BITMAP Bitmap, const Game & game)
{
	unsigned short int Checker = 0b1;
	Checker = Checker << width;
	short int x = 0;
	for (int i = 0; i <= width; i++)
	{
		if (row & Checker)
			StretchBlt(hdc, x, y, game.BlockWidth, game.BlockHeight, hCompatibleDC, 0, 0, Bitmap.bmWidth,
				Bitmap.bmHeight, SRCCOPY);
		Checker = Checker >> 1;
		x += game.BlockWidth;
	}
}

void ShowField(unsigned short int * field, short int size, short int width, HDC hdc, BITMAP Bitmap, BITMAP bkBitmap, const Game & game)
{
	StretchBlt(hdc, 0, 0, game.BlockWidth* (width + 1), game.BlockHeight * size, game.hBkDC, 0, 0, bkBitmap.bmWidth,
		bkBitmap.bmHeight, SRCCOPY);
	short int y = 0;
	for (short int i = 0; i < size; i++)
	{
		ShowRow(field[i], width, y, hdc, game.hCompatibleDC, Bitmap, game);
		y += game.BlockHeight;
	}
}

void InitField(HWND hWnd, Game & game)
{
	RECT rt;
	GetClientRect(hWnd, &rt);
	pole.Xmax = 15; pole.Xmin = 6; pole.Ymax = 19, pole.Ymin = 0, pole.checkNumber = pow(2, pole.Xmax - pole.Xmin + 1) - 1;
	pole.field = new unsigned short int[20];

	game.BlockWidth = 30;
	game.BlockHeight = 30;

	SetFigure(figure, pole.Xmin, pole.Ymin);
	ClearField(pole.field, 20);
	InsertToField(pole, figure);
}

void DrawInfo(HWND hWnd,HDC hdc, const Game & game, Figure & figure)
{
	HANDLE hFieldBitmap;
	BITMAP fieldBitmap, Bitmap;
	HDC hHelpDC;
	RECT rt;
	HPEN hPen, hOldPen;
	COLORREF oldColor;
	char help[30];

	hHelpDC = CreateCompatibleDC(hdc);

	hFieldBitmap = CreateCompatibleBitmap(hdc, 160, WindowHeight);
	SelectObject(hHelpDC, hFieldBitmap);
	GetObject(hFieldBitmap, sizeof(BITMAP), &fieldBitmap);
	GetObject(game.blockBitMap, sizeof(BITMAP), &Bitmap);

	hPen = CreatePen(PS_SOLID, 3, game.colorLine);
	GetClientRect(hWnd, &rt);
	FillRect(hHelpDC, &rt, CreateSolidBrush(game.colorInfo));

	float scale = 0.35;
	short int height = rt.bottom - rt.top;
	hOldPen = (HPEN)SelectObject(hHelpDC, hPen);

	oldColor = SetBkColor(hHelpDC, game.colorInfo);
	TextOut(hHelpDC, rt.left + 10, height * 0.25, game.name, strlen(game.name));

	wsprintf(help, "score: %d", game.score);
	TextOut(hHelpDC, rt.left + 20, height * 0.4, help, strlen(help));

	wsprintf(help, "Lv %d", game.level);
	TextOut(hHelpDC, rt.left + 20, height * 0.55, help, strlen(help));

	MoveToEx(hHelpDC, rt.left, height * 0.3, NULL);
	LineTo(hHelpDC, rt.right, height * 0.3);

	MoveToEx(hHelpDC, rt.left, height * 0.5, NULL);
	LineTo(hHelpDC, rt.right, height * 0.5);

	SetBkColor(hHelpDC, oldColor);
	DeleteObject(SelectObject(hdc, hOldPen));

	for (int i = 0; i < 4; i++)
	{
		ShowRow(figure.Form[i], 3, 0 + i*30, hHelpDC, game.hCompatibleDC, Bitmap, game);
	} 

	StretchBlt(hdc, 300, 0, 160, WindowHeight, hHelpDC, 0, 0, fieldBitmap.bmWidth,
		fieldBitmap.bmHeight, SRCCOPY);

	DeleteDC(hHelpDC);
}

void CheckRecords(Players * players, const char * name, int score)
{
	for (int i = 0; i < 10; i++)
	{
		if (score > players[i].score)
		{
			if (i < 9)
			{
				for (int j = 9; j > i; j--)
				{
					memcpy(players[j].name, players[j - 1].name, sizeof(char) * 10);
					players[j].score = players[j - 1].score;
				}
			}

			memcpy(players[i].name,name,sizeof(char) * 10);
			players[i].score = score;

			break;
		}
	}
}

void InitPlayes(Players * players)
{
	FILE *file;
	//fread(pstrBuffer, 1, iLength, file);
	file = fopen("log.txt", "rb");
	if (file == NULL)
	{
		CreateFileA("log.txt", GENERIC_READ | GENERIC_WRITE, NULL, NULL, NULL, FILE_ATTRIBUTE_NORMAL, NULL);
		/*file = fopen("log.txt", "rb");*/
	}
	char * buffer = new char[10];
	int x;
	fread(buffer, 1, 3, file);

	x = atoi(&buffer[0]);
	if (x)
	{
		for (int i = 0; i < 10; i++)
		{
			players[i].name = new char[11];

			fread(buffer, 1, 6, file);
			fread(buffer, 1, 10, file);

			memcpy(players[i].name, buffer, sizeof(char) * 10);
			players[i].name[10] = '/0';

			fread(buffer, 1, 8, file);
			fread(buffer, 1, 10, file);

			players[i].score = atoi(buffer);

			fread(buffer, 1, 2, file);
		}
	}
	else
	{	
		for (int i = 0; i < 10; i++)
		{
			players[i].name = new char[11];
			memcpy(players[i].name, "-         ", sizeof(char) * 10);
			players[i].name[10] = '\0';
			players[i].score = 0;
		}
	}

	fclose(file);
}

void startGame(HWND hWnd, HDC hdc, BITMAP & Bitmap, BITMAP & bkBitmap, HANDLE & hOldBitmap, Game & game, Figure & nextFigure)
{
	HINSTANCE hInstance;
	hdc = GetDC(hWnd);

	InitField(hWnd, game);
	InitPlayes(players);
	NextFigure(nextFigure, pole.Xmin, pole.Ymin);

	hInstance = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
	DialogBox(hInstance, MAKEINTRESOURCE(DialogStart), hWnd, Start);

	game.lengthLine = pole.Xmax - pole.Xmin + 1;
	game.NumberOfBitmap = 1;
	game.NumberOfBkBitmap = 1;
	game.NumberOfInfoColor = 1;

	game.blockBitMap = LoadBitmap(hInst, "block1");
	GetObject(game.blockBitMap, sizeof(BITMAP), &Bitmap);

	game.bkBitMap = LoadBitmap(hInst, "bk1");
	GetObject(game.bkBitMap, sizeof(BITMAP), &bkBitmap);

	game.colorInfo = RGB(0, 191, 255);
	game.colorLine = RGB(139, 0, 139);
	game.score = 0;

	DrawInfo(hWnd, hdc, game,nextFigure);
	game.hCompatibleDC = CreateCompatibleDC(hdc);
	game.hBkDC = CreateCompatibleDC(hdc);
	hOldBitmap = SelectObject(game.hCompatibleDC, game.blockBitMap);
	SelectObject(game.hBkDC, game.bkBitMap);
	SetTimer(hWnd, ID_TIMER, 1000 - game.level * 100, (TIMERPROC)TimerProc);

	ReleaseDC(hWnd, hdc);
}

void DrawField(HDC hdc, HDC hCompatibleDC, BITMAP Bitmap, BITMAP bkBitMap,const Game & game)
{
	HANDLE hFieldBitmap;
	BITMAP fieldBitmap;
	RECT rt;
	HDC hHelpDC;

	short int width = pole.Xmax - pole.Xmin, height = pole.Ymax - pole.Ymin + 1,
		realWidth = (width + 1) * game.BlockWidth, realHeight = height * game.BlockHeight;

	hHelpDC = CreateCompatibleDC(hdc);

	hFieldBitmap = CreateCompatibleBitmap(hdc, realWidth, realHeight);
	SelectObject(hHelpDC, hFieldBitmap);
	GetObject(hFieldBitmap, sizeof(BITMAP), &fieldBitmap);

	rt.top = 0; rt.left = 0; rt.right = realWidth; rt.bottom = realHeight;
	
	ShowField(pole.field, height, width, hHelpDC,Bitmap, bkBitMap, game);

	StretchBlt(hdc, 0, 0, realWidth, realHeight, hHelpDC, 0, 0, fieldBitmap.bmWidth,
		fieldBitmap.bmHeight, SRCCOPY);

	DeleteDC(hHelpDC);
}

void GameOver(HWND hWnd)
{
	KillTimer(hWnd, ID_TIMER);
	endGame = true;
}

void Pause(HWND hWnd)
{
	if (endGame)
		return;
	RECT rt;
	if (pause)
	{
		SetTimer(hWnd, ID_TIMER, 1000 - game.level * 100, (TIMERPROC)TimerProc);
		pause = false;
	}
	else
	{
		KillTimer(hWnd, ID_TIMER);
		pause = true;
	}

	GetClientRect(hWnd, &rt);
	InvalidateRect(hWnd, &rt, 1);
}

void UpdateBitmap(Game & game)
{
	HANDLE hBitmap;
	BITMAP Bitmap;
	char fileName[10];

	wsprintf(fileName, "block%d", game.NumberOfBitmap);

	DeleteObject(&game.blockBitMap);
	game.blockBitMap = LoadBitmap(hInst, fileName);;
	SelectObject(game.hCompatibleDC, game.blockBitMap);
}

void UpdateBkBitmap(Game & game)
{
	HANDLE hBitmap;
	BITMAP Bitmap;
	char fileName[10];

	wsprintf(fileName, "bk%d", game.NumberOfBkBitmap);

	DeleteObject(&game.bkBitMap);
	game.bkBitMap = LoadBitmap(hInst, fileName);;
	SelectObject(game.hBkDC, game.bkBitMap);
}

void SaveRecord(const Players * players)
{
	FILE *file;
	char * score = new char[10];
	//fread(pstrBuffer, 1, iLength, file);
	file = fopen("log.txt", "wb");

	fwrite("\r\n", 1, 3, file);
	for (int i = 0; i < 10; i++)
	{
		for (int i = 0; i < 10; i++)
		{
			score[i] = ' ';
		}
		fwrite(" name: ", 1, 6, file);
		fwrite(players[i].name, 1, 10, file);
		fwrite(" score: ",1, 8,file);

		_itoa(players[i].score, score, 10);
		fwrite(score, 1, 10, file);
		fwrite("\r\n", 1, 2, file);
	}
	
	fclose(file);
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine,
	int       nCmdShow)
{
	MSG msg;
	HACCEL hAccel;
	MyRegisterClass(hInstance);
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}


ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = (WNDPROC)WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(NULL, IDI_QUESTION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = ClassName;
	wcex.hIconSm = NULL;

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;
	HMENU hMenu = LoadMenu(hInstance, szAppName);
	hInst = hInstance;
	hWnd = CreateWindow(ClassName, "Program",
		WS_OVERLAPPEDWINDOW,
		((GetSystemMetrics(SM_CXSCREEN) - WindowWidth) /2), 20, WindowWidth, WindowHeight,
		NULL, hMenu, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);
	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HINSTANCE hInstance;
	static HANDLE hBitmap, hOldBitmap;
	static BITMAP fieldBitmap, Bitmap, bkBitMap;
	PAINTSTRUCT ps;
	static HDC hdc, hCompatibleDC, hHelpDC;
	RECT rt;

	switch (message)
	{
	case WM_CREATE:
	{
			startGame(hWnd, hdc, Bitmap, bkBitMap, hOldBitmap, game, nextFigure);
	}
	case WM_PAINT:
	{
		
		GetClientRect(hWnd, &rt);
		hdc = BeginPaint(hWnd, &ps); 

		DrawField(hdc, hCompatibleDC, Bitmap, bkBitMap, game);
		DrawInfo(hWnd, hdc,game,nextFigure);

		if (endGame)
			TextOut(hdc, (rt.right - 120) / 2, rt.bottom / 2, "You Lose", 8);
		else
		if (pause)
			TextOut(hdc, (rt.right - 120) / 2, rt.bottom / 2, "Pause", 5);

		EndPaint(hWnd, &ps);
		break;
	}

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
			case MENU_PAUSE:
				Pause(hWnd);
			break;

			case MENU_BLOCK:
				hInstance = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
				DialogBox(hInstance, MAKEINTRESOURCE(DialogChangeBlockColor), hWnd, ChangeBlockColor);
				break;
			case MENU_BK:
				hInstance = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
				DialogBox(hInstance, MAKEINTRESOURCE(DialogChangeBkColor), hWnd, ChangeBkColor);
				break;

			case MENU_INFO:
				hInstance = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
				DialogBox(hInstance, MAKEINTRESOURCE(DialogChangeInfoColor), hWnd, ChangeInfoColor);
				break;

			case MENU_RECORDS:
				hInstance = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
				DialogBox(hInstance, MAKEINTRESOURCE(DialogRecords), hWnd, Records);
				break;

			case MENU_HELP:
				hInstance = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
				DialogBox(hInstance, MAKEINTRESOURCE(DialogHelp), hWnd, Help);
				break;

			case MENU_FASTER:
				if(game.level < 9)
				game.level++;

				KillTimer(hWnd, ID_TIMER);
				if (!pause)
				SetTimer(hWnd, ID_TIMER, 1000 - game.level * 100, (TIMERPROC)TimerProc);
				GetClientRect(hWnd, &rt);
				InvalidateRect(hWnd, &rt, 1);
				break;

			case MENU_LOWER:
				if (game.level > 0)
					game.level--;

				KillTimer(hWnd, ID_TIMER);
				if(!pause)
				SetTimer(hWnd, ID_TIMER, 1000 - game.level * 100, (TIMERPROC)TimerProc);
				GetClientRect(hWnd, &rt);
				InvalidateRect(hWnd, &rt, 1);
				break;
			case MENU_RESTART:
				ClearField(pole.field, 20);
				NextFigure(figure, pole.Xmin, pole.Ymin);
				NextFigure(nextFigure, pole.Xmin, pole.Ymin);
				InsertToField(pole, figure);

				CheckRecords(players, game.name, game.score);
				SaveRecord(players);

				game.score = 0;
				GetClientRect(hWnd, &rt);
				InvalidateRect(hWnd, &rt, 1);
				break;
		}
		break;

	case WM_TIMER:
		InvalidateRect(hWnd, NULL, FALSE);
		return 0;

	case WM_ERASEBKGND:
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_UP:
			if (!pause && !endGame)
			Rotation(pole, figure);
			break;

		case VK_DOWN:
			if (!pause && !endGame)
				if (!move(pole, figure, Down))
				{
					checkLines(pole, figure.y, game);

					figure = nextFigure;
					NextFigure(nextFigure, pole.Xmin, pole.Ymin);
					if (!InsertToField(pole, figure))
					{
						CheckRecords(players, game.name, game.score);
						SaveRecord(players);
						GameOver(hWnd);
					}
				}
			break;

		case VK_LEFT:
			if (!pause && !endGame)
			move(pole, figure, Left);
			break;

		case VK_RIGHT:
			if (!pause && !endGame)
			move(pole, figure, Right);
			break;

		case VK_SPACE:
			Pause(hWnd);
			break;
		}

		GetClientRect(hWnd, &rt);
		InvalidateRect(hWnd, &rt, 1);
		break;

	case WM_DESTROY:
		CheckRecords(players, game.name, game.score);
		SaveRecord(players);
		DeleteObject(SelectObject(hCompatibleDC, hOldBitmap));
		DeleteDC(hCompatibleDC);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

VOID CALLBACK TimerProc(HWND hWnd, UINT iMsg, UINT iTimerID, DWORD dwTime)
{
	RECT rt;
	if (!move(pole, figure, Down))
	{
		checkLines(pole, figure.y, game);
		figure = nextFigure;

		NextFigure(nextFigure, pole.Xmin, pole.Ymin);
		if (!InsertToField(pole, figure))
		{
			CheckRecords(players, game.name, game.score);
			SaveRecord(players);
			GameOver(hWnd);
		}
	}

	GetClientRect(hWnd, &rt);
	InvalidateRect(hWnd, &rt, 1);
}

BOOL CALLBACK ChangeBlockColor(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	static HWND hWndMain;
	HDC hdc, hCompatibleDC;
	RECT rt;
	PAINTSTRUCT ps;
	HANDLE hBitmap, hOldBitmap;
	BITMAP Bitmap;

	hWndMain = FindWindowEx(NULL, NULL, ClassName, NULL);

	switch (iMsg)
	{

	case WM_INITDIALOG:
		
		CheckRadioButton(hWnd, ID_VIOLET, ID_CROSS, ID_VIOLET + game.NumberOfBitmap - 1);
		return TRUE;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		hCompatibleDC = CreateCompatibleDC(hdc);
		char fileName[10];
		for (int i = 1; i < 7; i++)
		{
			wsprintf(fileName, "block%d", i);

			hBitmap = LoadBitmap(hInst, fileName);
			GetObject(hBitmap, sizeof(BITMAP), &Bitmap);
			hOldBitmap = SelectObject(hCompatibleDC, hBitmap);

			StretchBlt(hdc, 5, 12 + ((i - 1) * 60), 30, 30, hCompatibleDC, 0, 0, Bitmap.bmWidth,
				Bitmap.bmHeight, SRCCOPY);

			DeleteObject(SelectObject(hCompatibleDC, hOldBitmap));
		}
		DeleteDC(hCompatibleDC);
		
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
			case ID_VIOLET	:
			case ID_WHITE	:
			case ID_GREEN	:
			case ID_GRAY	:
			case ID_BLACK	:
			case ID_CROSS	:
				CheckRadioButton(hWnd, ID_VIOLET, ID_CROSS, LOWORD(wParam));
				game.NumberOfBitmap = LOWORD(wParam) - 109;
				
				UpdateBitmap(game);
				GetClientRect(hWndMain, &rt);
				InvalidateRect(hWndMain, &rt, 1);
				UpdateWindow(hWndMain);
				break;
			case IDCANCEL:
				EndDialog(hWnd, 0);
				break;
		}
		break;
	}
	return FALSE;
}

BOOL CALLBACK ChangeBkColor(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	static HWND hWndMain;
	HDC hdc, hCompatibleDC;
	RECT rt;
	PAINTSTRUCT ps;
	HANDLE hBitmap, hOldBitmap;
	BITMAP Bitmap;

	hWndMain = FindWindowEx(NULL, NULL, ClassName, NULL);

	switch (iMsg)
	{

	case WM_INITDIALOG:
		CheckRadioButton(hWnd, ID_BK_WHITE, ID_BK_YELLOW, ID_BK_WHITE + game.NumberOfBkBitmap - 1);
		return TRUE;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		hCompatibleDC = CreateCompatibleDC(hdc);
		char fileName[10];
		for (int i = 1; i < 7; i++)
		{
			wsprintf(fileName, "bk%d", i);

			hBitmap = LoadBitmap(hInst, fileName);
			GetObject(hBitmap, sizeof(BITMAP), &Bitmap);
			hOldBitmap = SelectObject(hCompatibleDC, hBitmap);

			StretchBlt(hdc, 5, 12 + ((i - 1) * 60), 30, 30, hCompatibleDC, 0, 0, 52,
				52, SRCCOPY);

			DeleteObject(SelectObject(hCompatibleDC, hOldBitmap));
		}
		DeleteDC(hCompatibleDC);

		break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_BK_WHITE:
		case ID_BK_VIOLET:
		case ID_BK_GREEN:
		case ID_BK_GRAY:
		case ID_BK_BLUE:
		case ID_BK_YELLOW:
			CheckRadioButton(hWnd, ID_BK_WHITE, ID_BK_YELLOW, LOWORD(wParam));
			game.NumberOfBkBitmap = LOWORD(wParam) - 115;

			UpdateBkBitmap(game);
			GetClientRect(hWndMain, &rt);
			InvalidateRect(hWndMain, &rt, 1);
			UpdateWindow(hWndMain);
			break;
		case IDCANCEL:
			EndDialog(hWnd, 0);
			break;
		}
		break;
	}
	return FALSE;
}

BOOL CALLBACK ChangeInfoColor(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	static HWND hWndMain;
	HDC hdc, hCompatibleDC;
	RECT rt;
	PAINTSTRUCT ps;

	hWndMain = FindWindowEx(NULL, NULL, ClassName, NULL);

	switch (iMsg)
	{

	case WM_INITDIALOG:
		CheckRadioButton(hWnd, ID_INFO_VIOLET, ID_INFO_YELLOW, game.NumberOfInfoColor + ID_INFO_WHITE -1);
		return TRUE;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		hCompatibleDC = CreateCompatibleDC(hdc);
		char fileName[10];
		for (int i = 1; i < 7; i++)
		{
			

		}
		DeleteDC(hCompatibleDC);

		break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_INFO_BLUE:
			game.colorInfo = RGB(0, 191, 255);
			game.NumberOfInfoColor = 1;
			break;
		case ID_INFO_WHITE:
			game.colorInfo = RGB(255, 255, 255);
			game.NumberOfInfoColor = 2;
			break;
		case ID_INFO_GREEN:
			game.colorInfo = RGB(0, 255, 0);
			game.NumberOfInfoColor = 3;
			break;
		case ID_INFO_GRAY:
			game.colorInfo = RGB(211, 211, 211);
			game.NumberOfInfoColor = 4;
			break;
		case ID_INFO_VIOLET:
			game.colorInfo = RGB(218, 112, 214);
			game.NumberOfInfoColor = 5;
			break;
		case ID_INFO_YELLOW:
			game.colorInfo = RGB(240, 230, 140);
			game.NumberOfInfoColor = 6;
			break;
		case IDCANCEL:
			EndDialog(hWnd, 0);
			break;
		}

		if (LOWORD(wParam) >= ID_INFO_BLUE || LOWORD(wParam) <= ID_INFO_YELLOW)
		{
			CheckRadioButton(hWnd, ID_INFO_BLUE, ID_INFO_YELLOW, LOWORD(wParam));
			GetClientRect(hWndMain, &rt);
			InvalidateRect(hWndMain, &rt, 1);
			UpdateWindow(hWndMain);
		}
		break;
	}
	return FALSE;
}

BOOL CALLBACK Start(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{

	case WM_INITDIALOG:
		game.level = 0;
		CheckRadioButton(hWnd, ID_LV0, ID_LV9, ID_LV0);
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{

		case IDCANCEL:
		case IDOK:
		{
			HWND hEdit1 = GetDlgItem(hWnd, IDM_TEXT);
			TCHAR text[11] = { 0 };
			SendMessage(hEdit1, WM_GETTEXT, (WPARAM)11, (LPARAM)text);
			text[10] = '\0';
			game.name = new char(11);
			for (int i = 0; i < 10; i++)
				game.name[i] = ' ';
			game.name[10] = '\0';
			memcpy(game.name, text, sizeof(TCHAR) * strlen(text));
			EndDialog(hWnd, 0);
		}
			break;
		case ID_LV0:
			game.level = 0;
			break;
		case ID_LV1:
			game.level = 1;
			break;
		case ID_LV2:
			game.level = 2;
			break;
		case ID_LV3:
			game.level = 3;
			break;
		case ID_LV4:
			game.level = 4;
			break;
		case ID_LV5:
			game.level = 5;
			break;
		case ID_LV6:
			game.level = 6;
			break;
		case ID_LV7:
			game.level = 7;
			break;
		case ID_LV8:
			game.level = 8;
			break;
		case ID_LV9:
			game.level = 9;
			break;
		}

		if(LOWORD(wParam) >= ID_LV0|| LOWORD(wParam) <= ID_LV9)
			CheckRadioButton(hWnd, ID_LV0, ID_LV9, LOWORD(wParam));

		
		break;
	}
	return FALSE;
}

BOOL CALLBACK Records(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;

	switch (iMsg)
	{

	case WM_INITDIALOG:
		return TRUE;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		char Line[30];
			for(int i = 0; i < 10; i++)
			{ 
				TextOut(hdc, 10, 40 + ((i - 1) * 20), "Name: ", 6);
				TextOut(hdc, 65, 40 + ((i - 1) * 20), players[i].name, 10);
				wsprintf(Line, "score : %d", players[i].score);

				TextOut(hdc, 155, 40 + ((i - 1) * 20), Line, strlen(Line));
			}

		EndPaint(hWnd,&ps);
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{

		case IDCANCEL:
		case IDOK:
			EndDialog(hWnd, 0);
			break;
		}
		break;
	}
	return FALSE;
}

BOOL CALLBACK Help(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{

	case WM_INITDIALOG:
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{

		case IDCANCEL:
		case IDOK:
			EndDialog(hWnd, 0);
			break;
		}
		break;
	}
	return FALSE;
}