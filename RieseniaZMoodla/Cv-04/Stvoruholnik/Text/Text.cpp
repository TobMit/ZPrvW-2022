// Zakladna aplikacia

#include <windows.h>
#include <windowsx.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void SpracujKlavesu(HWND hwnd, int wParam);

TCHAR szWinName[] = L"MojeOkno"; 		// Nazov oknovej triedy

int x, y, sirka, vyska, sirkaKP, vyskaKP;

const int WM_MOJASPRAVA = WM_USER;

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
	static bool klik;
	static HRGN region;
	TEXTMETRIC tm;
	static TCHAR keyBuf[5];

	switch (message) {
	case WM_CREATE:
		x = y = 0;
		klik = true;
		hdc = GetDC(hwnd);
		GetTextMetrics(hdc, &tm);
		ReleaseDC(hwnd, hdc);
		sirka = tm.tmMaxCharWidth;
		vyska = tm.tmHeight;
		break;
	case WM_SIZE:
		sirkaKP = LOWORD(lParam);
		vyskaKP = HIWORD(lParam);
		SetScrollRange(hwnd, SB_HORZ, 0, sirkaKP / sirka, true);
		SetScrollRange(hwnd, SB_VERT, 0, vyskaKP / vyska, true);
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
				y += vyska;
				break;
			case SB_LINEUP:
				y -= vyska;
				break;
			case SB_THUMBPOSITION:
			case SB_THUMBTRACK:
				y = HIWORD(wParam);
				break;
			}
			y = y < 0 ? 0 : y;
			y = y > vyskaKP - vyska ? vyskaKP - vyska : y;
			SetScrollPos(hwnd, SB_VERT, y / vyska, true);
			SetCaretPos(x, y);
			InvalidateRect(hwnd, nullptr, false);
		}
		break;
	case WM_HSCROLL:
		if (klik)
		{
			switch (LOWORD(wParam))
			{
			case SB_LINERIGHT:
				x += sirka;
				break;
			case SB_LINELEFT:
				x -= sirka;
				break;
			case SB_THUMBPOSITION:
			case SB_THUMBTRACK:
				x = HIWORD(wParam);
				break;
			}
			x = x < 0 ? 0 : x;
			x = x > sirkaKP - sirka ? sirkaKP - sirka : x;
			SetScrollPos(hwnd, SB_HORZ, x / sirka, true);
			SetCaretPos(x, y);
			InvalidateRect(hwnd, nullptr, false);
		}
		break;
	case WM_MOJASPRAVA:
		SpracujKlavesu(hwnd, wParam);
		break;

	case WM_KEYDOWN:
		SendMessage(hwnd, WM_MOJASPRAVA, wParam, 0);
		break;
	case WM_CHAR:
		*keyBuf = wParam;
		HideCaret(hwnd);
		hdc = GetDC(hwnd);
		TextOut(hdc, x, y, keyBuf, 1);
		SendMessage(hwnd, WM_MOJASPRAVA, VK_RIGHT, 0);
		ReleaseDC(hwnd, hdc);
		ShowCaret(hwnd);
		break;
	case WM_SETFOCUS:
		CreateCaret(hwnd, nullptr, 1, vyska);
		SetCaretPos(x, y);
		ShowCaret(hwnd);
		break;
	case WM_KILLFOCUS:
		HideCaret(hwnd);
		DestroyCaret();
		break;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
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

static bool kurzor = false;

void SpracujKlavesu(HWND hwnd, int wParam)
{
	switch (wParam)
	{
	case VK_UP:
		SendMessage(hwnd, WM_VSCROLL, SB_LINEUP, 0);
		break;
	case VK_DOWN:
		SendMessage(hwnd, WM_VSCROLL, SB_LINEDOWN, 0);
		break;
	case VK_NEXT:
		SendMessage(hwnd, WM_VSCROLL, SB_PAGEDOWN, 0);
		break;
	case VK_PRIOR:
		SendMessage(hwnd, WM_VSCROLL, SB_PAGEUP, 0);
		break;
	case VK_LEFT:
		SendMessage(hwnd, WM_HSCROLL, GetKeyState(VK_CONTROL) < 0 ? SB_PAGELEFT : SB_LINELEFT, 0);
		break;
	case VK_RIGHT:
		SendMessage(hwnd, WM_HSCROLL, GetKeyState(VK_CONTROL) < 0 ? SB_PAGERIGHT : SB_LINERIGHT, 0);
		break;
	case VK_HOME:
		x = 1;
		y = 0;
		SendMessage(hwnd, WM_HSCROLL, SB_LINELEFT, 0);
		SendMessage(hwnd, WM_VSCROLL, SB_LINEUP, 0);
		break;
	case VK_END:
		x = sirkaKP - sirka - 1;
		y = vyskaKP - vyska;
		SendMessage(hwnd, WM_HSCROLL, SB_LINERIGHT, 0);
		SendMessage(hwnd, WM_VSCROLL, SB_LINEDOWN, 0);
		break;
	case VK_INSERT:
		HideCaret(hwnd);
		DestroyCaret();
		CreateCaret(hwnd, 0, kurzor ? 1 : sirka, vyska);
		kurzor = !kurzor;
		ShowCaret(hwnd);
		break;
	}
}