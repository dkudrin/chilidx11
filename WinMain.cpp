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
		0, //DWORD dwExStyle,			// ���������� ����� ����
		pClassName, //LPCTSTR lpClassName,		// ��������� �� ������������������ ��� ������
		"Happy hard window", //LPCTSTR lpWindowName,		// ��������� �� ��� ����
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, //DWORD dwStyle,			// ����� ����
		200, //int x,				// �������������� ������� ����
		200,//int y,				// ������������ ������� ����
		640,//int nWidth,			// ������ ����
		640, //int nHeight,			// ������ ����
		nullptr, //HWND hWndParent,			// ���������� ������������� ��� ���� ������������
		nullptr, //HMENU hMenu,			// ���������� ���� ��� ������������� ��������� ����
		hInstance, //HINSTANCE hInstance,		// ���������� ���������� ���������� ���������	
		nullptr //LPVOID lpParam 			// ��������� �� ������ �������� ����
	);

	// show window 
	ShowWindow(hWnd, SW_SHOW);
	LPMSG lpMsg;
	GetMessage(
		lpMsg, // ��������� �� ��������� ������� ����� ��������� ���������� �� Windows �� ��������� ���� 
		NULL,
		
		);
	while (true);
	return 0;
}