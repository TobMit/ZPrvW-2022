// Zakladna aplikacia

#include <windows.h>
#include <windowsx.h>
#include "resource.h"
#include <time.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

char szWinName[] = "MojeOkno"; 		// Nazov oknovej triedy

void nakresliStvorce(HDC hdc, int x, int y, int sirka, int vyska, RECT &rect);

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

	HACCEL hAkceleratory = LoadAccelerators(hThisInst, MAKEINTRESOURCE(IDR_ACCELERATOR1));
	// Slucka sprav
	while (GetMessage(&msg, NULL, 0, 0)) {
		if (hAkceleratory && !TranslateAcceleratorA(hwnd, hAkceleratory, &msg)) {
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
	static HMENU hmenu;
	static int sirkaKP, vyskaKP;

	switch (message) {
	case WM_CREATE:
		srand(time(NULL));
		hmenu = GetMenu(hwnd);
		sirkaKP = 0;
		vyskaKP = 0;
		break;
	case WM_SIZE:
		sirkaKP = LOWORD(lParam);
		vyskaKP = HIWORD(lParam);
		InvalidateRect(hwnd, nullptr, true);
		break;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		{
			int staryRezim = SetROP2(hdc, R2_XORPEN);
			HPEN pero = CreatePen(PS_SOLID, 1, RGB(250, 250, 250));
			HPEN starePero = (HPEN)SelectObject(hdc, pero);
			for (int x = 0; x < sirkaKP;)
			{
				MoveToEx(hdc, x, 0, nullptr);
				LineTo(hdc, x, vyskaKP);
				x += sirkaKP / 30;
			}
			for (int y = 0; y < vyskaKP;)
			{
				MoveToEx(hdc, 0, y, nullptr);
				LineTo(hdc, sirkaKP, y);
				y += vyskaKP / 30;
			}
			SelectObject(hdc, starePero);
			DeleteObject(pero);
			SetROP2(hdc, staryRezim);
		}
		EndPaint(hwnd, &ps);
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_PROGRAM_START:
			EnableMenuItem(hmenu, ID_PROGRAM_START, MF_DISABLED);
			EnableMenuItem(hmenu, ID_PROGRAM_STOP, MF_ENABLED);
			SetTimer(hwnd, 1, 250, nullptr);
			break;
		case ID_PROGRAM_STOP:
			EnableMenuItem(hmenu, ID_PROGRAM_START, MF_ENABLED);
			EnableMenuItem(hmenu, ID_PROGRAM_STOP, MF_DISABLED);
			KillTimer(hwnd, 1);
			break;
		case ID_PROGRAM_KONIEC:
			PostMessage(hwnd, WM_CLOSE, 0, 0);
			break;
		default:
			break;
		}
		break;
	case WM_TIMER:
		{
			hdc = GetDC(hwnd);
			HBRUSH farebnyBrush = CreateSolidBrush(RGB(rand() % 256, rand() % 256, rand() % 256));
			HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, farebnyBrush);
			int x = rand() % (sirkaKP /30);
			int y = rand() % (vyskaKP/30);
			int vyska = vyskaKP / 30;
			int sirka = sirkaKP / 30;
			//Rectangle(hdc, x, y, x + sirka, y + vyska);
			SetRect(&rect, x * sirka, y * vyska, x * sirka + sirka, y * vyska + vyska);
			FillRect(hdc, &rect, farebnyBrush);
			SelectObject(hdc, oldBrush);
			//EndPaint(hwnd, &ps);
			ReleaseDC(hwnd, hdc);
			//InvalidateRect(hwnd, nullptr, false);
		}
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

void nakresliStvorce(HDC hdc, int x, int y, int sirka, int vyska, RECT &rect)
{
	
}
