#pragma once
#include "ChiliWin.h"
#include <string>
#include "ChiliException.h"

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
	// member functions принимают как Hidden параметр ссылку на объект из которого они вызваны.
	// winAPI не знает о том какой у нас мембер -
	// - поэтому для него используются static callback-и которые не имеют скрытых параметров
	HWND hWnd;

};

// error exception helper macro
#define CHWND_EXCEPT(hr) Window::Exception(__LINE__, __FILE__, hr)