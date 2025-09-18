#pragma once
#include<Windows.h>



LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


class Window
{
public:
	
	Window();
	Window(LPCWSTR className, WNDPROC lpfnWndProc, LPCWSTR title, DWORD s, int xCoord, int yCoord, int w, int h);
	~Window();

	HWND getHandle();

	bool ProcessMessages();

private:
	LPCWSTR cName;
	HINSTANCE mInstance;
	HWND mHandle;
};

