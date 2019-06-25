#pragma once
#include "ChiliWin.h"
#include <string>
#include "ChiliException.h"
#include "KeyBoard.h"
#include "Mouse.h"
#include <optional>

class Window
{
public:
	class Exception : public ChiliException
	{
	public:
		Exception(int line, const char * file, HRESULT hr);
		const char * what() const override;
		const char * GetType() const override;
		static std::string TranslateErrorCode(HRESULT hr);
		HRESULT GetErrorCode() const;
		std::string GetErrorString() const;
	private:
		HRESULT hr;
	};
private:
	// singleton manages registration/cleanup of windows class
	// private nested class
	class WindowClass
	{
	public:
		static const char * GetName();
		static HINSTANCE GetInstance();
	private:
		WindowClass();
		~WindowClass();
		WindowClass(const WindowClass&) = delete;
		WindowClass& operator=(const WindowClass&) = delete;
		static constexpr const char* wndClassName = "Dkudrin Direct3D Engine Window";
		static WindowClass wndClass; // static instance - singleton
		HINSTANCE hInst;
	};
public:
	Window(int width, int height, const char * name); // creates window
	~Window(); // destroyes window
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	void SetTitle(const std::string& title);
	static std::optional<int> ProcessMessages();
private:
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	// member functions ��������� ��� Hidden �������� ������ �� ������ �� �������� ��� �������.
	// winAPI �� ����� � ��� ����� � ��� ������ -
	// - ������� ��� ���� ������������ static callback-� ������� �� ����� ������� ����������
public:
	KeyBoard kbd;
	Mouse mouse;
private:
	int width;
	int height;
	HWND hWnd;
};

// error exception helper macro
#define CHWND_EXCEPT(hr) Window::Exception(__LINE__, __FILE__, hr)
#define CHWND_LAST_EXCEPT() Window::Exception(__LINE__, __FILE__, GetLastError())