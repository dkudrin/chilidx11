#pragma once
#include "ChiliWin.h"
#include <string>
#include "ChiliException.h"
#include "KeyBoard.h"
#include "Mouse.h"
#include "Graphics.h"
#include <optional>
#include <memory>

class Window
{
public:
	class Exception : public ChiliException
	{
		using ChiliException::ChiliException;
	public:
		static std::string TranslateErrorCode(HRESULT hr);
	};
	class HrException : public Exception
	{
	public:
		HrException(int line, const char* file, HRESULT hr);
		const char* what() const override;
		const char* GetType() const override;
		HRESULT GetErrorCode() const;
		std::string GetErrorDescription() const;
	private:
		HRESULT hr;
	};
	class NoGfxException : public Exception
	{
	public:
		using Exception::Exception;
		const char* GetType() const override;
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
	Graphics& Gfx();
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
	std::unique_ptr<Graphics> pGfx; // uniq pointer to graphics
};

// error exception helper macro
#define CHWND_EXCEPT( hr ) Window::HrException( __LINE__,__FILE__,(hr) )
#define CHWND_LAST_EXCEPT() Window::HrException( __LINE__,__FILE__,GetLastError() )
#define CHWND_NOGFX_EXCEPT() Window::NoGfxException( __LINE__,__FILE__ )