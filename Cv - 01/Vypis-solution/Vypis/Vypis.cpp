// Zakladna aplikacia

#include <windows.h> // staršia knižnica
#include <windowsx.h>  // novšia verzia
#include <stdio.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//char szWinName[] = "MojeOkno"; 		// staré označovanie
TCHAR szWinName[] = "MojeOkno"; 		// Nazov oknovej triedy , toto automaticky windows preroí na char

int APIENTRY WinMain(HINSTANCE hThisInst, HINSTANCE hPrevInst, PSTR lpszArgs, int nWinMode)
{
	HWND        	hwnd;			//popisovač okna H - handle
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
	wndclass.lpszMenuName = NULL;                            // Ziadne menu

	wndclass.cbClsExtra = 0;          	// nie su potrebne
	wndclass.cbWndExtra = 0;          	// ziadne extra informacie

	//wndclass.hbrBackground = (HBRUSH)GetStockObject(COLOR_WINDOW + 1);	// Biele pozadie alebo (HBRUSH)(COLOR_WINDOW+1) použie sa farba z win
	wndclass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

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

	static int xPos = 0, yPos = 0;	//musia byť static, aby boli označené ako globálne, lebo mi z WM_MOUSEMOVE vyskakujeme a ked by neboli označené ako static tak sa zakaždym vynulujú

	char buf[100];

	switch (message) {
	case WM_MOUSEMOVE:
		xPos = GET_X_LPARAM(lParam);	//ziskam informácie o polohe myši, ms doc viac info
		yPos = GET_Y_LPARAM(lParam);
		InvalidateRect(hwnd, NULL, true); // vynútim prekreslenie okna, keď by som miesto true dal false, tak by mi to neprekreslovalo celé

		break;

	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps); // párové funkcie ktoré musia byť pri obsluhe správy
		GetClientRect(hwnd, &rect);
		wsprintf(buf, "%d, %d", xPos, yPos);	//konvertujem int na char cez buffer
		//DrawText(hdc, "AHOJ WINDOWS !!! ", -1, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		DrawText(hdc, buf, -1, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		EndPaint(hwnd, &ps);		// párová funkcia ktorá musí byť pri obsluhe správy
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, message, wParam, lParam); // správy ktoré ma nezaujímaju, pošlem naspeť do systému
	}
	return 0;
}