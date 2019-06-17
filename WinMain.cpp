#include <Windows.h>

int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nShowCmd) 
{
	// register window class
	const auto pClassName = "hw3dbutts";

	WNDCLASSEX wc = {0};
	wc.cbSize = sizeof(WNDCLASS);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = DefWindowProc;
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
		640, //int nHeight,			// высота окна
		nullptr, //HWND hWndParent,			// дескриптор родительского или окна собственника
		nullptr, //HMENU hMenu,			// дескриптор меню или идентификатор дочернего окна
		hInstance, //HINSTANCE hInstance,		// дескриптор экземпляра прикладной программы	
		nullptr //LPVOID lpParam 			// указатель на данные создания окна
	);

	// show window 
	ShowWindow(hWnd, SW_SHOW);
	LPMSG lpMsg;
	GetMessage(
		lpMsg, // указатель на структуру которая будет наполнена сообщением от Windows по состоянию окна 
		NULL,
		
		);
	while (true);
	return 0;
}