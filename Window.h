#pragma once
#include "ChiliWin.h"
#include <string>

class Window
{
private:
	// singleton manages registration/cleanup of windows class
	// private nested class
	class WindowClass
	{
	public:
		static const char * GetName() noexcept;
		static HINSTANCE GetInstance() noexcept;
	private:
		WindowClass() noexcept;
		~WindowClass() noexcept;
		WindowClass(const WindowClass&) = delete;
		WindowClass& operator=(const WindowClass&) = delete;
		static constexpr const char* wndClassName = "Dkudrin Direct3D Engine Window";
		static WindowClass wndClass; // static instance - singleton
		HINSTANCE hInst;
	};
public:
	Window(int width, int height, const char * name) noexcept; // creates window
	~Window(); // destroyes window
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
private:
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	// member functions ��������� ��� Hidden �������� ������ �� ������ �� �������� ��� �������.
	// winAPI �� ����� � ��� ����� � ��� ������ -
	// - ������� ��� ���� ������������ static callback-� ������� �� ����� ������� ����������
	HWND hWnd;

};