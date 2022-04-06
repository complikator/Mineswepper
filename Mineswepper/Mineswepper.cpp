// Mineswepper.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "Mineswepper.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
WCHAR fieldWindowClass[MAX_LOADSTRING];            // the field window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
ATOM                RegisterFieldClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    FieldProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    CustomSize(HWND, UINT, WPARAM, LPARAM);

Board* board;
BoardDrawer* drawer;
Referee* ref;

bool timerStarted = false;
int timerId = 10;

double time = 0;

void checkGameState(HWND hwnd);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.

	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_MINESWEPPER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	LoadStringW(hInstance, IDC_FIELD, fieldWindowClass, MAX_LOADSTRING);
	RegisterFieldClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MINESWEPPER));

	MSG msg;

	// Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MINESWEPPER));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_MINESWEPPER);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

ATOM RegisterFieldClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = FieldProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MINESWEPPER));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(CreateSolidBrush(0x777777));
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_MINESWEPPER);
	wcex.lpszClassName = fieldWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

void drawTime(HWND hwnd) {

	HDC hdc = GetDC(hwnd);
	HFONT font = CreateFont(25, 0, 0, 0, FW_BOLD, false, FALSE, 0, EASTEUROPE_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Arial"));
	SetTextColor(hdc, RGB(255, 0, 0));
	HFONT oldfont = (HFONT)SelectObject(hdc, font);


	TCHAR s[10];
	_stprintf_s(s, 10, TEXT("%06.1f"), time++ / 10);

	RECT rc;
	GetClientRect(hwnd, &rc);
	rc.right = rc.right / 2;
	rc.bottom = 30;
	DrawText(hdc, s, (int)_tcslen(s), &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	ReleaseDC(hwnd, hdc);
	SelectObject(hdc, oldfont);
	DeleteObject(font);
	SetTextColor(hdc, RGB(0, 0, 0));

	UpdateWindow(hwnd);
}

void drawBombCounter(HWND hwnd) {
	HDC hdc = GetDC(hwnd);
	HFONT font = CreateFont(25, 0, 0, 0, FW_BOLD, false, FALSE, 0, EASTEUROPE_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Arial"));
	SetTextColor(hdc, RGB(255, 0, 0));
	HFONT oldfont = (HFONT)SelectObject(hdc, font);


	TCHAR s[10];
	_stprintf_s(s, 10, TEXT("%04d"), ref->GetMarksLeft());

	RECT rc;
	GetClientRect(hwnd, &rc);
	rc.left = rc.left + (rc.right - rc.left) / 2;
	rc.bottom = 30;
	DrawText(hdc, s, (int)_tcslen(s), &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	ReleaseDC(hwnd, hdc);
	SelectObject(hdc, oldfont);
	DeleteObject(font);
	SetTextColor(hdc, RGB(0, 0, 0));

	UpdateWindow(hwnd);
}

void onTimer(HWND hwnd, UINT u1, UINT_PTR u2, DWORD u3) {

	if (ref->GameFinished()) {
		checkGameState(hwnd);
		return;
	}

	drawTime(hwnd);
	drawBombCounter(hwnd);

	ref->TimerChanged(time);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Store instance handle in our global variable

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	drawer->UpdateBoard(board, true);
	onTimer(hWnd, 0, 0, 0);

	return TRUE;
}

void initBoard(int x, int y, int bombs) {
	RandomInitializer init(x * y, bombs);
	ref = new Referee(bombs, x * y, defaultTime);

	board = new Board(x, y, &init, ref);


	drawer->AdjustWindow(x, y);
	drawer->CreateView(board);
	drawer->UpdateBoard(board, true);

	time = 0;
	timerStarted = false;
}




LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE: {
		drawer = new BoardDrawer(hWnd, panelHeight, fieldSize, fieldWindowClass, hInst);

		initBoard(defaultFieldsX, defaultFieldsY, defaultBombCount);

	}
				  break;

	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case ID_HELP_DEBUG32772: {
			drawer->SwitchDebugMode();

			CheckMenuItem(GetMenu(hWnd), ID_HELP_DEBUG32772, drawer->IsDebugMode() ? MF_CHECKED : MF_UNCHECKED);

			drawer->UpdateBoard(board, true);
		}break;
		case ID_FILE_NEW: {
			initBoard(board->GetSizeX(), board->GetSizeY(), board->GetBombsCount());
			drawer->UpdateBoard(board, true);

			onTimer(hWnd, 0, 0, 0);
			KillTimer(hWnd, timerId);
		}break;
		case ID_FILE_CUSTOMSIZE: {
			DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, CustomSize);

		}break;
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code that uses hdc here...
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void checkGameState(HWND hwnd) {
	if (ref->GameFinished())
	{
		KillTimer(hwnd, timerId);
		if (ref->IsWin()) {
			MessageBox(hwnd, L"Win", L"Mineswepper", 0);
		}
		else {
			MessageBox(hwnd, L"Loose", L"Mine ", MB_ICONERROR);
		}
	}
}

void validateDimensions(int* height, int* width, int* bombs) {
	if (*height > maxHeight) {
		*height = maxHeight;
	}
	if (*height < minHeight) {
		*height = minHeight;
	}
	if (*width > maxWidth) {
		*width = maxWidth;
	}
	if (*width < minWidth) {
		*width = minWidth;
	}

	if (*bombs < minBombs) {
		*bombs = minBombs;
	}

	if (*bombs > (*height) * (*width)) {
		*bombs = minBombs;
	}
}

LRESULT CALLBACK FieldProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_LBUTTONUP: {
		if (!timerStarted) {
			SetTimer(GetParent(hWnd), timerId, 100, (TIMERPROC)onTimer);
			timerStarted = true;
		}

		if (ref->GameFinished())
			break;
		int x;
		int y;

		drawer->Hwnd2Pos(hWnd, &x, &y);

		if (board->GetFieldState(x, y) != FieldState::Empty)
			break;

		board->Activate(x, y);
		drawer->UpdateBoard(board);

		checkGameState(GetParent(hWnd));
	}break;
	case WM_RBUTTONDOWN: {
		if (!timerStarted) {
			SetTimer(GetParent(hWnd), timerId, 100, nullptr);
			timerStarted = true;
		}

		if (ref->GameFinished() || ref->GetMarksLeft() == 0)
			break;
		int x;
		int y;

		drawer->Hwnd2Pos(hWnd, &x, &y);

		if (board->GetFieldState(x, y) == FieldState::Marked)
			board->Unmark(x, y);
		else if (board->GetFieldState(x, y) == FieldState::Empty)
			board->Mark(x, y);
		drawer->UpdateBoard(board);

		checkGameState(GetParent(hWnd));
	}break;
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
// Message handler for about box.
INT_PTR CALLBACK CustomSize(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK) {
			BOOL ret[3];
			int height = GetDlgItemInt(hDlg, IDC_EDIT1, &ret[0], FALSE);

			int width = GetDlgItemInt(hDlg, IDC_EDIT2, &ret[1], FALSE);

			int bombs = GetDlgItemInt(hDlg, IDC_EDIT3, &ret[2], FALSE);

			if (ret[0] && ret[1] && ret[2]) {

				validateDimensions(&height, &width, &bombs);

				initBoard(width, height, bombs);
				onTimer(GetParent(hDlg), 0, 0, 0);
				KillTimer(GetParent(hDlg), timerId);
			}
		}

		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}