#include <cstdint>
#include <exception>
#include <Windows.h>
#include <Render/Core/D3D12/D3D12Api.h>

using namespace DuckLib;
using namespace Render;

bool runTest = true;

IApi* api = new D3D12Api();


void InitRender()
{
	
}

LRESULT __stdcall WndProc(HWND hwnd, uint32_t msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
}

HWND InitWindow(uint32_t width, uint32_t height, HINSTANCE hInstance)
{
	WNDCLASSEXA wndClass;
	memset(&wndClass, 0, sizeof(wndClass));

	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hIcon = 0;
	wndClass.hIconSm = 0;
	wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wndClass.hInstance = hInstance;
	wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wndClass.lpszMenuName = nullptr;
	wndClass.lpszClassName = "renderWindowClass";

	RegisterClassExA(&wndClass);

	RECT clientAreaRect;
	clientAreaRect.left = 0;
	clientAreaRect.top = 0;
	clientAreaRect.right = width;
	clientAreaRect.bottom = height;
	AdjustWindowRectEx(&clientAreaRect, WS_OVERLAPPEDWINDOW, false, 0);

	uint32_t windowWidth = clientAreaRect.right - clientAreaRect.left;
	uint32_t windowHeight = clientAreaRect.bottom - clientAreaRect.top;

	HWND hwnd = CreateWindowExA(0,
		"renderWindowClass",
		"",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		windowWidth,
		windowHeight,
		nullptr,
		nullptr,
		hInstance,
		nullptr
	);

	if (hwnd == nullptr)
		throw std::exception("maddafakka");

	return hwnd;
}

int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, char* cmdLine, int32_t cmdShow)
{
	HWND window = InitWindow(800, 600, hInstance);
	MSG msg;
	
	while (PeekMessage(&msg, window, 0, 0, PM_REMOVE))
	{
		
	}

	return 0;
}