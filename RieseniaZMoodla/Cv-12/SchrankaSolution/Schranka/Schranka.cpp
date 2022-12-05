// Zakladna aplikacia

#include <windows.h>
#include <windowsx.h>
#include <cstring>
#include "resource.h"

const int CAS = 5;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void KresliBMP(HDC hdc, int x, int y, int sirka, int vyska, HBITMAP hBitmap);
char szWinName[] = "MojeOkno"; 		// Nazov oknovej triedy

int APIENTRY WinMain(HINSTANCE hThisInst, HINSTANCE hPrevInst, PSTR lpszArgs, int nWinMode)
{
	HWND        	hwnd;
	MSG         	msg;
	WNDCLASSEX    	wndclass;

	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = 0;          	// Implicitny styl
	wndclass.lpfnWndProc = WndProc; 	// Oknova funkcia
	wndclass.hInstance = hThisInst;  	// Popisovac tejto instancie
	wndclass.lpszClassName = szWinName;  	// Nazov oknovej triedy

	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION); // Druh ikony
	wndclass.hIconSm = 0; 		// Druh malej ikony

	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);     // Druh kurzora
	wndclass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);                            // Ziadne menu

	wndclass.cbClsExtra = 0;          	// nie su potrebne
	wndclass.cbWndExtra = 0;          	// ziadne extra informacie

	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);	// Biele pozadie alebo (HBRUSH)(COLOR_WINDOW+1)

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
	HACCEL hacell = LoadAccelerators(hThisInst, MAKEINTRESOURCE(IDR_ACCELERATOR1));
	// Slucka sprav
	while (GetMessage(&msg, NULL, 0, 0)) {
		if (hacell && !TranslateAccelerator(hwnd, hacell, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return msg.wParam;
}

// Tuto funkciu vola Windows - prenasa do nej spravy z fronty sprav
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	RECT	  rect;
	static bool mamText, mamBMP;
	static char* mojeData;
	static int casUplynuty;
	switch (message) {
	case WM_CREATE:
		mamText = false;
		mamBMP = false;
		mojeData = nullptr;
		casUplynuty = CAS;
		SetTimer(hwnd, 1, 1000, nullptr);
		break;
	case WM_TIMER:
		if (casUplynuty > 0)
		{
			casUplynuty--;
			if (casUplynuty == 0)
			{
				if (MessageBox(hwnd, "Skonèi?", "Dotaz", MB_YESNO | MB_ICONQUESTION) == IDYES)
					PostMessage(hwnd, WM_CLOSE, 0, 0);
				else
					casUplynuty = CAS;
			}
		}
		break;
	case WM_KEYDOWN:
		casUplynuty = CAS;
		break;
	case WM_INITMENUPOPUP:
		if (LOWORD(lParam) == 1)
		{
			HMENU hmenu = GetMenu(hwnd);
			mamText = IsClipboardFormatAvailable(CF_TEXT);
			mamBMP = IsClipboardFormatAvailable(CF_BITMAP);
			EnableMenuItem(hmenu, ID_EDIT_VLOZ, mamText || mamBMP ? MF_ENABLED : MF_DISABLED);
		}
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_PROGRAM_KONIEC:
			PostMessage(hwnd, WM_CLOSE, 0, 0);
			break;
		case ID_EDIT_VLOZ:
			OpenClipboard(hwnd);
			if (mamText)
			{
				HANDLE hclip = GetClipboardData(CF_TEXT);
				if (hclip)
				{
					delete[] mojeData;
					mojeData = nullptr;
					int pocet = GlobalSize(hclip);
					if (pocet > 0)
					{
						mojeData = new char[pocet + 1];
						char* pdata = (char*)GlobalLock(hclip);
						if (pdata)
						{
							lstrcpy(mojeData, pdata);
							InvalidateRect(hwnd, nullptr, true);
						}
						GlobalUnlock(hclip);
					}
				}
			}
			if (mamBMP)
			{
				HANDLE hclip = GetClipboardData(CF_BITMAP);
				if (hclip)
				{
					HBITMAP hBitmap = (HBITMAP)GetClipboardData(CF_BITMAP);
					if (hBitmap)
					{
						HDC hdc = GetDC(hwnd);
						GetClientRect(hwnd, &rect);
						KresliBMP(hdc, 0, 0, rect.right, rect.bottom, hBitmap);
						ReleaseDC(hwnd, hdc);
					}
					GlobalUnlock(hclip);
				}
			}
			CloseClipboard();
			break;
		case ID_EDIT_KOPIRUJ:
			if (mojeData)
			{
				HANDLE hclip = GlobalAlloc(GHND | GMEM_SHARE, lstrlen(mojeData) + 1 * sizeof(char));
				if (hclip)
				{
					char* pdata = (char*)GlobalLock(hclip);
					lstrcpy(pdata, mojeData);
					strrev(pdata);
					GlobalUnlock(hclip);
					OpenClipboard(hwnd);
					SetClipboardData(CF_TEXT, hclip);
					CloseClipboard();
				}
			}
			break;
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		GetClientRect(hwnd, &rect);
		if (mojeData)
			DrawText(hdc, mojeData, -1, &rect, DT_LEFT | DT_EXPANDTABS);
		EndPaint(hwnd, &ps);
		break;

	case WM_DESTROY:
		KillTimer(hwnd, 1);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}

void KresliBMP(HDC hdc, int x, int y, int sirka, int vyska, HBITMAP hBitmap)
{
	HDC hMemDC;
	BITMAP bm;
	POINT px, py;
	hMemDC = CreateCompatibleDC(hdc);
	SelectObject(hMemDC, hBitmap);
	SetMapMode(hMemDC, GetMapMode(hdc));
	GetObject(hBitmap, sizeof(BITMAP), &bm);
	px.x = bm.bmWidth;
	px.y = bm.bmHeight;
	DPtoLP(hMemDC, &px, 1);
	py.x = py.y = 0;
	DPtoLP(hMemDC, &py, 1);
	//BitBlt(hdc, x, y, px.x, px.y, hMemDC, py.x, py.y, SRCCOPY);
	StretchBlt(hdc, x, y, sirka, vyska, hMemDC, py.x, py.y, px.x, px.y, SRCCOPY);
	DeleteDC(hMemDC);
}