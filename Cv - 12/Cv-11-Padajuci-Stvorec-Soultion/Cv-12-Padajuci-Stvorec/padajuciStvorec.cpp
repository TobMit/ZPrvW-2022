// Zakladna aplikacia

#include <windows.h>
#include <windowsx.h>
#include "resource.h"
#include <cstdlib>
#include <ctime>

#define ROZMER_STVORCA 40

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
	static int sirkaKP, vyskaKP;
	static int poziciaX, poziciaY;
	static bool klik[100], start;
	static int body;
	static int stvorce[100][3];

	switch (message) {
	case WM_CREATE:
		srand(time(NULL));
		sirkaKP = vyskaKP = 0;
		poziciaX = poziciaY = 0;
		body = 0;
		for (int i = 0; i < 100; i++)
		{
			klik[i] = true;
		}
		start = false;
		SetTimer(hwnd, 1, 16, nullptr);
		memset(stvorce, -1, sizeof(stvorce));
		break;
	case WM_SIZE:
		sirkaKP = LOWORD(lParam);
		vyskaKP = HIWORD(lParam);
		break;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		if (start)
		{
			//dokončenie kažých 5s sa generuje nový štvorec a jeden červený ktorý dáva viac bodov
			for (int i = 0; i < 100; i++) {
				if (klik[i])
				{
					klik[i] = false;
					poziciaX = rand() % (sirkaKP - ROZMER_STVORCA);
					poziciaY = 0;
				}
				if (poziciaY + ROZMER_STVORCA > vyskaKP)
				{
					body--;
					klik[i] = true;
					InvalidateRect(hwnd, nullptr, true);
				}
				Rectangle(hdc, poziciaX, poziciaY, ROZMER_STVORCA + poziciaX, ROZMER_STVORCA + poziciaY);
			}
		}
		EndPaint(hwnd, &ps);
		char buf[20];
		wsprintf(buf, "Hra: %db", body);
		SetWindowText(hwnd, buf);
		break;
	case WM_LBUTTONDOWN:
		POINT p;
		p.x = GET_X_LPARAM(lParam);
		p.y = GET_Y_LPARAM(lParam);
		{
			bool trafil = false;
			for (int i = 0; i < 100; i++)
			{
				if (p.x >= poziciaX && p.x <= poziciaX + ROZMER_STVORCA &&
					p.y >= poziciaY && p.y <= poziciaY + ROZMER_STVORCA)
				{
					klik[i] = true;
					body++;
					InvalidateRect(hwnd, nullptr, true);
					trafil = true;
					break;
				}
			}

			if (!trafil)
			{
				body--;
			}
		}
		break;
	case WM_TIMER:
		if (start) {
			poziciaY++;
			
			InvalidateRect(hwnd, nullptr, true);
		}
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_PROGRAM_KONIEC:
			PostMessage(hwnd, WM_CLOSE, 0, 0);
			break;
		case ID_PROGRAM_START:
			start = true;
			InvalidateRect(hwnd, nullptr, true);
			break;
		case ID_PROGRAM_STOP:
			start = false;
			InvalidateRect(hwnd, nullptr, true);
			break;
			break;
		default:
			break;
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