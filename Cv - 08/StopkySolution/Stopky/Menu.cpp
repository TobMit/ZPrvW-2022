﻿// Zakladna aplikacia

#include <windows.h>
#include <windowsx.h>
#include "resource.h"
#include <ctime>

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
		"Stopky",  // Titulok okna
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
	RECT	  rect;
	static time_t startCas, aktualnyCas;
	static HMENU hmenu;
	static bool casovacAktivny = false;

	switch (message) {
	case WM_CREATE:
		hmenu = GetMenu(hwnd);
		break;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		{ // zavolá sa alokácia / inicializácia objekut
			GetClientRect(hwnd, &rect);
			int difCas = aktualnyCas - startCas;
			char casBuf[20];
			wsprintf(casBuf,"%02d:%02d:%02d", difCas/3600, (difCas%3600)/60, (difCas%3600%60)); // funkcia windowsu má výhodu oproti sprintf, je uložená v dll
			DrawText(hdc, casBuf, -1, &rect, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
		} // volá sa deštruktor objektu
		EndPaint(hwnd, &ps);
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_PROGRAM_KONIEC:
			PostMessage(hwnd, WM_CLOSE, 0, 0);
				break;
		case ID_PROGRAM_40005: //toto by malo byť štart
			casovacAktivny = true;
			startCas = aktualnyCas = time(nullptr);
			InvalidateRect(hwnd,nullptr, true);
			SetTimer(hwnd, 1, 1000, nullptr); // keď je tam nullptr správa ide do oknovej procedúri
			EnableMenuItem(hmenu, ID_PROGRAM_40005, MF_DISABLED);
			EnableMenuItem(hmenu, ID_PROGRAM_STOP, MF_ENABLED);
			break;

		case ID_PROGRAM_STOP:
			KillTimer(hwnd,1);
			casovacAktivny = false;
			EnableMenuItem(hmenu, ID_PROGRAM_40005, MF_ENABLED);
			EnableMenuItem(hmenu, ID_PROGRAM_STOP, MF_DISABLED);
			break;
		}
		break;

	case WM_TIMER:
		aktualnyCas = time(nullptr);
		InvalidateRect(hwnd, nullptr, true);
		break;

	case WM_DESTROY:
		if (casovacAktivny)
		{
			KillTimer(hwnd, 1);
		}
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}