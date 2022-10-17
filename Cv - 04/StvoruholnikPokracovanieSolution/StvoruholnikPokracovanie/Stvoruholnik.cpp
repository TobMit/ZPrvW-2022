// Zakladna aplikacia

#include <windows.h>
#include <windowsx.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

TCHAR szWinName[] = L"MojeOkno"; 		// Nazov oknovej triedy

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
		L"Rolovanie",  // Titulok okna
		WS_OVERLAPPEDWINDOW | WS_VSCROLL | WS_HSCROLL, // Styl okna
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
	static int x, y, sirka, vyska, sirkaKP, vyskaKP;
	static bool klik;
	static HRGN region;
	switch (message) {
	case WM_CREATE:
		x = y = 0;
		klik = true;
		break;
	case WM_SIZE:
		sirkaKP = LOWORD(lParam);
		vyskaKP = HIWORD(lParam);
		sirka = sirkaKP / 10;
		vyska = vyskaKP / 10;
		SetScrollRange(hwnd, SB_HORZ, 0, sirkaKP - sirka, true);
		SetScrollRange(hwnd, SB_VERT, 0, vyskaKP - vyska, true);
		x = x < 0 ? 0 : x;
		x = x > sirkaKP - sirka ? sirkaKP - sirka : x;
		y = y < 0 ? 0 : y;
		y = y > vyskaKP - vyska ? vyskaKP - vyska : y;
		break;
	case WM_VSCROLL:
		if (klik)
		{
			switch (LOWORD(wParam))
			{
			case SB_LINEDOWN:
				y++;
				break;
			case SB_PAGEDOWN:
				y += vyska;
				break;
			case SB_LINEUP:
				y--;
				break;
			case SB_PAGEUP:
				y -= vyska;
				break;
			case SB_THUMBPOSITION:
			case SB_THUMBTRACK:
				y = HIWORD(wParam);
				break;
			}
			y = y < 0 ? 0 : y;
			y = y > vyskaKP - vyska ? vyskaKP - vyska : y;
			SetScrollPos(hwnd, SB_VERT, y, false);
			InvalidateRect(hwnd, nullptr, true);
		}
		break;
	case WM_HSCROLL:
		if (klik)
		{
			switch (LOWORD(wParam))
			{
			case SB_LINERIGHT:
				x++;
				break;
			case SB_PAGERIGHT:
				x += sirka;
				break;
			case SB_LINELEFT:
				x--;
				break;
			case SB_PAGELEFT:
				x -= sirka;
				break;
			case SB_THUMBPOSITION:
			case SB_THUMBTRACK:
				x = HIWORD(wParam);
				break;
			}
			x = x < 0 ? 0 : x;
			x = x > sirkaKP - sirka ? sirkaKP - sirka : x;
			SetScrollPos(hwnd, SB_HORZ, x, false);
			InvalidateRect(hwnd, nullptr, true);
		}
		break;
	case WM_LBUTTONDOWN:
	{
		POINT p;
		p.x = GET_X_LPARAM(lParam);
		p.y = GET_Y_LPARAM(lParam);
		SetRect(&rect, x, y, x + sirka, y + vyska);
		region = CreateEllipticRgn(x, y, x + sirka, y + vyska);
		klik = PtInRegion(region, p.x, p.y);
		InvalidateRect(hwnd, nullptr, true);
	}
	break;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		Ellipse(hdc, x, y, x + sirka, y + vyska);
		if (klik)
		{
			HBRUSH sivy = GetStockBrush(LTGRAY_BRUSH);
			HBRUSH oldStetec = (HBRUSH)SelectObject(hdc, sivy);
			region = CreateEllipticRgn(x + 1, y + 1, x + sirka, y + vyska);
			FillRgn(hdc, region, sivy);
			SelectObject(hdc, oldStetec);
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