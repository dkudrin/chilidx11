#pragma once
#include "ChiliWin.h"
#include "ChiliException.h"
#include <d3d11.h>

class Graphics
{
public:
	class Exception : public ChiliException // base graphic exception
	{
		using ChiliException::ChiliException;
	};
	class HrException : public Exception // graphic exception that have HRESULT
	{
	public:
		HrException(int line, const char * file, HRESULT hr);
		const char * what() const override;
		const char * GetType() const override;
		HRESULT GetErrorCode() const;
		std::string GetErrorString() const;
		std::string GetErrorDescription() const;
	private:
		HRESULT hr;
	};
	class DeviceRemovedException : public HrException
	{
		using HrException::HrException;
	public:
		const char* GetType() const override;
	};
public:
	Graphics(HWND hWnd);
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	~Graphics();
	void EndFrame();
	void ClearBuffer(float red, float green, float blue); // function for clearing buffer with color
private:
	ID3D11Device* pDevice = nullptr;
	IDXGISwapChain* pSwap = nullptr;
	ID3D11DeviceContext* pContext = nullptr;
	ID3D11RenderTargetView* pTarget = nullptr;
};

