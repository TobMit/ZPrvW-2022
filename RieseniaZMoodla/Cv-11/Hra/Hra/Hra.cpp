// Zakladna aplikacia

#include <windows.h>
#include <windowsx.h>
#include "resource.h"
#include <ctime>
#include <cstdlib>
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

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
		"Hra",  // Titulok okna
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
	return msg.wParam;
}

// Tuto funkciu vola Windows - prenasa do nej spravy z fronty sprav
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	static RECT	  rect;
	static bool start;
	static HMENU menu;
	static int x = 0, y = 0;
	static int body = 0;
	switch (message) {
	case WM_CREATE:
		menu = GetMenu(hwnd);
		start = false;
		EnableMenuItem(menu, ID_PROGRAM_START, MF_ENABLED);
		EnableMenuItem(menu, ID_PROGRAM_STOP, MF_DISABLED);
		GetClientRect(hwnd, &rect);
		srand(time(nullptr));
		break;
	case WM_TIMER:
		y++;
		if (y + 40 >= rect.bottom)
			PostMessage(hwnd, WM_COMMAND, ID_PROGRAM_STOP, 0);
		else
			InvalidateRect(hwnd, 0, true);
		break;
	case WM_LBUTTONDOWN:
	{
		RECT kocka;
		SetRect(&kocka, x, y, x + 40, y + 40);
		POINT p;
		p.x = GET_X_LPARAM(lParam);
		p.y = GET_Y_LPARAM(lParam);
		HRGN region = CreateEllipticRgn(x, y, x + 40, y + 40);
		if (PtInRegion(region, p.x, p.y))
			//if (PtInRect(&kocka, p))
		{
			body++;
			x = rand() % (rect.right - 40);
			y = 0;
			InvalidateRect(hwnd, nullptr, true);
		}
		else
			body--;
		char buf[100];
		wsprintf(buf, "Hra: %d", body);
		SetWindowText(hwnd, buf);
	}
	break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_PROGRAM_START:
			EnableMenuItem(menu, ID_PROGRAM_START, MF_DISABLED);
			EnableMenuItem(menu, ID_PROGRAM_STOP, MF_ENABLED);
			body = 0;
			start = true;
			x = rand() % (rect.right - 40);
			y = 0;
			InvalidateRect(hwnd, nullptr, true);
			SetTimer(hwnd, 1, 10, nullptr);
			break;
		case ID_PROGRAM_STOP:
			EnableMenuItem(menu, ID_PROGRAM_START, MF_ENABLED);
			EnableMenuItem(menu, ID_PROGRAM_STOP, MF_DISABLED);
			start = false;
			break;
		case ID_PROGRAM_KONIEC:
			PostMessage(hwnd, WM_CLOSE, 0, 0);
		default:
			break;
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		if (start)
		{
			Ellipse(hdc, x, y, x + 40, y + 40);
		}
		EndPaint(hwnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}