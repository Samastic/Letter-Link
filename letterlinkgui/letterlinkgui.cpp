// letterlinkgui.cpp : Defines the entry point for the application.

#include <windowsx.h>
#include "framework.h"
#include "letterlinkgui.h"
#include "letterlink.h"

#define MAX_LOADSTRING 100
#define IDM_ADDLINE 1101
#define IDM_REMOVELINE 1102

struct LineEditState {
    int lineIndex;  // Index of the line being edited
    int charIndex;  // Index of the character position in the line
};

LineEditState editState = { -1, 0 };  // Initialize with no line selected

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
LetterLink letterLink;                          // main gameplay object
std::vector<std::string> wordset = letterLink.getWordset();
int linecount = 0;

// Forward declarations of functions included in this code module:
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

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_LETTERLINKGUI, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LETTERLINKGUI));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LETTERLINKGUI));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_LETTERLINKGUI);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Store instance handle in our global variable

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    // Create the "+" button to add new lines
    HWND hAddLineButton = CreateWindow(
        L"BUTTON",  // Predefined class; Unicode assumed
        L"+",       // Button text
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles
        10,         // x position
        10,         // y position
        30,         // Button width
        30,         // Button height
        hWnd,       // Parent window
        (HMENU)IDM_ADDLINE, // Button ID
        hInstance,
        NULL);

    // Create the "-" button to remove lines
    HWND hRemoveLineButton = CreateWindow(
        L"BUTTON",  // Predefined class; Unicode assumed
        L"-",       // Button text
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles
        50,         // x position
        10,         // y position
        30,         // Button width
        30,         // Button height
        hWnd,       // Parent window
        (HMENU)IDM_REMOVELINE, // Button ID for remove line
        hInstance,
        NULL);

    if (!hAddLineButton || !hRemoveLineButton) {
        MessageBox(hWnd, L"Button creation failed!", L"Error", MB_OK | MB_ICONERROR);
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}




//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case IDM_ADDLINE:
            if (letterLink.getWordset().size() < 10) {
                linecount++;
                letterLink.insertWordAtPosition("     ", linecount);
                editState.lineIndex = 1;  // Set focus to the new line
                editState.charIndex = 0;  // Start at the beginning of the new line
                InvalidateRect(hWnd, NULL, TRUE);
            }
            break;
        case IDM_REMOVELINE:
            if (letterLink.getWordset().size() > 2) {  // Only allow removal if there are more than 2 words
                letterLink.removeWordAtPosition(linecount);
                linecount--;
                editState.lineIndex = -1;  // Reset focus
                InvalidateRect(hWnd, NULL, TRUE);
            }
            else {
                MessageBox(hWnd, L"Cannot remove base words!", L"Warning", MB_OK | MB_ICONWARNING);
            }
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;

    case WM_LBUTTONDOWN:  // Handle mouse click to select a box
    {
        int xPos = GET_X_LPARAM(lParam);
        int yPos = GET_Y_LPARAM(lParam);

        int startX = 50;  // Starting X position for drawing
        int startY = 50;  // Starting Y position for drawing
        int boxSize = 50; // Size of each letter box
        int padding = 10; // Padding between boxes

        // Determine which box was clicked
        int rowIndex = (yPos - startY) / (boxSize + padding);
        int colIndex = (xPos - startX) / (boxSize + padding);

        if (rowIndex >= 0 && rowIndex < letterLink.getWordset().size() &&
            colIndex >= 0 && colIndex < 5) {
            editState.lineIndex = rowIndex;
            editState.charIndex = colIndex;
            SetFocus(hWnd);  // Ensure the window retains focus for keyboard input
            InvalidateRect(hWnd, NULL, TRUE);  // Request window repaint
        }
    }
    break;

    case WM_CHAR:  // Handle character input
    {
        if (editState.lineIndex != -1 && editState.lineIndex < letterLink.getWordset().size()) {
            char typedChar = (char)wParam;
            if (isalpha(typedChar) && editState.charIndex < 5) {
                letterLink.setCharAt(editState.lineIndex, editState.charIndex, typedChar);
                editState.charIndex++;

                // Prevent moving to the next line; stay on the current line
                if (editState.charIndex >= 5) {
                    editState.charIndex = 5;  // Stay at the end of the current line
                }

                InvalidateRect(hWnd, NULL, TRUE);  // Redraw to reflect changes
            }
        }
    }
    break;

    case WM_KEYDOWN:  // Handle backspace
    {
        if (wParam == VK_BACK && editState.lineIndex != -1 && editState.charIndex > 0) {
            letterLink.setCharAt(editState.lineIndex, editState.charIndex - 1, ' ');
            editState.charIndex--;
            InvalidateRect(hWnd, NULL, TRUE);  // Redraw to reflect changes
        }
    }
    break;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        // Existing logic for drawing wordset grid
        std::vector<std::string> wordset = letterLink.getWordset();
        int startX = 50;  // Starting X position for drawing
        int startY = 50;  // Starting Y position for drawing
        int boxSize = 50; // Size of each letter box
        int padding = 10; // Padding between boxes

        HFONT hFont = CreateFont(
            30,                        // Height of font
            0,                         // Average character width
            0,                         // Angle of escapement
            0,                         // Base-line orientation angle
            FW_BOLD,                   // Font weight
            FALSE,                     // Italic attribute option
            FALSE,                     // Underline attribute option
            FALSE,                     // Strikeout attribute option
            DEFAULT_CHARSET,           // Character set identifier
            OUT_DEFAULT_PRECIS,        // Output precision
            CLIP_DEFAULT_PRECIS,       // Clipping precision
            DEFAULT_QUALITY,           // Output quality
            DEFAULT_PITCH | FF_SWISS,  // Pitch and family
            L"Arial");                 // Typeface name

        HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);
        SetBkMode(hdc, TRANSPARENT);  // Set background mode to transparent for text

        // Drawing the grid of words
        for (int i = 0; i < wordset.size(); ++i)
        {
            std::string word = wordset[i];
            for (int j = 0; j < 5; ++j)
            {
                int x = startX + (j * (boxSize + padding));
                int y = startY + (i * (boxSize + padding));

                Rectangle(hdc, x, y, x + boxSize, y + boxSize);

                if (editState.lineIndex == i && editState.charIndex == j) {
                    // Highlight the selected box
                    HBRUSH hBrush = CreateSolidBrush(RGB(200, 200, 255));

                    RECT rect = { x, y, x + boxSize, y + boxSize };  // Declare and initialize a RECT object
                    FillRect(hdc, &rect, hBrush);  // Use the address of the RECT object

                    DeleteObject(hBrush);
                }

                if (!isspace(word[j])) {
                    wchar_t letter[2] = { (wchar_t)word[j], L'\0' };
                    SIZE textSize;
                    GetTextExtentPoint32(hdc, letter, 1, &textSize);
                    int textX = x + (boxSize - textSize.cx) / 2;
                    int textY = y + (boxSize - textSize.cy) / 2;

                    TextOut(hdc, textX, textY, letter, 1);
                }
            }
        }

        // Prepare the string to be displayed in the bottom right corner
        std::string wordListDisplay = "Word List: ";
        for (const auto& word : wordset) {
            wordListDisplay += "\"" + word + "\", ";
        }

        // Remove the last comma and space
        if (!wordset.empty()) {
            wordListDisplay.pop_back();
            wordListDisplay.pop_back();
        }

        // Convert the final string to wide string for Windows API
        std::wstring wideWordListDisplay(wordListDisplay.begin(), wordListDisplay.end());

        // Calculate the position to display the word list in the bottom right
        RECT clientRect;
        GetClientRect(hWnd, &clientRect);

        int textX = clientRect.right - 1000;  // Example offset for x position
        int textY = clientRect.bottom - 50;  // Example offset for y position

        // Display the word list in the bottom right corner
        TextOut(hdc, textX, textY, wideWordListDisplay.c_str(), wideWordListDisplay.size());

        // Clean up
        SelectObject(hdc, hOldFont);
        DeleteObject(hFont);
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



// Message handler for about box.
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
