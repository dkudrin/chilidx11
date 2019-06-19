#include <Windows.h>

LRESULT CALLBACK WndProc(
	HWND hWnd, // handle
	UINT msg, // number of the message (message Id)
	WPARAM wParam, // some param (for example - exit code, if msg is WM_QUIT)
	LPARAM lParam)
{
	switch (msg) {
		case WM_CLOSE: // Google "list of window messages" - wine HQ
			PostQuitMessage(69); // 69 - exit code
			break;
		case WM_KEYDOWN:
			if (wParam == 'F')
			{
				SetWindowTextA(hWnd, "Dkudrin window title");
			}
			break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam); // Default window Procedure
}

int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nShowCmd) 
{
	// register window class
	const auto pClassName = "hw3dbutts";

	WNDCLASSEX wc = {0};
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = WndProc; // Каждое окно программы будет иметь свой укзатель на процедуру
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = nullptr;
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;	
	wc.lpszClassName = pClassName;
	wc.hIconSm = nullptr;
	RegisterClassEx(&wc);

	// create window
	HWND hWnd = CreateWindowEx(
		0, //DWORD dwExStyle,			// улучшенный стиль окна
		pClassName, //LPCTSTR lpClassName,		// указатель на зарегистрированное имя класса
		"Happy hard window", //LPCTSTR lpWindowName,		// указатель на имя окна
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, //DWORD dwStyle,			// стиль окна
		200, //int x,				// горизонтальная позиция окна
		200,//int y,				// вертикальная позиция окна
		640,//int nWidth,			// ширина окна
		480, //int nHeight,			// высота окна
		nullptr, //HWND hWndParent,			// дескриптор родительского или окна собственника
		nullptr, //HMENU hMenu,			// дескриптор меню или идентификатор дочернего окна
		hInstance, //HINSTANCE hInstance,		// дескриптор экземпляра прикладной программы	
		nullptr //LPVOID lpParam 			// указатель на данные создания окна
	);

	// show window 
	ShowWindow(hWnd, SW_SHOW);

	// message pump
	MSG msg; // указатель на сообщение с типом данных Структура (в MSDN есть описание полей что в ней храниться https://docs.microsoft.com/en-us/windows/desktop/api/winuser/ns-winuser-tagmsg)
	BOOL gResult;
	while ((gResult = GetMessage(
		&msg, // указатель на структуру которая будет наполнена сообщением от Windows по состоянию окна 
		nullptr, // указатель на окно от которого получать сообщения, если NULL - тогда он будет получать сообщения от всех окон
		0, // фильтр типов сообщений которые мы хотим получать
		0 // тоже фильтр типов сообщений которые мы хотим получать
	)) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	if (gResult == -1)
	{
		return -1;
	}
	else
	{
		return msg.wParam;
	}
}