#include "ChiliWin.h"
#include <string>
#include <sstream>

#include "Window.h"

int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nShowCmd) 
{
	try
	{
		Window wnd(800, 300, "Test title");
		Window wnd2(400, 100, "Test title2");
		MSG msg;
		BOOL gResult;

		// Examples of exception throw: 
		// throw CHWND_EXCEPT(ERROR_ARENA_TRASHED);
		// throw std::runtime_error("Test error");

		while ((gResult = GetMessage(&msg, nullptr, 0, 0)) > 0)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (gResult == -1)
		{
			return -1;
		}

		return msg.wParam;
	}
	catch (const ChiliException& e)
	{
		MessageBox(nullptr, e.what(), e.GetType(), MB_OK | MB_ICONEXCLAMATION);
	}
	catch (const std::exception& e)
	{
		MessageBox(nullptr, e.what(), "Standard Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	catch (...)
	{
		MessageBox(nullptr, "No details availible", "Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	return -1;
}