#pragma once

#include "Board.h"
#include "framework.h"
#include "resource.h"

#define DEFAULT_COLOR 0x777777
#define EMPTY_ACTIVE_COLOR 0xc8c8c8
#define BOMB_COLOR RGB(0, 0, 0)

class BoardDrawer {
private:
	HWND mainWindow;
	int panelHeight;
	int fieldSize;
	int fieldsX;
	int fieldsY;

	bool isDebugMode = false;

	std::vector<HWND> childWindows;
	WCHAR* fieldClassName;
	HINSTANCE hInst;

	void clearBoard();

	void redrawField(int x, int y, Board*);

	void colorField(HWND hwnd, COLORREF color);
	void drawText(HWND hwnd, TCHAR* text, COLORREF color);
	void drawBitmap(HWND hwnd);
	void drawCircle(HWND hwnd);

	COLORREF colorMapping[9] = {0, 0x00ff0000, 0x0000b300, 0x000000ff, 0x00fff000, 0x00ff00ff, 0x00abcdef, 0x00aaaaaa, 0xbbbbbb };
public:
	BoardDrawer(HWND, int, int, WCHAR*, HINSTANCE);
	void AdjustWindow(int, int);

	void DebugModeOn();

	void DebugModeOff();

	void SwitchDebugMode();

	bool IsDebugMode();

	void CreateView(Board*);

	void Hwnd2Pos(HWND, int*, int*);

	void UpdateBoard(Board*, bool forceUpdate = false);

	HWND Pos2Hwnd(int x, int y);
};