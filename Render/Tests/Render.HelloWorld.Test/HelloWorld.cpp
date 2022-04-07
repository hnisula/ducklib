#include <cstdint>
#include <exception>
#include <Windows.h>
#include <Render/Core/D3D12/D3D12Api.h>

using namespace DuckLib;
using namespace Render;

constexpr int WND_WIDTH = 800;
constexpr int WND_HEIGHT = 600;

bool runTest = true;
HWND window;

IDevice* api = new D3D12Api();
ISwapChain* swapChain;
ICommandBuffer* cmdBuffer;

void InitRender(uint32_t width, uint32_t height, HWND windowHandle)
{
	swapChain = api->CreateSwapChain(width, height, Format::R8G8B8A8_UNORM, 2, windowHandle);
	cmdBuffer = api->CreateCommandBuffer();
}

void RenderFrame()
{
	cmdBuffer->Reset();

	cmdBuffer->Transition(
		swapChain->GetCurrentBuffer(),
		ResourceState::PRESENT,
		ResourceState::RENDER_TARGET);

	float clearColor[] = { 0.0f, 1.0f, 0.0f, 1.0f };
	Rect scissorRect{ 0, 0, WND_WIDTH, WND_HEIGHT };
	Viewport viewport{ 0.0f, 0.0f, (float)WND_WIDTH, (float)WND_HEIGHT, 0, 1 };

	cmdBuffer->SetScissorRect(scissorRect);
	cmdBuffer->SetViewport(viewport);
	cmdBuffer->SetRT(swapChain->GetCurrentBuffer());
	cmdBuffer->Clear(swapChain->GetCurrentBuffer(), clearColor);

	cmdBuffer->

	cmdBuffer->Transition(swapChain->GetCurrentBuffer(), ResourceState::RENDER_TARGET, ResourceState::PRESENT);

	cmdBuffer->Close();

	api->ExecuteCommandBuffers(&cmdBuffer, 1);

	swapChain->Present();

	api->SignalCompletion(swapChain);
	swapChain->WaitForFrame();

	Sleep(20);
}

void DestroyRender()
{
	api->DestroySwapChain(swapChain);
}

LRESULT __stdcall WndProc(HWND hwnd, uint32_t msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CLOSE:
		DestroyWindow(window);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		runTest = false;
		return 0;
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
	wndClass.hIcon = nullptr;
	wndClass.hIconSm = nullptr;
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

	HWND newWindow = CreateWindowExA(
		0,
		"renderWindowClass",
		"HelloWorld",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		windowWidth,
		windowHeight,
		nullptr,
		nullptr,
		hInstance,
		nullptr
	);

	if (newWindow == nullptr)
		throw std::exception("maddafakka");

	return newWindow;
}

int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, char* cmdLine, int32_t cmdShow)
{
	const uint32_t width = WND_WIDTH;
	const uint32_t height = WND_HEIGHT;
	window = InitWindow(width, height, hInstance);
	MSG msg;

	InitRender(width, height, window);

	while (runTest)
	{
		while (PeekMessage(&msg, window, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		RenderFrame();
	}

	DestroyRender();

	return 0;
}
