#include <Windows.h>

LRESULT __stdcall WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {}

    return DefWindowProc(hWnd, message, wParam, lParam);
}

int __stdcall WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, char* cmdLine, int cmdShow) {
    return 0;
}
