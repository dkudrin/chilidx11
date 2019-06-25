#include "Window.h"
#include <sstream>
#include "resource.h"

// Window Class Stuff
Window::WindowClass Window::WindowClass::wndClass;

Window::WindowClass::WindowClass() // конструктор
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
	wc.hIcon = static_cast<HICON>(LoadImage(hInst, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 32, 32, 0));
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = GetName();
	wc.hIconSm = static_cast<HICON>(LoadImage(hInst, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 16, 16, 0));
	RegisterClassEx(&wc); // Регистрация инстанса окна
}

Window::WindowClass::~WindowClass() // деструктор
{
	UnregisterClass(wndClassName, GetInstance()); // Разрегистрация инстанса окна
}

const char * Window::WindowClass::GetName()
{
	return wndClassName; // возавращает имя окна
}

HINSTANCE Window::WindowClass::GetInstance()
{
	return wndClass.hInst; // Возвращает просто инстанс окна
}

// Window
Window::Window(int width, int height, const char* name)
	:
	width(width),
	height(height)
{ // конструктор окна
	// calculate window size based on desired client region size
	RECT wr;
	wr.left = 100;
	wr.right = width + wr.left;
	wr.top = 100;
	wr.bottom = height + wr.top;
	DWORD wndStyle = WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU;
	if (AdjustWindowRect(&wr, wndStyle, FALSE) == 0)
	{
		throw CHWND_LAST_EXCEPT();
	}
	this;
	HINSTANCE wndClassInstance = WindowClass::GetInstance();
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
		wndClassInstance,
		this // !!! pointer to our window instance
	);
	if (hWnd == nullptr)
	{
		throw CHWND_LAST_EXCEPT();
	}
	// show window
	ShowWindow(hWnd, SW_SHOWDEFAULT);
}

Window::~Window()
{
	DestroyWindow(hWnd);
}

LRESULT WINAPI Window::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// use create parameter passed in form CreateWindow() to store window class pointer
	if (msg == WM_NCCREATE) // WM_NCCREATE посылается до события WM_CREATE в момент первичного создания окна
		// wParam - не используется
		// lParam - указатель на CREATESTRUCTW структуру, которая содержит информацию об окне которой будет создано
		// Члены структуры CREATESTRUCTW идентичны параметрам которые принимает функция CreateWindowEx!!!
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

LRESULT WINAPI Window::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// retrieve ptr to window class
	Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	// forward message to window class handler
	return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
}

LRESULT Window::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
	case WM_KILLFOCUS:
		kbd.ClearState();
		break;
	/******************** KEYBOARD MESSAGES *************************/
	case WM_SYSKEYDOWN:
	// syskey commands need to be handled to track ALT key (VK_MENU)
	case WM_KEYDOWN:
		if (!(lParam & 0x40000000) || kbd.AutorepeatIsEnabled()) // только если мы сами позволяем делать повторные нажатия в играх слать сообщения о них иначе игнор
		{
			kbd.OnKeyPressed(static_cast<unsigned char>(wParam));
		}
		break;
	case WM_SYSKEYUP:
	case WM_KEYUP:
		kbd.OnKeyReleased(static_cast<unsigned char>(wParam));
	case WM_CHAR:
		kbd.OnChar(static_cast<unsigned char>(wParam));
		break;
	/******************** END KEYBOARD MESSAGES *************************/
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

// Window Exception Stuff
Window::Exception::Exception(int line, const char * file, HRESULT hr) // Derived class constructor
	:
	ChiliException(line, file), // call base counstructor class
	hr(hr) // set priveate var hr
{}

const char * Window::Exception::what() const
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] " << GetErrorCode() << std::endl
		<< "[Description]" << GetErrorString() << std::endl
		<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char * Window::Exception::GetType() const
{
	return "Chili Widndow Eception";
}

std::string Window::Exception::TranslateErrorCode(HRESULT hr)
{
	char * pMsgBuf = nullptr;
	DWORD nMsgLen = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr,
		hr,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPSTR>(&pMsgBuf),
		0,
		nullptr
	);
	if (nMsgLen == 0)
	{
		return "Unidentified error code";
	}
	std::string errorString = pMsgBuf;
	LocalFree(pMsgBuf);
	return errorString;
}

HRESULT Window::Exception::GetErrorCode() const
{
	return hr;
}

std::string Window::Exception::GetErrorString() const
{
	return TranslateErrorCode(hr);
}

void Window::SetTitle(const std::string& title)
{
	if (SetWindowText(hWnd, title.c_str()) == 0)
	{
		throw CHWND_LAST_EXCEPT();
	}
}