// Zakladna aplikacia

#include <windows.h>
#include <windowsx.h>
#include "resource.h"

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

	switch (message) {
	case WM_CREATE:
		hmenu = GetMenu(hwnd);
		break;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		GetClientRect(hwnd, &rect);
		DrawText(hdc, "AHOJ WINDOWS !!! ", -1, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		EndPaint(hwnd, &ps);
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_PROGRAM_START:
			EnableMenuItem(hmenu, ID_PROGRAM_START, MF_DISABLED);
			EnableMenuItem(hmenu, ID_PROGRAM_STOP, MF_ENABLED);
			PostMessage(hwnd, WM_CLOSE, 0, 0);
			break;
		case ID_PROGRAM_STOP:
			EnableMenuItem(hmenu, ID_PROGRAM_START, MF_ENABLED);
			EnableMenuItem(hmenu, ID_PROGRAM_STOP, MF_DISABLED);
			break;
		case ID_PROGRAM_KONIEC:
			PostMessage(hwnd, WM_CLOSE, 0, 0);
			break;
		default:
			break;
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}