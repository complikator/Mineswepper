
#include "BoardDrawer.h"

BoardDrawer::BoardDrawer(HWND window, int panelHeight, int fieldSize, WCHAR* fieldClassName, HINSTANCE hInst) : mainWindow(window), panelHeight(panelHeight), fieldSize(fieldSize), fieldClassName(fieldClassName), hInst(hInst) {}

void BoardDrawer::AdjustWindow(int fieldsX, int fieldsY) {
	this->fieldsX = fieldsX;
	this->fieldsY = fieldsY;

	int screenX = GetSystemMetrics(SM_CXSCREEN);
	int screenY = GetSystemMetrics(SM_CYSCREEN);

	int clientWidth = fieldsX * fieldSize + (fieldsX - 1);
	int clientHeight = fieldsY * fieldSize + (fieldsY - 1) + panelHeight;

	RECT rc;

	rc.left = 0;
	rc.top = 0;
	rc.right = clientWidth;
	rc.bottom = clientHeight;

	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, TRUE);

	int windowWidth = rc.right - rc.left;
	int windowHeight = rc.bottom - rc.top;

	int centerX = (screenX - windowWidth) / 2;
	int centerY = (screenY - windowHeight) / 2;

	MoveWindow(mainWindow, centerX, centerY, windowWidth, windowHeight, TRUE);
}

void BoardDrawer::DebugModeOn() {
	isDebugMode = true;
}

void BoardDrawer::DebugModeOff() {
	isDebugMode = false;
}

void BoardDrawer::CreateView(Board* board) {
	clearBoard();
	childWindows.resize(fieldsX * fieldsY);

	for (int i = 0; i < fieldsY; i++) {
		for (int j = 0; j < fieldsX; j++) {
			auto x = j * fieldSize + j;
			auto y = i * fieldSize + i + panelHeight;
			childWindows[i * fieldsX + j] = CreateWindow(fieldClassName, L"", WS_CHILD | WS_VISIBLE, x, y, fieldSize, fieldSize, mainWindow, nullptr, hInst, nullptr);
		}
	}
}

void BoardDrawer::clearBoard() {
	for (auto& h : childWindows) {
		DestroyWindow(h);
	}
}

void BoardDrawer::Hwnd2Pos(HWND hWnd, int* x, int* y) {
	for (int i = 0; i < fieldsX * fieldsY; i++) {
		if (childWindows[i] == hWnd) {
			*y = i / fieldsX;
			*x = i % fieldsX;
		}
	}
}

void BoardDrawer::UpdateBoard(Board* board, bool forceUpdate) {
	for (int i = 0; i < fieldsX; i++) {
		for (int j = 0; j < fieldsY; j++) {
			if (board->IsFieldDirty(i, j) || forceUpdate) {
				redrawField(i, j, board);
				board->CleanUpField(i, j);
			}
		}
	}
}

void BoardDrawer::colorField(HWND hwnd, COLORREF color) {
	HDC hdc = GetDC(hwnd);

	HBRUSH brush = CreateSolidBrush(color);
	HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);

	RECT rc;
	GetClientRect(hwnd, &rc);

	FillRect(hdc, &rc, brush);

	SelectObject(hdc, oldBrush);

	DeleteObject(oldBrush);

	ReleaseDC(hwnd, hdc);
}

void BoardDrawer::drawCircle(HWND hwnd) {
	HDC hdc = GetDC(hwnd);

	HBRUSH brush = CreateSolidBrush(BOMB_COLOR);
	HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);

	Ellipse(hdc, 2, 2, 22, 22);

	SelectObject(hdc, oldBrush);

	DeleteObject(oldBrush);

	ReleaseDC(hwnd, hdc);
}

void BoardDrawer::drawText(HWND hwnd, TCHAR* text, COLORREF color) {
	HDC hdc = GetDC(hwnd);

	SetBkMode(hdc, TRANSPARENT);
	SetTextColor(hdc, color);
	SetTextColor(hdc, color);

	HFONT font = CreateFont(-14, 0, 0, 0, FW_LIGHT, false, false, 0, EASTEUROPE_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, TEXT("Verdana"));

	HFONT oldFont = (HFONT)SelectObject(hdc, font);
	RECT rc;
	GetClientRect(hwnd, &rc);
	DrawText(hdc, text, (int)_tcslen(text), &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	SelectObject(hdc, oldFont);
	DeleteObject(font);

	ReleaseDC(hwnd, hdc);
}

void BoardDrawer::redrawField(int x, int y, Board* board) {
	auto type = board->GetFieldType(x, y);
	auto state = board->GetFieldState(x, y);

	auto hwnd = Pos2Hwnd(x, y);

	switch (type) {
	case FieldType::Empty: {
		switch (state) {
		case FieldState::Active: {

			colorField(hwnd, EMPTY_ACTIVE_COLOR);

			int neigh;
			if ((neigh = board->GetNeighborsValues(x, y)) > 0) {
				TCHAR buf[2];

				_stprintf_s(buf, 2, TEXT("%d"), neigh);

				drawText(hwnd, buf, colorMapping[neigh]);
			}

		}break;
		case FieldState::Empty: {
			colorField(hwnd, DEFAULT_COLOR);

			if (isDebugMode) {
				int neigh;
				if ((neigh = board->GetNeighborsValues(x, y)) > 0) {
					TCHAR buf[2];

					_stprintf_s(buf, 2, TEXT("%d"), neigh);

					drawText(hwnd, buf, colorMapping[neigh]);
				}
			}
		}break;
		case FieldState::Marked: {
			drawBitmap(hwnd);
		}
		}
	}break;
	case FieldType::Bomb: {
		switch (state) {
		case FieldState::Active: {
			colorField(hwnd, EMPTY_ACTIVE_COLOR);
			drawCircle(hwnd);

		}break;
		case FieldState::Empty: {
			colorField(hwnd, DEFAULT_COLOR);

			if (isDebugMode) {
				drawCircle(hwnd);
			}
		}break;
		case FieldState::Marked: {
			drawBitmap(hwnd);
		}
		}
	}break;
	}

	UpdateWindow(hwnd);
}

HWND BoardDrawer::Pos2Hwnd(int x, int y) {
	return childWindows[y * fieldsX + x];
}

void BoardDrawer::drawBitmap(HWND hwnd) {
	HDC hdc = GetDC(hwnd);

	HBITMAP bitmap = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP1));
	HDC memDc = CreateCompatibleDC(hdc);
	HBITMAP oldBitmap = (HBITMAP)SelectObject(memDc, bitmap);
	BitBlt(hdc, 0, 0, 48, 48, memDc, 0, 0, SRCCOPY);

	RECT rc;
	GetClientRect(hwnd, &rc);

	StretchBlt(hdc, 0, 0, rc.right - rc.left, rc.bottom - rc.top, memDc,
		0, 0, 48, 48, SRCCOPY);
	SelectObject(memDc, oldBitmap);
	DeleteObject(bitmap);
	DeleteDC(memDc);

	ReleaseDC(hwnd, hdc);
}

void BoardDrawer::SwitchDebugMode() {
	isDebugMode = !isDebugMode;
}

bool BoardDrawer::IsDebugMode() {
	return isDebugMode;
}