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

	HACCEL hacell = LoadAccelerators(hThisInst, MAKEINTRESOURCE(IDR_ACCELERATOR1));

	// Slucka sprav
	while (GetMessage(&msg, NULL, 0, 0)) {
		if (hacell && !TranslateAcceleratorA(hwnd, hacell, &msg)) {
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
	static bool mamText;
	static char* mojeData;

	switch (message) {
	case WM_CREATE:
		mamText = 0;
		mojeData = nullptr;
		break;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		GetClientRect(hwnd, &rect);
		if (mojeData)
		{
			DrawText(hdc, mojeData, -1, &rect, DT_LEFT | DT_EXPANDTABS); // aby zobrazovalo tabelátori
		}
		EndPaint(hwnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
		// ak nemám text tak bude rozbalovacie menu edit zablokovane
	case WM_INITMENUPOPUP: // reaguje na rozbalenie menu
		if (LOWORD(lParam) == 1)
		{
			HMENU hmenu = GetMenu(hwnd);
			mamText = IsClipboardFormatAvailable(CF_TEXT); // kontrolujem či sa nachádza text
			EnableMenuItem(hmenu, ID_EDIT_VLOZ, mamText ? MF_ENABLED : MF_DISABLED);
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
			if (true) // pre istotu ak by som nemal text tak bude zablokovaná
			{
				HANDLE hclip = GetClipboardData(CF_TEXT);
				if (hclip)
				{
					delete[] mojeData; // ak by som tam mal niečo z predchadzajúceho behu tak to vynulujem
					mojeData = nullptr;
					int pocet = GlobalSize(hclip);
					if (pocet > 0)
					{
						mojeData = new char[pocet + 1];
						char* pdata = (char*)GlobalLock(hclip);
						if (pdata) {
							lstrcpy(mojeData, pdata);
							InvalidateRect(hwnd, nullptr, true);
						}
						GlobalUnlock(hclip);
						
					}
				}
			}
			break;
		}
		break;
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}