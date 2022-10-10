// Zakladna aplikacia

#include <windows.h>
#include <windowsx.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

char szWinName[] = "MojeOkno"; 		// Nazov oknovej triedy

int APIENTRY WinMain(HINSTANCE hThisInst, HINSTANCE hPrevInst, PSTR lpszArgs, int nWinMode)
{
	HWND        	hwnd;
	MSG         	msg;
	WNDCLASSEX    	wndclass;

	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW;          	// Implicitny styl
	wndclass.lpfnWndProc = WndProc; 	// Oknova funkcia
	wndclass.hInstance = hThisInst;  	// Popisovac tejto instancie
	wndclass.lpszClassName = szWinName;  	// Nazov oknovej triedy

	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION); // Druh ikony
	wndclass.hIconSm = 0; 		// Druh malej ikony

	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);     // Druh kurzora
	wndclass.lpszMenuName = NULL;                            // Ziadne menu

	wndclass.cbClsExtra = 0;          	// nie su potrebne
	wndclass.cbWndExtra = 0;          	// ziadne extra informacie

	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);	// Biele pozadie alebo (HBRUSH)(COLOR_WINDOW+1)

	RegisterClassEx(&wndclass);            // Registracia okna
										   // Po registracii vytvorenie okna
	hwnd = CreateWindowEx(
		WS_EX_ACCEPTFILES,
		szWinName,           // Nazov oknovej triedy
		"Windows - zakladny program",  // Titulok okna
		WS_OVERLAPPEDWINDOW | WS_HSCROLL | WS_VSCROLL, // Styl okna
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
	static int vyska = 0;
	static int sirka = 0;
	static int xPociatok = 0;
	static int yPociatok = 0;
	static int sirkaKP = 0;
	static int vyskaKP = 0;
	static bool klik;

	switch (message) {
	case WM_SIZE:
	{
		sirkaKP = LOWORD(lParam);
		vyskaKP = HIWORD(lParam);
		vyska = HIWORD(lParam) / 10;
		sirka = LOWORD(lParam) / 10;
		SetScrollRange(hwnd, SB_HORZ, 0, sirkaKP - sirka, true);
		SetScrollRange(hwnd, SB_VERT, 0, vyskaKP - vyska, true);
	}
	break;

	case WM_VSCROLL:
		if (!klik) {
			switch (LOWORD(wParam))
			{
			case SB_THUMBTRACK:
				yPociatok = HIWORD(wParam);
				SetScrollPos(hwnd, SB_VERT, HIWORD(wParam), true);
				InvalidateRect(hwnd, NULL, true);
				break;
			case SB_LINEDOWN:
				if (yPociatok + vyska < vyskaKP) {
					yPociatok = yPociatok + 1;
				}
				SetScrollPos(hwnd, SB_VERT, yPociatok, true);
				InvalidateRect(hwnd, NULL, true);
				break;
			case SB_LINEUP:
				if (yPociatok > 0) {
					yPociatok = yPociatok - 1;
				}

				SetScrollPos(hwnd, SB_VERT, yPociatok, true);
				InvalidateRect(hwnd, NULL, true);
				break;
			case SB_PAGEDOWN:
				yPociatok = yPociatok + vyska;
				SetScrollPos(hwnd, SB_VERT, yPociatok, true);
				InvalidateRect(hwnd, NULL, true);
				break;
			case SB_PAGEUP:
				yPociatok = yPociatok - vyska;
				SetScrollPos(hwnd, SB_VERT, yPociatok, true);
				InvalidateRect(hwnd, NULL, true);
				break;
			default:
				break;
			}
		}
		break;

	case WM_HSCROLL:
		if (!klik) {
			switch (LOWORD(wParam))
			{
			case SB_THUMBTRACK:
				xPociatok = HIWORD(wParam);
				SetScrollPos(hwnd, SB_HORZ, HIWORD(wParam), true);
				InvalidateRect(hwnd, NULL, true);
				break;
			case SB_LINEDOWN:
				if (xPociatok + sirka < sirkaKP)
				{
					xPociatok = xPociatok + 1;
				}
				SetScrollPos(hwnd, SB_HORZ, xPociatok, true);
				InvalidateRect(hwnd, NULL, true);
				break;
			case SB_LINEUP:
				if (xPociatok > 0)
				{
					xPociatok = xPociatok - 1;
				}
				SetScrollPos(hwnd, SB_HORZ, xPociatok, true);
				InvalidateRect(hwnd, NULL, true);
				break;
			case SB_PAGEDOWN:
				xPociatok = xPociatok + sirka;
				SetScrollPos(hwnd, SB_HORZ, xPociatok, true);
				InvalidateRect(hwnd, NULL, true);
				break;
			case SB_PAGEUP:
				xPociatok = xPociatok - sirka;
				SetScrollPos(hwnd, SB_HORZ, xPociatok, true);
				InvalidateRect(hwnd, NULL, true);
				break;
			default:
				break;
			}
		}
		break;
	case WM_LBUTTONDOWN:
	{	// keď je v case {} tak sa z toho stávaju glob premenné I gess
		POINT p;
		p.x = GET_X_LPARAM(wParam);
		p.y = GET_Y_LPARAM(wParam);
		
		SetRect(&rect, xPociatok, xPociatok + sirka, yPociatok, yPociatok + vyska);
		klik = PtInRect(&rect, p);
		InvalidateRect(hwnd, nullptr, true);
	}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);

		Rectangle(hdc, xPociatok, yPociatok, xPociatok + sirka, yPociatok + vyska);
		if (klik)
		{
			HBRUSH  sivy = GetStockBrush(GRAY_BRUSH);
			HBRUSH oldstetc = (HBRUSH)SelectObject(hdc, sivy);
			SetRect(&rect, xPociatok, xPociatok + sirka, yPociatok, yPociatok + sirka);
			FillRect(hdc, &rect, sivy);
			SelectObject(hdc,oldstetc);
			// stetc nemusim mazať pretože ziskavame systémovi štetec

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