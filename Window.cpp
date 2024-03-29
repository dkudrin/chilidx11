#include "Window.h"
#include <sstream>
#include "resource.h"

// Window Class Stuff
Window::WindowClass Window::WindowClass::wndClass;

Window::WindowClass::WindowClass() // �����������
	:
	hInst(GetModuleHandle(nullptr)) // initialization list - ������ ������������� ����� ���������� �� ������������
									// GetModuleHandle -  ��������� hadle �� instance ����
									// ���� �� �� ��� ������������� �������� �� ���������� � ����������� ��������

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
	RegisterClassEx(&wc); // ����������� �������� ����
}

Window::WindowClass::~WindowClass() // ����������
{
	UnregisterClass(wndClassName, GetInstance()); // �������������� �������� ����
}

const char * Window::WindowClass::GetName()
{
	return wndClassName; // ����������� ��� ����
}

HINSTANCE Window::WindowClass::GetInstance()
{
	return wndClass.hInst; // ���������� ������ ������� ����
}

// Window
Window::Window(int width, int height, const char* name)
	:
	width(width),
	height(height)
{ // ����������� ����
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
	pGfx = std::make_unique<Graphics>(hWnd);
}

Window::~Window()
{
	DestroyWindow(hWnd);
}

void Window::SetTitle(const std::string& title)
{
	if (SetWindowText(hWnd, title.c_str()) == 0)
	{
		throw CHWND_LAST_EXCEPT();
	}
}

std::optional<int> Window::ProcessMessages()
{
	MSG msg;
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			return msg.wParam;
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return {};
}

Graphics & Window::Gfx()
{
	if (!pGfx)
	{
		throw CHWND_NOGFX_EXCEPT();
	}
	return *pGfx;
}

LRESULT WINAPI Window::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// use create parameter passed in form CreateWindow() to store window class pointer
	if (msg == WM_NCCREATE) // WM_NCCREATE ���������� �� ������� WM_CREATE � ������ ���������� �������� ����
		// wParam - �� ������������
		// lParam - ��������� �� CREATESTRUCTW ���������, ������� �������� ���������� �� ���� ������� ����� �������
		// ����� ��������� CREATESTRUCTW ��������� ���������� ������� ��������� ������� CreateWindowEx!!!
		// https://docs.microsoft.com/ru-ru/previous-versions/9930zz74(v=vs.120)
		// ����� ��������� ������ ��������
		// lpCreateParams - ��������� �� ������, ������������ ��� �������� ����.
		// � ����� ������ - ��� ��� ����� "this" ������� �� �������� ��������� ����������
		// ��� ������ CreateWindow - ��� ��������� ��
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
		if (!(lParam & 0x40000000) || kbd.AutorepeatIsEnabled()) // ������ ���� �� ���� ��������� ������ ��������� ������� � ����� ����� ��������� � ��� ����� �����
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

	/******************** MOUSE MESSAGES *************************/
	case WM_MOUSEMOVE:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		// in client region -> log move, and log enter + capture mouse
		if (pt.x >= 0 && pt.x < width && pt.y >= 0 && pt.y < height)
		{
			mouse.OnMouseMove(pt.x, pt.y);
			if (!mouse.IsInWindow())
			{
				SetCapture(hWnd); // winAPI function
				mouse.OnMouseEnter();
			}
		}
		// not in client -> log move / maintain capture if button down
		else
		{
			if (wParam & (MK_LBUTTON | MK_RBUTTON))
			{
				mouse.OnMouseMove(pt.x, pt.y);
			}
			// button up -> release capture /log event for leaving
			else
			{
				ReleaseCapture(); // winAPI function
				mouse.OnMouseLeave();
			}
		}
		mouse.OnMouseMove(pt.x, pt.y);
		break;
	}
	case WM_LBUTTONDOWN:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnLeftPressed(pt.x, pt.y);
		break;
	}
	case WM_RBUTTONDOWN:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnRightPressed(pt.x, pt.y);
		break;
	}
	case WM_LBUTTONUP:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnLeftReleased(pt.x, pt.y);
		break;
	}
	case WM_RBUTTONUP:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnRightReleased(pt.x, pt.y);
		break;
	}
	case WM_MOUSEWHEEL:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		const int delta = GET_WHEEL_DELTA_WPARAM(wParam);
		mouse.OnWheelDelta(pt.x, pt.y, delta);
		break;
	}
	/******************** END MOUSE MESSAGES *************************/
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}


/********************* EXCEPTIONS *****************************/

// Window Exception Stuff
std::string Window::Exception::TranslateErrorCode(HRESULT hr)
{
	char * pMsgBuf = nullptr;
	// windows will allocate memory for err string and make our pointer point to it
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
	// 0 string length returned indicates a failure
	if (nMsgLen == 0)
	{
		return "Unidentified error code";
	}
	// copy error string from windows-allocated buffer to std::string
	std::string errorString = pMsgBuf;
	// free windows buffer
	LocalFree(pMsgBuf);
	return errorString;
}

Window::HrException::HrException(int line, const char * file, HRESULT hr) // Derived class constructor
	:
	Exception(line, file), // call base counstructor class
	hr(hr) // set priveate var hr
{}

const char * Window::HrException::what() const
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
		<< std::dec << " (" << (unsigned long)GetErrorCode() << ")" << std::endl
		<< "[Description] " << GetErrorDescription() << std::endl
		<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char * Window::HrException::GetType() const
{
	return "Chili Widndow HrException";
}

HRESULT Window::HrException::GetErrorCode() const
{
	return hr;
}

std::string Window::HrException::GetErrorDescription() const
{
	return Exception::TranslateErrorCode(hr);
}

const char* Window::NoGfxException::GetType() const
{
	return "Chili Window Exception [No Graphics]";
}
