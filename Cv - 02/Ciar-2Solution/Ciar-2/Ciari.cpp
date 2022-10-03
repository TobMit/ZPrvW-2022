// Zakladna aplikacia

#include <windows.h>
#include <windowsx.h>
#include <cstdlib>
#include <ctime>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

char szWinName[] = "Ciary"; 		// Nazov oknovej triedy

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
	wndclass.lpszMenuName = NULL;                            // Ziadne menu

	wndclass.cbClsExtra = 0;          	// nie su potrebne
	wndclass.cbWndExtra = 0;          	// ziadne extra informacie

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
	const int POCET_BODOV = 100;
	HDC hdc;
	PAINTSTRUCT ps;
	RECT	  rect;
	static POINT body[POCET_BODOV]; //sturktura xy
	int sirka, vyska;
	static HPEN pero;
	HPEN povodnePero;


	switch (message) {
	case WM_SIZE:
		vyska = HIWORD(lParam);
		sirka = LOWORD(lParam);
		if (vyska > 0 && sirka > 0) {
			for (int i = 0; i < POCET_BODOV; i++)
			{
				body[i].x = rand() % sirka;
				body[i].y = rand() % vyska;
			}
		}

		break;
	case WM_CREATE:
		srand(time(nullptr));
		for (int i = 0; i < POCET_BODOV; i++) {
			body[i].x = 0;
			body[i].y = 0;
		}
		//pero = CreatePen(PS_SOLID, 1, RGB(255, 0, 0)); //keď vytváram pero tak musíme zrušiť
		break;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		for (int i = 0; i < POCET_BODOV; i++)
		{
			SetPixel(hdc, body[i].x, body[i].y, RGB(0, 0, 0));
		}
		//povodnePero = (HPEN)SelectObject(hdc, pero);
		for (int i = 0; i < POCET_BODOV - 1; i++)
		{
			for (int j = 0; j < POCET_BODOV; j++)
			{
				pero = CreatePen(PS_SOLID, 1, RGB(rand() % 255, rand() % 255, rand() % 255)); //keď vytváram pero tak musíme zrušiť
				povodnePero = (HPEN)SelectObject(hdc, pero);

				MoveToEx(hdc, body[i].x, body[i].y, nullptr);
				LineTo(hdc, body[j].x, body[j].y);
				SelectObject(hdc, povodnePero); //aby sa vrátila farba 
				DeleteObject(pero);
			}
			
		
		}

		//SelectObject(hdc, povodnePero); //aby sa vrátila farba 
		EndPaint(hwnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		//DeleteObject(pero); // musíme ho zrušiť aby sme uvolnili operačnú pamäť
		break;
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}