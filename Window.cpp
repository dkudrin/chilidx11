#include "Window.h"

// Window Class Stuff
Window::WindowClass Window::WindowClass::wndClass;

Window::WindowClass::WindowClass() noexcept // конструктор
	:
	hInst(GetModuleHandle(nullptr)) // initialization list - список инициализаций полей значениями до конструктора
									// GetModuleHandle -  получение hadle от instance окна
									// если бы не эта инициализация пришлось бы передавать в конструктор параметр

{
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = HandleMsgSetup;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetInstance();
	wc.hIcon = nullptr;
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = GetName();
	wc.hIconSm = nullptr;
	RegisterClassEx(&wc); // Регистрация инстанса окна
}

Window::WindowClass::~WindowClass() // деструктор
{
	UnregisterClass(wndClassName, GetInstance()); // Разрегистрация инстанса окна
}

const char * Window::WindowClass::GetName() noexcept
{
	return wndClassName; // возавращает имя окна
}

HINSTANCE Window::WindowClass::GetInstance() noexcept
{
	return wndClass.hInst; // Возвращает просто инстанс окна
}

// Window
Window::Window(int width, int height, const char* name) noexcept { // конструктор окна
	// calculate window size based on desired client region size
	RECT wr;
	wr.left = 100;
	wr.right = width + wr.left;
	wr.top = 100;
	wr.bottom = height + wr.top;
	DWORD wndStyle = WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU;
	AdjustWindowRect(&wr, wndStyle, FALSE);
	// create window & get hWnd
	hWnd = CreateWindow(
		WindowClass::GetName(), // class name
		name, // name of the window
		wndStyle, // style of the window
		CW_USEDEFAULT, // starting positions of the window - let Windows deside where it must be
		CW_USEDEFAULT,
		wr.right - wr.left,
		wr.bottom - wr.top,
		nullptr,
		nullptr,
		WindowClass::GetInstance(),
		this // !!! pointer to our window instance
	);
	// show window
	ShowWindow(hWnd, SW_SHOWDEFAULT);
}

Window::~Window()
{
	DestroyWindow(hWnd);
}

LRESULT WINAPI Window::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	// use create parameter passed in form CreateWindow() to store window class pointer
	if (msg = WM_NCCREATE) // WM_NCCREATE посылается до события WM_CREATE в момент первичного создания окна
		// wParam - не используется
		// lParam - указатель на CREATESTRUCT структуру, которая содержит информацию об окне которой будет созданы
		// Члены структуры CREATESTRUCT идентичны параметрам которые принимает функция CreateWindowEx!!!
		// https://docs.microsoft.com/ru-ru/previous-versions/9930zz74(v=vs.120)
		// Особо интересен первый параметр
		// lpCreateParams - Указывает на данные, используемые для создания окна.
		// В нашем случае - это тот самый "this" который мы передали последним параметром
		// при вызове CreateWindow - это указатель на 
	{
		// extract ptr to window class from creation data
		const CREATESTRUCTW * const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);
		// SetWindowLongPtr - WinAPI function 
		SetWindowLongPtr(
			hWnd,
			GWLP_USERDATA,
			reinterpret_cast<LONG_PTR>(pWnd)); // set WinAPI managed user data to store ptr to window class
		// set message proc to normal (non-setup) handler now that setup is finished
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk));
		// forward message to window class handler
		return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
	}
	// if we get a message before the WM_NCCREATE message, handle with default handler
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT WINAPI Window::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	// retrieve ptr to window class
	Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	// forward message to window class handler
	return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
}

LRESULT Window::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	switch (msg)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}