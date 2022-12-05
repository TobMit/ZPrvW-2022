// Zakladna aplikacia

#include <windows.h>
#include <windowsx.h>
#include "resource.h"
#include <ctime>
#include <string>
#include <fstream>

using namespace std;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DialogFun(HWND, UINT, WPARAM, LPARAM);
void Uloz(HWND, string, char* cas);
char szWinName[] = "MojeOkno"; 		// Nazov oknovej triedy
HINSTANCE hInstance;

int APIENTRY WinMain(HINSTANCE hThisInst, HINSTANCE hPrevInst, PSTR lpszArgs, int nWinMode)
{
	HWND        	hwnd;
	MSG         	msg;
	WNDCLASSEX    	wndclass;

	hInstance = hThisInst;  // Odlzoenie posivaca instancie aplikacie do globalnej premennej

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

	switch (message) {
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_PROGRAM_STOPKY:
			DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), hwnd, (DLGPROC)DialogFun);
			break;
		case ID_PROGRAM_KONIEC:
			PostMessage(hwnd, WM_CLOSE, 0, 0);
			break;
		default:
			break;
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		GetClientRect(hwnd, &rect);
		//DrawText(hdc, "AHOJ WINDOWS !!! ", -1, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
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

BOOL CALLBACK DialogFun(HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static time_t startCas;
	static time_t aktualnyCas;
	static string nazov;
	switch (msg)
	{
	case WM_INITDIALOG:
		return false;
	case WM_TIMER:
	{
		time(&aktualnyCas);
		int sek = difftime(aktualnyCas, startCas);
		int hod = sek / 3600;
		int min = sek % 3600 / 60;
		sek %= 60;
		char stopkyBuf[30];
		wsprintf(stopkyBuf, "%02d:%02d:%02d", hod, min, sek);
		SetWindowText(GetDlgItem(hdlg, IDC_EDIT_CAS), stopkyBuf);
	}
	return true;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON_START:
			EnableWindow(GetDlgItem(hdlg, IDC_BUTTON_START), false);
			EnableWindow(GetDlgItem(hdlg, IDC_BUTTON_STOP), true);
			{
				char stopkyBuf[30] = { "00:00:00" };
				SetWindowText(GetDlgItem(hdlg, IDC_EDIT_CAS), stopkyBuf);
			}
			time(&startCas);
			SetTimer(hdlg, 1, 1000, nullptr);
			return true;
		case IDC_BUTTON_STOP:
			KillTimer(hdlg, 1);
			EnableWindow(GetDlgItem(hdlg, IDC_BUTTON_START), true);
			EnableWindow(GetDlgItem(hdlg, IDC_BUTTON_STOP), false);
			break;
		case IDC_EDIT_NAZOV:
			if (HIWORD(wParam) == EN_CHANGE)
			{
				char buf[100];
				GetWindowText(GetDlgItem(hdlg, IDC_EDIT_NAZOV), buf, 100);
				nazov = buf;
				EnableWindow(GetDlgItem(hdlg, IDC_BUTTON_ULOZ), nazov.length() > 0);
				return true;
			}
			return false;
		case IDC_BUTTON_ULOZ:
		{
			char cas[100];
			GetWindowText(GetDlgItem(hdlg, IDC_EDIT_CAS), cas, 100);
			Uloz(hdlg, nazov, cas);
		}
		return true;
		case IDOK:
			if (IsWindowEnabled(GetDlgItem(hdlg,IDC_BUTTON_STOP)))
				SendMessage(hdlg, WM_COMMAND, IDC_BUTTON_STOP, 0);
			EndDialog(hdlg, 0);
		}
		return false;
	}
	return false;
}


void Uloz(HWND hdlg, string nazov, char* cas)
{
	OPENFILENAME ofn;
	char menoSuboru[1000]{};

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hdlg;
	ofn.lpstrFile = menoSuboru;
	ofn.nMaxFile = sizeof(menoSuboru);
	ofn.lpstrFilter = "Všetky\0*.*\0Text\0*.txt\0";
	ofn.nFilterIndex = 2;
	ofn.lpstrFileTitle = 0;
	ofn.lpstrInitialDir = nullptr;
	ofn.Flags = OFN_PATHMUSTEXIST;
	if (GetSaveFileName(&ofn))
	{
		ofstream of(ofn.lpstrFile, ios_base::app);
		if (of)
		{
			of << nazov << ": " << cas << endl;
		}
	}
}
