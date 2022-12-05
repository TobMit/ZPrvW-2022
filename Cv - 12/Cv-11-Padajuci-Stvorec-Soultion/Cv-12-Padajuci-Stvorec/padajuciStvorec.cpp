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
	static bool start;
	static int body;
	static int cas;
	static int stvorce[100][3];
	static HMENU hmenu;

	switch (message) {
	case WM_CREATE:
		srand(time(NULL));
		sirkaKP = vyskaKP = 0;
		poziciaX = poziciaY = 0;
		body = 0;
		/*for (int i = 0; i < 100; i++)
		{
			klik[i] = true;
		}*/
		start = false;
		SetTimer(hwnd, 1, 10, nullptr);
		memset(stvorce, -1, sizeof(stvorce));
		hmenu = GetMenu(hwnd);
		cas = 0;
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
				if (stvorce[i][0] != -1)
				{
					if (stvorce[i][1] + ROZMER_STVORCA > vyskaKP)
					{
						body--;
						//klik[i] = true;
						InvalidateRect(hwnd, nullptr, true);
						stvorce[i][0] = -1;
						stvorce[i][1] = -1;
						stvorce[i][2] = -1;
					}
					//Rectangle(hdc, poziciaX, poziciaY, ROZMER_STVORCA + poziciaX, ROZMER_STVORCA + poziciaY);

					Rectangle(hdc, stvorce[i][0], stvorce[i][1], ROZMER_STVORCA + stvorce[i][0], ROZMER_STVORCA + stvorce[i][1]);
					if (stvorce[i][2] == 1)
					{
						HBRUSH redBrush = CreateSolidBrush(RGB(255, 0, 0));
						HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, redBrush);
;						SetRect(&rect, stvorce[i][0], stvorce[i][1], ROZMER_STVORCA + stvorce[i][0], ROZMER_STVORCA + stvorce[i][1]);
						FillRect(hdc, &rect, redBrush);
						SelectObject(hdc, oldBrush);
					}
				}
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
				/*if (p.x >= poziciaX && p.x <= poziciaX + ROZMER_STVORCA &&
					p.y >= poziciaY && p.y <= poziciaY + ROZMER_STVORCA)*/
				if (p.x >= stvorce[i][0] && p.x <= stvorce[i][0] + ROZMER_STVORCA &&
					p.y >= stvorce[i][1] && p.y <= stvorce[i][1] + ROZMER_STVORCA)
				{
					//klik[i] = true;
					if (stvorce[i][2] == 1)
					{
						body += 5;
					}
					else {
						body++;
					}
					stvorce[i][0] = rand() % (sirkaKP - ROZMER_STVORCA);
					stvorce[i][1] = 0;
					if (rand()%10 == 0)
					{
						stvorce[i][2] = 1;
					}
					else {
						stvorce[i][2] = -1;
					}
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
			for (int i = 0; i < 100; i++)
			{
				if (stvorce[i][0] != -1)
				{
					stvorce[i][1]++;
				}
			}
			//poziciaY++;
			
			InvalidateRect(hwnd, nullptr, true);
		}
		if (cas % 500 == 0) {
			for (int i = 0; i < 100; i++)
			{
				if (stvorce[i][0] == -1) {
					stvorce[i][0] = rand() % (sirkaKP - ROZMER_STVORCA);
					stvorce[i][1] = 0;
					stvorce[i][2] = -1;
					InvalidateRect(hwnd, nullptr, true);
					break;
				}
			}
		}
		cas++;
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_PROGRAM_KONIEC:
			PostMessage(hwnd, WM_CLOSE, 0, 0);
			break;
		case ID_PROGRAM_START:
			EnableMenuItem(hmenu, ID_PROGRAM_START, MF_DISABLED);
			EnableMenuItem(hmenu, ID_PROGRAM_STOP, MF_ENABLED);
			start = true;
			body = 0;
			InvalidateRect(hwnd, nullptr, true);
			break;
		case ID_PROGRAM_STOP:
			EnableMenuItem(hmenu, ID_PROGRAM_START, MF_ENABLED);
			EnableMenuItem(hmenu, ID_PROGRAM_STOP, MF_DISABLED);
			start = false;
			for (int i = 0; i < 100; i++)
			{
				stvorce[i][0] = -1;
				stvorce[i][1] = -1;
				stvorce[i][2] = -1;
			}
			InvalidateRect(hwnd, nullptr, true);
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