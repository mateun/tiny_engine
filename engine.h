#include <iostream>

#define WINDOWING
#ifdef WINDOWING
#include <Windows.h>
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInst, LPSTR cmdLine, int cmdShow)
{
    int width = 800;
    int height = 600;

    const std::wstring className = L"GameWindowClass";

    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.lpfnWndProc = DefWindowProc; 
    wc.hInstance = hInstance;
    wc.lpszClassName = className.c_str();
    wc.hCursor = LoadCursorW(nullptr, reinterpret_cast<LPCWSTR>(IDC_ARROW));
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    RegisterClassExW(&wc);

    RECT clientRect = {0, 0, width, height};
    DWORD style = WS_OVERLAPPEDWINDOW;
    AdjustWindowRect(&clientRect, style, FALSE); // FALSE = no menu bar
    int adjustedWidth = clientRect.right - clientRect.left;
    int adjustedHeight = clientRect.bottom - clientRect.top;

    HWND hwnd = CreateWindowExW(
        0,
        className.c_str(),
        L"GameWindow",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, adjustedWidth, adjustedHeight,
        nullptr,
        nullptr,
        hInstance,
        nullptr
    );
	
	
    if (hwnd) {
        ShowWindow(hwnd, cmdShow);
        UpdateWindow(hwnd);
    }
    
    bool running = true;
    while (running) {

        MSG msg;
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                running = false;
            }
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }

    }
}



#endif
