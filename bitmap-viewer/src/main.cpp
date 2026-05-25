#include <windows.h>
#include <commdlg.h>
#include <iostream>
#include <string>

// Global handle for the loaded bitmap
HBITMAP g_hBitmap = NULL;

// Window Procedure declaration
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// Helper function to show native Win32 File Open Dialog to pick a BMP
std::string PromptForBMPFile() {
    OPENFILENAMEA ofn;
    char szFile[260] = {0};
    
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "Bitmap Files (*.bmp)\0*.bmp\0All Files (*.*)\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileNameA(&ofn) == TRUE) {
        return std::string(ofn.lpstrFile);
    }
    return std::string();
}

int main(int argc, char* argv[]) {
    std::cout << "===========================================\n";
    std::cout << "  Win32 Bitmap Viewer (C++98)              \n";
    std::cout << "===========================================\n\n";

    std::string imagePath;

    // 1. Determine image path (command line or file dialog)
    if (argc > 1) {
        imagePath = argv[1];
        std::cout << "Loading image from command-line: " << imagePath << "\n";
    } else {
        std::cout << "No file path provided. Opening File Open Dialog...\n";
        imagePath = PromptForBMPFile();
        if (imagePath.empty()) {
            std::cout << "No file selected. Exiting.\n";
            return 0;
        }
        std::cout << "Selected image: " << imagePath << "\n";
    }

    // 2. Load the BMP file
    g_hBitmap = (HBITMAP)LoadImageA(
        NULL,
        imagePath.c_str(),
        IMAGE_BITMAP,
        0, 0,
        LR_LOADFROMFILE | LR_CREATEDIBSECTION
    );

    if (g_hBitmap == NULL) {
        std::cerr << "Error: Failed to load bitmap file: " << imagePath << "\n";
        MessageBoxA(NULL, "Failed to load bitmap image.", "Error", MB_OK | MB_ICONERROR);
        return 1;
    }

    // Get bitmap properties to size the window
    BITMAP bmp;
    GetObject(g_hBitmap, sizeof(BITMAP), &bmp);
    std::cout << "Successfully loaded BMP: " << bmp.bmWidth << "x" << bmp.bmHeight << " pixels\n";

    // 3. Register the Win32 window class
    HINSTANCE hInstance = GetModuleHandle(NULL);
    const char* className = "BitmapViewerClass";

    WNDCLASSEXA wcx;
    ZeroMemory(&wcx, sizeof(wcx));
    wcx.cbSize = sizeof(wcx);
    wcx.style = CS_HREDRAW | CS_VREDRAW;
    wcx.lpfnWndProc = WndProc;
    wcx.hInstance = hInstance;
    wcx.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcx.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcx.lpszClassName = className;

    if (!RegisterClassExA(&wcx)) {
        std::cerr << "Error: Failed to register window class.\n";
        DeleteObject(g_hBitmap);
        return 1;
    }

    // Adjust window dimensions to achieve a 1:1 scale client area for the image
    // Sensible maximum bounds (e.g. 1024x768) if the image is too large
    int clientWidth = bmp.bmWidth;
    int clientHeight = bmp.bmHeight;
    if (clientWidth < 100) clientWidth = 100;
    if (clientHeight < 100) clientHeight = 100;
    
    RECT wr = { 0, 0, clientWidth, clientHeight };
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);
    int windowWidth = wr.right - wr.left;
    int windowHeight = wr.bottom - wr.top;

    // Create the Window
    std::string windowTitle = "Bitmap Viewer - " + imagePath;
    HWND hWnd = CreateWindowExA(
        0,
        className,
        windowTitle.c_str(),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        windowWidth, windowHeight,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (hWnd == NULL) {
        std::cerr << "Error: Failed to create window.\n";
        DeleteObject(g_hBitmap);
        return 1;
    }

    // Show and paint the Window
    ShowWindow(hWnd, SW_SHOWDEFAULT);
    UpdateWindow(hWnd);

    // 4. Message loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

// Window Procedure implementation
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

            if (g_hBitmap != NULL) {
                BITMAP bmp;
                GetObject(g_hBitmap, sizeof(BITMAP), &bmp);

                // Create a memory device context compatible with the window's DC
                HDC hdcMem = CreateCompatibleDC(hdc);
                HGDIOBJ hOld = SelectObject(hdcMem, g_hBitmap);

                // Blit from Memory DC to Window DC (1:1 scale, top-left aligned at 0,0)
                BitBlt(
                    hdc,
                    0, 0,
                    bmp.bmWidth,
                    bmp.bmHeight,
                    hdcMem,
                    0, 0,
                    SRCCOPY
                );

                // Clean up Memory DC and restore old selection
                SelectObject(hdcMem, hOld);
                DeleteDC(hdcMem);
            } else {
                RECT rect;
                GetClientRect(hWnd, &rect);
                DrawTextA(hdc, "No image loaded.", -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            }

            EndPaint(hWnd, &ps);
            break;
        }

        case WM_DESTROY:
            // Release GDI bitmap handle on window closure
            if (g_hBitmap != NULL) {
                DeleteObject(g_hBitmap);
                g_hBitmap = NULL;
            }
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
