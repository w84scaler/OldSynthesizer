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

    midiOutOpen(&hmidi, MIDI_MAPPER, 0, 0, 0);
    FillKeyArrays();

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

   HDC hDCScreen = GetDC(NULL);
   int Horres = GetDeviceCaps(hDCScreen, HORZRES);
   int Vertres = GetDeviceCaps(hDCScreen, VERTRES);
   ReleaseDC(NULL, hDCScreen);

   HWND hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_SIZEBOX,
       (Horres - 21 * WHITE_WIDTH) / 2, (3 * WHITE_WIDTH + WHITE_HEIGHT) / 2 + WHITE_WIDTH, 21 * WHITE_WIDTH, 3 * WHITE_WIDTH + WHITE_HEIGHT - 11, nullptr, nullptr, hInstance, nullptr);

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
    HMENU colorMenu = CreatePopupMenu();
    AppendMenu(colorMenu, MF_STRING, COLOR_WHITE, "White key");
    AppendMenu(colorMenu, MF_STRING, COLOR_BLACK, "Black key");
    AppendMenu(colorMenu, MF_STRING, COLOR_GRAY, "Gray key");

    HMENU progmenu = CreatePopupMenu();
    AppendMenu(progmenu, MF_UNCHECKED, MENU_SIGN, "Sign\tTab");
    AppendMenu(progmenu, MF_UNCHECKED, MENU_PEDAL, "Pedal\tShift");
    AppendMenu(progmenu, MF_POPUP | MF_STRING, (UINT_PTR)colorMenu, "Color");
    AppendMenu(progmenu, MF_STRING, IDM_ABOUT, "About\tF1");
    AppendMenu(progmenu, MF_STRING, IDM_EXIT, "Exit\tAlt+F4");

    HMENU instmenu = CreatePopupMenu();
    AppendMenu(instmenu, MF_STRING, INST_PIANO, "Piano\tF2");
    AppendMenu(instmenu, MF_STRING, INST_GUITAR, "Guitar\tF3");
    AppendMenu(instmenu, MF_STRING, INST_DRUM, "Drum\tF4");

    HMENU menu = CreateMenu();
    AppendMenu(menu, MF_POPUP, (UINT_PTR)progmenu, "Synthesizer");
    AppendMenu(menu, MF_POPUP, (UINT_PTR)instmenu, "Instrument");

    SetMenu(hWnd, menu);
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    INT x, y;

    INT ksIndex = 0;
    BOOL keyFinded = false;

    switch (message)
    {
        case WM_SIZE:
        {
            for (int i = 0; i < 19; i++)
                pKey->isWhitePress[i] = false;
            for (int i = 0; i < 13; i++)
                pKey->isBlackPress[i] = false;
            InvalidateRect(hWnd, NULL, false);
        }
        break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);

            switch (wmId)
            {
            case MENU_SIGN:
                {
                    OnCheckMenuItem(hWnd, &isSign, MENU_SIGN);
                    InvalidateRect(hWnd, NULL, false);
                }
                break;
            case MENU_PEDAL:
                {
                    OnCheckMenuItem(hWnd, &isPedal, MENU_PEDAL);
                    InvalidateRect(hWnd, NULL, false);
                }
                break;
            case INST_PIANO:
                {
                    instrumentIndex = 0;
                    instrument = instrumentArray[0];
                    InvalidateRect(hWnd, NULL, false);
                }
                break;
            case INST_GUITAR:
                {
                    instrumentIndex = 1;
                    instrument = instrumentArray[1];
                    InvalidateRect(hWnd, NULL, false);
                }
                break;
            case INST_DRUM:
                {
                    instrumentIndex = 2;
                    instrument = instrumentArray[2];
                    InvalidateRect(hWnd, NULL, false);
                }
                break;
            case COLOR_WHITE:
                OnColorChange(hWnd, &pKey->hbrWhite);
                break;
            case COLOR_BLACK:
                OnColorChange(hWnd, &pKey->hbrBlack);
                break;
            case COLOR_GRAY:
                OnColorChange(hWnd, &pKey->hbrGray);
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
            pKey->hbrGray = CreateSolidBrush(RGB(127, 127, 127));
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
            while ((ksIndex < 37) && (!keyFinded))
            {
                if (keySimbols[ksIndex] == wParam)
                    keyFinded = true;
                else
                    ksIndex++;
            }
            if (keyFinded)
            {
                if (!(lParam & 0x40000000)) {
                    midiOutShortMsg(hmidi, 0x00000000 + instrument + 0xC0);
                    DWORD midimsg = 0x90 + (60 + keyArray[ksIndex] + Octave * 12) * 0x100 + SOUND_SPEED * 0x10000;
                    midiOutShortMsg(hmidi, midimsg);
                    if (isWhite(keyArray[ksIndex]))
                        pKey->isWhitePress[tranformFromAllKeysToWB[keyArray[ksIndex]]] = true;
                    else
                        pKey->isBlackPress[tranformFromAllKeysToWB[keyArray[ksIndex]]] = true;
                    InvalidateRect(hWnd, NULL, false);
                }
            }
            
            if ((wParam > 112) && (wParam < 116))
            {
                instrumentIndex = wParam - 113;
                instrument = instrumentArray[instrumentIndex];
                InvalidateRect(hWnd, NULL, false);
            }

            switch (wParam)
            {
            case VK_F1:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case VK_SHIFT:
                {
                    OnCheckMenuItem(hWnd, &isPedal, MENU_PEDAL);
                    InvalidateRect(hWnd, NULL, false);
                }
                break;
            case VK_TAB:
                {
                    OnCheckMenuItem(hWnd, &isSign, MENU_SIGN);
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
    case WM_KEYUP:
        {
            while ((ksIndex < 37) && (!keyFinded))
            {
                if (keySimbols[ksIndex] == wParam)
                    keyFinded = true;
                else
                    ksIndex++;
            }
            if (keyFinded) {
                if (!isPedal) {
                    DWORD midimsg = 0x90 + (60 + keyArray[ksIndex] + Octave * 12) * 0x100 + SOUND_SPEED * 0x10000;
                    midiOutShortMsg(hmidi, midimsg);
                }
                if (isWhite(keyArray[ksIndex]))
                    pKey->isWhitePress[tranformFromAllKeysToWB[keyArray[ksIndex]]] = false;
                else
                    pKey->isBlackPress[tranformFromAllKeysToWB[keyArray[ksIndex]]] = false;
                InvalidateRect(hWnd, NULL, false);
            }
        }
        break;
    case WM_MOUSEMOVE: 
        {
            if (mouse) 
            {
                BOOL mouseBWCopy = mouseBW;
                INT mouseIndexCopy = mouseIndex;
                x = LOWORD(lParam);
                y = HIWORD(lParam);
                if ((x < KEY_LEFT + 19 * WHITE_WIDTH) && (x > KEY_LEFT) && (y < WHITE_HEIGHT + KEY_TOP) && (y > KEY_TOP))
                { 
                    FindMouseKey(x, y);
                    if (mouseBW != mouseBWCopy || mouseIndex != mouseIndexCopy || mouseMiss)
                    {
                        mouseMiss = false;
                        StopSound(mouseIndexCopy, mouseBWCopy);
                        if (mouseBWCopy)
                            pKey->isWhitePress[mouseIndexCopy] = false;
                        else
                            pKey->isBlackPress[mouseIndexCopy - 1] = false;
                        PlaySound(mouseIndex, mouseBW);
                    }
                }
                else
                {
                    StopSound(mouseIndex, mouseBW);
                    if (mouseBW)
                        pKey->isWhitePress[mouseIndex] = false;
                    else
                        pKey->isBlackPress[mouseIndex - 1] = false;
                    mouseMiss = true;
                }
                InvalidateRect(hWnd, NULL, false);
            }
        }
        break;
    case WM_LBUTTONDOWN:
        {
            x = LOWORD(lParam);
            y = HIWORD(lParam);
            mouse = true;
            if ((x < KEY_LEFT + 19 * WHITE_WIDTH) && (x > KEY_LEFT) && (y < WHITE_HEIGHT + KEY_TOP) && (y > KEY_TOP))
            {
                FindMouseKey(x, y);
                PlaySound(mouseIndex, mouseBW);
                InvalidateRect(hWnd, NULL, false);
            }
        }
        break;
    case WM_LBUTTONUP:
        {
            if (mouse)
            {
                StopSound(mouseIndex, mouseBW);
                if (mouseBW)
                    pKey->isWhitePress[mouseIndex] = false;
                else
                    pKey->isBlackPress[mouseIndex - 1] = false;
                InvalidateRect(hWnd, NULL, false);
            }
            mouse = false;
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
            TextOut(buffDC, KEY_LEFT, 15, instruments[instrumentIndex], 6);SelectObject(buffDC, hFont);

            SelectObject(buffDC, hFont);
            TextOut(buffDC, KEY_LEFT + 19 * WHITE_WIDTH - 110, KEY_TOP + WHITE_HEIGHT + 15, "w84scaler", 9);

            if (isPedal) 
            {
                SelectObject(buffDC, hFont);
                TextOut(buffDC, KEY_LEFT + 19 * WHITE_WIDTH - 60, 15, "Pedal", 5);
            }
            
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

VOID FillKeyArrays()
{
    for (int i = 0; i < 17; i++)
        keyArray[i] = i;
    for (int i = 17; i < 22; i++)
        keyArray[i] = i - 5;
    for (int i = 22; i < 37; i++)
        keyArray[i] = i - 5;
}

BOOL isWhite(int index)
{
    if ((index == 1) || (index == 3) || (index == 6) || (index == 8) || (index == 10) || (index == 13) ||
        (index == 15) || (index == 18) || (index == 20) || (index == 22) || (index == 25) || (index == 27) || (index == 30))
        return false;
    else
        return true;
}

VOID OnCheckMenuItem(HWND hWnd, BOOL* flag, UINT menuItemCode)
{
    HMENU hmenu = GetMenu(hWnd);
    UINT style = (*flag ? MF_UNCHECKED : MF_CHECKED) | MF_BYCOMMAND;
    CheckMenuItem(hmenu, menuItemCode, style);
    (*flag) ^= true;
}

VOID OnColorChange(HWND hWnd, HBRUSH* keyBrush) 
{
    CHOOSECOLOR dlgBox = { 0 };
    dlgBox.hwndOwner = hWnd;
    dlgBox.lStructSize = sizeof(CHOOSECOLOR);
    dlgBox.lpTemplateName = "Customization";
    dlgBox.rgbResult = RGB(127, 127, 127);
    dlgBox.lpCustColors = (LPDWORD)acrCustClr;
    dlgBox.Flags = CC_FULLOPEN | CC_RGBINIT;
    if (ChooseColor(&dlgBox)) {
        COLORREF rgb = dlgBox.rgbResult;
        *keyBrush = CreateSolidBrush(rgb);
        InvalidateRect(hWnd, NULL, false);
    }
}

VOID FindMouseKey(INT x, INT y) 
{
    INT whiteIndex, blackIndex;

    whiteIndex = (x - KEY_LEFT) / WHITE_WIDTH;
    BOOL keyFinded = false;
    blackIndex = 0;
    if (y <= KEY_TOP + BLACK_HEIGHT)
    {
        while (!keyFinded && (blackIndex < 13))
        {
            if ((x >= pKey->blackRect[blackIndex].left) && (x <= pKey->blackRect[blackIndex].right))
            {
                keyFinded = true;
            }
            blackIndex++;
        }
        if (keyFinded)
        {
            pKey->isBlackPress[blackIndex - 1] = true;
            mouseBW = false;
            mouseIndex = blackIndex;
        }
        else
        {
            pKey->isWhitePress[whiteIndex] = true;
            mouseBW = true;
            mouseIndex = whiteIndex;
        }
    }
    else
    {
        pKey->isWhitePress[whiteIndex] = true;
        mouseBW = true;
        mouseIndex = whiteIndex;
    }
}

VOID PlaySound(INT index, INT colorBW)
{
    DWORD midimsg;
    midiOutShortMsg(hmidi, 0x00000000 + instrument + 0xC0);
    if (colorBW)
        midimsg = 0x90 + (60 + tranformFromWhiteToAll[index] + Octave * 12) * 0x100 + SOUND_SPEED * 0x10000;
    else
        midimsg = 0x90 + (60 + tranformFromBlackToAll[index - 1] + Octave * 12) * 0x100 + SOUND_SPEED * 0x10000;
    midiOutShortMsg(hmidi, midimsg);
}

VOID StopSound(INT index, INT colorBW)
{
    DWORD midimsg;
    if (!isPedal)
    {
        if (colorBW)
        {
            midimsg = 0x90 + (60 + tranformFromWhiteToAll[index] + Octave * 12) * 0x100 + 0 * 0x10000;
            midiOutShortMsg(hmidi, midimsg);
        }
        else
        {
            midimsg = 0x90 + (60 + tranformFromBlackToAll[index - 1] + Octave * 12) * 0x100 + 0 * 0x10000;
            midiOutShortMsg(hmidi, midimsg);
        }
    }
}