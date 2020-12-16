#include "framework.h"
#include "OldSynthesizer.h"

#define MAX_LOADSTRING 100

HINSTANCE hInst;                                
CHAR szTitle[MAX_LOADSTRING];                  
CHAR szWindowClass[MAX_LOADSTRING];    

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadString(hInstance, IDC_OLDSYNTHESIZER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_OLDSYNTHESIZER));

    MSG msg;

    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}


ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_OLDSYNTHESIZER));
    wcex.hIconSm        = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SMALL));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCE(IDC_OLDSYNTHESIZER);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassEx(&wcex);
}


BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance;

   HWND hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 21 * WHITE_WIDTH, 3 * WHITE_WIDTH + WHITE_HEIGHT - 11, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   InitMenu(hWnd);

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

VOID InitMenu(HWND hWnd)
{
    HMENU progmenu = CreatePopupMenu();
    //SetMenuItemBitmaps(progmenu, 0, MF_BYPOSITION, nullptr, nullptr);
    AppendMenu(progmenu, MF_CHECKED, MENU_SIGN, "Sign\tTab");
    //SetMenuItemBitmaps(progmenu, 1, MF_BYPOSITION, nullptr, nullptr);
    AppendMenu(progmenu, MF_CHECKED, MENU_PEDAL, "Pedal\tLShift");
    AppendMenu(progmenu, MF_STRING, IDM_ABOUT, "About\tF1");
    AppendMenu(progmenu, MF_STRING, IDM_EXIT, "Exit\tAlt+F4");

    HMENU instmenu = CreatePopupMenu();
    AppendMenu(instmenu, MF_STRING, INST_PIANO, "Piano\tF2");
    AppendMenu(instmenu, MF_STRING, INST_GUITAR, "Guitar\tF3");
    AppendMenu(instmenu, MF_STRING, INST_DRUM, "Drum\tF4");

    HMENU octmenu = CreatePopupMenu();
    AppendMenu(octmenu, MF_STRING, OCT_CONTRA, "Contra+");
    AppendMenu(octmenu, MF_STRING, OCT_GREATE, "Greate+");
    AppendMenu(octmenu, MF_STRING, OCT_SMALL, "Small+");
    AppendMenu(octmenu, MF_STRING, OCT_FIRST, "First+");
    AppendMenu(octmenu, MF_STRING, OCT_SECOND, "Second+");
    AppendMenu(octmenu, MF_STRING, OCT_THIRD, "Third+");

    HMENU menu = CreateMenu();
    AppendMenu(menu, MF_POPUP, (UINT_PTR)progmenu, "Synthesizer");
    AppendMenu(menu, MF_POPUP, (UINT_PTR)instmenu, "Instrument");
    AppendMenu(menu, MF_POPUP, (UINT_PTR)octmenu, "Octave");
    SetMenu(hWnd, menu);
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);

            switch (wmId)
            {
            case MENU_SIGN:
                break;
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_CREATE:
        {
            pKey = (PKeyStruct)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(KeyStruct));
            SetWindowLong(hWnd, 0, (LONG)pKey);

            LOGFONT lf;
            ZeroMemory(&lf, sizeof(LOGFONT));
            lf.lfHeight = 25;
            strcpy_s(lf.lfFaceName, "Consolas");
            hFont = CreateFontIndirect(&lf);

            pKey->hbrBlack = CreateSolidBrush(RGB(0, 0, 0));
            pKey->hbrWhite = CreateSolidBrush(RGB(255, 255, 255));
            pKey->hbrGray = CreateSolidBrush(RGB(255, 0, 0));
            for (int i = 0; i < 19; i++)
            {
                pKey->whiteRect[i].top = KEY_TOP;
                pKey->whiteRect[i].bottom = KEY_TOP + WHITE_HEIGHT;
                pKey->whiteRect[i].left = KEY_LEFT + WHITE_WIDTH * i;
                pKey->whiteRect[i].right = pKey->whiteRect[i].left + WHITE_WIDTH;
            }
            int blackCount = 0;
            for (int i = 0; i < 18; i++)
            {
                if ((i == 2) || (i == 6) || (i == 9) || (i == 13) || (i == 16))
                    continue;
                pKey->blackRect[blackCount].top = KEY_TOP;
                pKey->blackRect[blackCount].bottom = KEY_TOP + BLACK_HEIGHT;
                pKey->blackRect[blackCount].left = KEY_LEFT + WHITE_WIDTH * (i + 1) - BLACK_WIDTH / 2;
                pKey->blackRect[blackCount].right = pKey->blackRect[blackCount].left + BLACK_WIDTH;
                blackCount++;
            }
        }
        break;
    case WM_KEYDOWN:
        {
            if ((wParam > 112) && (wParam < 116))
            {
                instrumentIndex = wParam - 113;
                InvalidateRect(hWnd, NULL, false);
            }

            switch (wParam)
            {
            case VK_F1:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case VK_TAB:
                {
                    isSign = isSign xor 1;
                    InvalidateRect(hWnd, NULL, false);
                }
                break;
            case VK_LEFT:
                {
                    if (Octave > -3)
                    {
                        Octave--;
                        InvalidateRect(hWnd, NULL, false);
                    }
                }
                break;
            case VK_RIGHT:
                {
                    if (Octave < 2)
                    {
                        Octave++;
                        InvalidateRect(hWnd, NULL, false);
                    }
                }
                break;
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            
            HDC buffDC = CreateCompatibleDC(hdc);
            RECT clientRect;
            GetClientRect(hWnd, &clientRect);
            HBITMAP hBM = CreateCompatibleBitmap(hdc, clientRect.right - clientRect.left, clientRect.bottom - clientRect.top);
            SelectObject(buffDC, hBM);
            FillRect(buffDC, &clientRect, (HBRUSH)GetStockObject(WHITE_BRUSH));

            for (int i = 0; i < 19; i++)
            {
                if (pKey->isWhitePress[i])
                    SelectObject(buffDC, pKey->hbrGray);
                else
                    SelectObject(buffDC, pKey->hbrWhite);
                Rectangle(buffDC, pKey->whiteRect[i].left, pKey->whiteRect[i].top, pKey->whiteRect[i].right, pKey->whiteRect[i].bottom);
            }
            
            for (int i = 0; i < 13; i++)
            {
                if (pKey->isBlackPress[i])
                    SelectObject(buffDC, pKey->hbrGray);
                else
                    SelectObject(buffDC, pKey->hbrBlack);
                Rectangle(buffDC, pKey->blackRect[i].left, pKey->blackRect[i].top, pKey->blackRect[i].right, pKey->blackRect[i].bottom);
            }

            if (isSign)
            {
                SelectObject(buffDC, hFont);
                for (int i = 0; i < 13; i++)
                {
                    SetTextColor(buffDC, RGB(255, 255, 255));
                    SetBkMode(buffDC, TRANSPARENT);
                    DrawText(buffDC, blackSimbols[i], 1, &pKey->blackRect[i], DT_BOTTOM | DT_SINGLELINE | DT_CENTER);
                }
                for (int i = 0; i < 19; i++)
                {
                    SetTextColor(buffDC, RGB(0, 0, 0));
                    SetBkMode(buffDC, TRANSPARENT);
                    DrawText(buffDC, whiteSimbols[i], 1, &pKey->whiteRect[i], DT_BOTTOM | DT_SINGLELINE | DT_CENTER);
                }
            }

            SelectObject(buffDC, hFont);
            TextOut(buffDC, KEY_LEFT, KEY_TOP + WHITE_HEIGHT + 15, octaves[Octave + 3], 20);

            SelectObject(buffDC, hFont);
            TextOut(buffDC, KEY_LEFT, 15, instruments[instrumentIndex], 6);
            
            SetStretchBltMode(hdc, COLORONCOLOR);
            BitBlt(hdc, 0, 0, clientRect.right - clientRect.left, clientRect.bottom - clientRect.top, buffDC, 0, 0, SRCCOPY);

            DeleteDC(buffDC);
            DeleteObject(hBM);
            buffDC = NULL;

            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
