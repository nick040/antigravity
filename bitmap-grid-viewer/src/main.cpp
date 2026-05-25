#include <windows.h>
#include <commdlg.h>
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>

// Grid Point Structure
struct GridPoint {
    double x;
    double y;
};

// Global variables
HBITMAP g_hBitmap = NULL;
std::vector<std::vector<GridPoint> > g_grid;
int g_gridRows = 15;
int g_gridCols = 15;
double g_maxOffsetPercent = 0.05; // 5% default offset of grid cell spacing
bool g_showGrid = true;
int g_bmpWidth = 0;
int g_bmpHeight = 0;

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

// Function to generate the grid points with random interior offsets
void GenerateGrid() {
    if (g_bmpWidth <= 0 || g_bmpHeight <= 0) return;
    
    if (g_gridRows < 2) g_gridRows = 2;
    if (g_gridCols < 2) g_gridCols = 2;

    // Resize grid 2D vector
    g_grid.resize(g_gridRows);
    for (int r = 0; r < g_gridRows; ++r) {
        g_grid[r].resize(g_gridCols);
    }

    double spacingX = static_cast<double>(g_bmpWidth - 1) / (g_gridCols - 1);
    double spacingY = static_cast<double>(g_bmpHeight - 1) / (g_gridRows - 1);

    for (int r = 0; r < g_gridRows; ++r) {
        for (int c = 0; c < g_gridCols; ++c) {
            // Nominal coordinates on the regular rectangular grid
            double nominalX = c * spacingX;
            double nominalY = r * spacingY;

            double offsetX = 0.0;
            double offsetY = 0.0;

            // Apply offsets only to interior points (not on the edges of the image)
            if (r > 0 && r < g_gridRows - 1 && c > 0 && c < g_gridCols - 1) {
                double maxOffsetX = spacingX * g_maxOffsetPercent;
                double maxOffsetY = spacingY * g_maxOffsetPercent;

                // Random value in range [-1.0, 1.0]
                offsetX = ((static_cast<double>(rand()) / RAND_MAX) * 2.0 - 1.0) * maxOffsetX;
                offsetY = ((static_cast<double>(rand()) / RAND_MAX) * 2.0 - 1.0) * maxOffsetY;
            }

            g_grid[r][c].x = nominalX + offsetX;
            g_grid[r][c].y = nominalY + offsetY;
        }
    }
}

// Draw the grid lines using thin light-grey pen
void DrawGrid(HDC hdc) {
    if (!g_showGrid || g_grid.empty()) return;

    // Create thin light-grey pen (width = 1, RGB(200, 200, 200))
    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(200, 200, 200));
    HGDIOBJ hOldPen = SelectObject(hdc, hPen);

    // Draw horizontal connections
    for (int r = 0; r < g_gridRows; ++r) {
        for (int c = 0; c < g_gridCols - 1; ++c) {
            int x1 = static_cast<int>(g_grid[r][c].x + 0.5);
            int y1 = static_cast<int>(g_grid[r][c].y + 0.5);
            int x2 = static_cast<int>(g_grid[r][c + 1].x + 0.5);
            int y2 = static_cast<int>(g_grid[r][c + 1].y + 0.5);

            MoveToEx(hdc, x1, y1, NULL);
            LineTo(hdc, x2, y2);
        }
    }

    // Draw vertical connections
    for (int r = 0; r < g_gridRows - 1; ++r) {
        for (int c = 0; c < g_gridCols; ++c) {
            int x1 = static_cast<int>(g_grid[r][c].x + 0.5);
            int y1 = static_cast<int>(g_grid[r][c].y + 0.5);
            int x2 = static_cast<int>(g_grid[r + 1][c].x + 0.5);
            int y2 = static_cast<int>(g_grid[r + 1][c].y + 0.5);

            MoveToEx(hdc, x1, y1, NULL);
            LineTo(hdc, x2, y2);
        }
    }

    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);
}

int main(int argc, char* argv[]) {
    std::cout << "===========================================\n";
    std::cout << "  Win32 Bitmap Viewer (C++98 Standard)     \n";
    std::cout << "===========================================\n\n";

    std::string imagePath;

    // 1. Determine image path and grid options from command-line or file dialog
    if (argc > 1) {
        imagePath = argv[1];
        std::cout << "Loading image from command-line: " << imagePath << "\n";
        
        // Parse optional grid parameters: [rows] [cols] [offset_percentage]
        if (argc > 2) {
            g_gridRows = atoi(argv[2]);
            if (g_gridRows < 2) g_gridRows = 2;
        }
        if (argc > 3) {
            g_gridCols = atoi(argv[3]);
            if (g_gridCols < 2) g_gridCols = 2;
        }
        if (argc > 4) {
            g_maxOffsetPercent = atof(argv[4]);
            if (g_maxOffsetPercent < 0.0) g_maxOffsetPercent = 0.0;
            if (g_maxOffsetPercent > 0.5) g_maxOffsetPercent = 0.5; // Cap at 50% to prevent grid crossing
        }
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
    g_bmpWidth = bmp.bmWidth;
    g_bmpHeight = bmp.bmHeight;
    std::cout << "Successfully loaded BMP: " << g_bmpWidth << "x" << g_bmpHeight << " pixels\n";
    std::cout << "Grid setup: " << g_gridRows << "x" << g_gridCols << " (Max Offset: " << g_maxOffsetPercent * 100 << "%)\n";
    std::cout << "Controls:\n";
    std::cout << "  - G: Toggle grid visibility\n";
    std::cout << "  - R / Space: Regenerate random offsets\n";
    std::cout << "  - Up / Down: Increase / decrease rows\n";
    std::cout << "  - Right / Left: Increase / decrease columns\n\n";

    // Seed RNG and generate initial grid
    srand(static_cast<unsigned int>(time(NULL)));
    GenerateGrid();

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
    int clientWidth = g_bmpWidth;
    int clientHeight = g_bmpHeight;
    if (clientWidth < 100) clientWidth = 100;
    if (clientHeight < 100) clientHeight = 100;
    
    RECT wr = { 0, 0, clientWidth, clientHeight };
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);
    int windowWidth = wr.right - wr.left;
    int windowHeight = wr.bottom - wr.top;

    // Create the Window
    std::string windowTitle = "Bitmap Viewer with Grid - " + imagePath;
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
                // Create a memory device context compatible with the window's DC
                HDC hdcMem = CreateCompatibleDC(hdc);
                HGDIOBJ hOld = SelectObject(hdcMem, g_hBitmap);

                // Blit from Memory DC to Window DC (1:1 scale, top-left aligned at 0,0)
                BitBlt(
                    hdc,
                    0, 0,
                    g_bmpWidth,
                    g_bmpHeight,
                    hdcMem,
                    0, 0,
                    SRCOPY
                );

                // Clean up Memory DC and restore old selection
                SelectObject(hdcMem, hOld);
                DeleteDC(hdcMem);

                // Draw the grid overlay on top of the bitmap
                DrawGrid(hdc);
            } else {
                RECT rect;
                GetClientRect(hWnd, &rect);
                DrawTextA(hdc, "No image loaded.", -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            }

            EndPaint(hWnd, &ps);
            break;
        }

        case WM_KEYDOWN: {
            bool needsRepaint = false;
            switch (wParam) {
                case 'G':
                case 'g':
                    g_showGrid = !g_showGrid;
                    needsRepaint = true;
                    std::cout << "Toggle grid: " << (g_showGrid ? "ON" : "OFF") << "\n";
                    break;
                case 'R':
                case 'r':
                case VK_SPACE:
                    GenerateGrid();
                    needsRepaint = true;
                    std::cout << "Regenerated grid offsets.\n";
                    break;
                case VK_UP:
                    g_gridRows++;
                    GenerateGrid();
                    needsRepaint = true;
                    std::cout << "Grid size updated: " << g_gridRows << "x" << g_gridCols << "\n";
                    break;
                case VK_DOWN:
                    if (g_gridRows > 2) {
                        g_gridRows--;
                        GenerateGrid();
                        needsRepaint = true;
                        std::cout << "Grid size updated: " << g_gridRows << "x" << g_gridCols << "\n";
                    }
                    break;
                case VK_RIGHT:
                    g_gridCols++;
                    GenerateGrid();
                    needsRepaint = true;
                    std::cout << "Grid size updated: " << g_gridRows << "x" << g_gridCols << "\n";
                    break;
                case VK_LEFT:
                    if (g_gridCols > 2) {
                        g_gridCols--;
                        GenerateGrid();
                        needsRepaint = true;
                        std::cout << "Grid size updated: " << g_gridRows << "x" << g_gridCols << "\n";
                    }
                    break;
            }
            if (needsRepaint) {
                // Request window repaint (FALSE parameter prevents background erase to eliminate flicker)
                InvalidateRect(hWnd, NULL, FALSE);
            }
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