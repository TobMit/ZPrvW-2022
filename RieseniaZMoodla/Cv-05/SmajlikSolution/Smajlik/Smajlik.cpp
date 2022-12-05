// Zakladna aplikacia

#include <windows.h>
#include <windowsx.h>
#include "resource.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

char szWinName[] = "MojeOkno"; 		// Nazov oknovej triedy
void KresliCiaru(HDC hdc, int x, int y, int sirka, int vyska);

HBRUSH zltyStetec = 0;
HBITMAP zltySmajlik = 0;
HBRUSH cervenyStetec = 0;
HBITMAP cervenySmajlik = 0;

int APIENTRY WinMain(HINSTANCE hThisInst, HINSTANCE hPrevInst, PSTR lpszArgs, int nWinMode)
{
	HWND        	hwnd;
	MSG         	msg;
	WNDCLASSEX    	wndclass;
	zltySmajlik = LoadBitmap(hThisInst, MAKEINTRESOURCE(IDB_BITMAP1));
	if (zltySmajlik)
		zltyStetec = CreatePatternBrush(zltySmajlik);
	cervenySmajlik = LoadBitmap(hThisInst, MAKEINTRESOURCE(IDB_BITMAP2));
	if (cervenySmajlik)
		cervenyStetec = CreatePatternBrush(cervenySmajlik);
	//LoadBitmap(hThisInst, "IDB_BITMAP1");

	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW;          	// Implicitny styl
	wndclass.lpfnWndProc = WndProc; 	// Oknova funkcia
	wndclass.hInstance = hThisInst;  	// Popisovac tejto instancie
	wndclass.lpszClassName = szWinName;  	// Nazov oknovej triedy

	wndclass.hIcon = LoadIcon(hThisInst, MAKEINTRESOURCE(IDI_ICON1)); // Druh ikony
	wndclass.hIconSm = LoadIcon(hThisInst, MAKEINTRESOURCE(IDI_ICON1));		// Druh malej ikony

	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);     // Druh kurzora
	wndclass.lpszMenuName = NULL;                            // Ziadne menu

	wndclass.cbClsExtra = 0;          	// nie su potrebne
	wndclass.cbWndExtra = 0;          	// ziadne extra informacie

	wndclass.hbrBackground = zltyStetec ? zltyStetec : (HBRUSH)(COLOR_WINDOW + 1);
	RegisterClassEx(&wndclass);            // Registracia okna
	// Po registracii vytvorenie okna
	hwnd = CreateWindowEx(
		WS_EX_ACCEPTFILES,
		szWinName,           // Nazov oknovej triedy
		"Windows - zakladny program",  // Titulok okna
		WS_OVERLAPPEDWINDOW, // Styl okna
		CW_USEDEFAULT,       // Pociatocna x-poloha
		CW_USEDEFAULT,       // Pociatocna y-poloha
		CW_USEDEFAULT,       // Pociatocna sirka
		CW_USEDEFAULT,       // Pociatocna vyska
		HWND_DESKTOP,        // NULL, popisovac rodicovskeho okna (ziadne)
		NULL,                // popisovac menu okna (ziadne)
		hThisInst,           // popisovac instancie programu
		NULL);		           // parametre tvorby

	ShowWindow(hwnd, nWinMode);        // Zobrazenie okna
	UpdateWindow(hwnd);

	// Slucka sprav
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	if (cervenyStetec)
		DeleteObject(cervenyStetec);
	if (cervenySmajlik)
		DeleteObject(cervenySmajlik);
	if (zltyStetec)
		DeleteObject(zltyStetec);
	if (zltySmajlik)
		DeleteObject(zltySmajlik);
	return msg.wParam;
}

// Tuto funkciu vola Windows - prenasa do nej spravy z fronty sprav
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	RECT	  rect;
	static int x = 0, y = 0;
	switch (message) {
	case WM_CREATE:
		//x = y = 0;
		break;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		//GetClientRect(hwnd, &rect);
		//MoveToEx(hdc, x, 0, nullptr);
		//LineTo(hdc, x, rect.bottom);
		//MoveToEx(hdc, 0, y, nullptr);
		//LineTo(hdc, rect.right, y);
		EndPaint(hwnd, &ps);
		break;
	case WM_MOUSEMOVE:
		hdc = GetDC(hwnd);
		GetClientRect(hwnd, &rect);
		KresliCiaru(hdc, x, y, rect.right, rect.bottom);
		x = GET_X_LPARAM(lParam);
		y = GET_Y_LPARAM(lParam);
		KresliCiaru(hdc, x, y, rect.right, rect.bottom);
		ReleaseDC(hwnd, hdc);
		break;
	case WM_LBUTTONDOWN:
		SetRect(&rect, x / 20 * 20, y / 20 * 20, x / 20 * 20 + 20, y / 20 * 20 + 20);
		hdc = GetDC(hwnd);
		KresliCiaru(hdc, x, y, rect.right, rect.bottom);
		InvertRect(hdc, &rect);
		//if (GetPixel(hdc, x / 20 * 20, y / 20 * 20) == RGB(255, 0, 0))
		//	FillRect(hdc, &rect, zltyStetec);
		//else
		//	FillRect(hdc, &rect, cervenyStetec);
		KresliCiaru(hdc, x, y, rect.right, rect.bottom);
		ReleaseDC(hwnd, hdc);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}

void KresliCiaru(HDC hdc, int x, int y, int sirka, int vyska)
{
	int staryRezim = SetROP2(hdc, R2_XORPEN);
	HPEN pero = CreatePen(PS_SOLID, 1, RGB(250, 250, 250));
	HPEN starePero = (HPEN)SelectObject(hdc, pero);
	MoveToEx(hdc, x, 0, nullptr);
	LineTo(hdc, x, vyska);
	MoveToEx(hdc, 0, y, nullptr);
	LineTo(hdc, sirka, y);
	SelectObject(hdc, starePero);
	DeleteObject(pero);
	SetROP2(hdc, staryRezim);
}