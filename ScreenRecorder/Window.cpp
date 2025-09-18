#include "Window.h"

Window::Window() : mInstance (GetModuleHandle(nullptr))
{
	const wchar_t* className = L"My Window Class";
	cName = className;
	WNDCLASS wClass = {};
	wClass.lpszClassName = className;
	wClass.hInstance = mInstance;
	wClass.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wClass.lpfnWndProc = WindowProc;

	RegisterClass(&wClass);

	DWORD style = NULL;
	RECT rect;

	int width = 640;
	int height = 480;

	//location on screen where window appears
	rect.left = 250;
	rect.top = 250;
	rect.right = rect.left + width;
	rect.bottom = rect.top + height;

	AdjustWindowRect(&rect, style, false);

	mHandle = CreateWindowEx(
		0,
		className,
		L"Title",
		style,
		rect.left,
		rect.top,
		rect.right - rect.left,
		rect.bottom - rect.top,
		NULL,
		NULL,
		mInstance,
		NULL
	);

	ShowWindow(mHandle, SW_SHOW);

}

Window::Window(LPCWSTR className, WNDPROC lpfnWndProc, LPCWSTR title, DWORD s, int xCoord, int yCoord, int w, int h, bool transparent) : mInstance(GetModuleHandle(nullptr))
{

	cName = className;
	WNDCLASS wClass = {};
	wClass.lpszClassName = className;
	wClass.hInstance = mInstance;
	wClass.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wClass.lpfnWndProc = lpfnWndProc;

	RegisterClass(&wClass);

	DWORD style = s;
	RECT rect;

	int width = w;
	int height = h;

	//location on screen where window appears
	rect.left = xCoord;
	rect.top = yCoord;
	rect.right = rect.left + width;
	rect.bottom = rect.top + height;

	AdjustWindowRect(&rect, style, false);

	
	mHandle = CreateWindowEx(
		WS_EX_TOPMOST | WS_EX_LAYERED,
		wClass.lpszClassName,
		title,
		style,
		rect.left,
		rect.top,
		rect.right - rect.left,
		rect.bottom - rect.top,
		NULL,
		NULL,
		wClass.hInstance,
		NULL
	);

	if (transparent)
		SetLayeredWindowAttributes(mHandle, RGB(0, 0, 0), 128, LWA_ALPHA);
	else
		SetLayeredWindowAttributes(mHandle, RGB(0, 0, 0), 255, LWA_ALPHA);

	ShowWindow(mHandle, SW_SHOW);

}



Window::~Window()
{
	DestroyWindow(mHandle);
	UnregisterClass(cName, mInstance);
	
}

HWND Window::getHandle()
{
	return mHandle;
}

bool Window::ProcessMessages()
{
	MSG msg = {};

	while (PeekMessage(&msg, nullptr, 0u, 0u, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
			return false;


		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return true;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
