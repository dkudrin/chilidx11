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
	wc.lpfnWndProc = WndProc; // ������ ���� ��������� ����� ����� ���� �������� �� ���������
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
		0, //DWORD dwExStyle,			// ���������� ����� ����
		pClassName, //LPCTSTR lpClassName,		// ��������� �� ������������������ ��� ������
		"Happy hard window", //LPCTSTR lpWindowName,		// ��������� �� ��� ����
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, //DWORD dwStyle,			// ����� ����
		200, //int x,				// �������������� ������� ����
		200,//int y,				// ������������ ������� ����
		640,//int nWidth,			// ������ ����
		480, //int nHeight,			// ������ ����
		nullptr, //HWND hWndParent,			// ���������� ������������� ��� ���� ������������
		nullptr, //HMENU hMenu,			// ���������� ���� ��� ������������� ��������� ����
		hInstance, //HINSTANCE hInstance,		// ���������� ���������� ���������� ���������	
		nullptr //LPVOID lpParam 			// ��������� �� ������ �������� ����
	);

	// show window 
	ShowWindow(hWnd, SW_SHOW);

	// message pump
	MSG msg; // ��������� �� ��������� � ����� ������ ��������� (� MSDN ���� �������� ����� ��� � ��� ��������� https://docs.microsoft.com/en-us/windows/desktop/api/winuser/ns-winuser-tagmsg)
	BOOL gResult;
	while ((gResult = GetMessage(
		&msg, // ��������� �� ��������� ������� ����� ��������� ���������� �� Windows �� ��������� ���� 
		nullptr, // ��������� �� ���� �� �������� �������� ���������, ���� NULL - ����� �� ����� �������� ��������� �� ���� ����
		0, // ������ ����� ��������� ������� �� ����� ��������
		0 // ���� ������ ����� ��������� ������� �� ����� ��������
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