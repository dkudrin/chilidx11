#include "Graphics.h"
#pragma comment(lib, "d3d11.lib")

Graphics::Graphics(HWND hWnd)
{
	DXGI_SWAP_CHAIN_DESC sd = {};
	// Buffer
	sd.BufferDesc.Width = 0; // use widnow size
	sd.BufferDesc.Height = 0; // use window size
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; // pixels layout
	sd.BufferDesc.RefreshRate.Numerator = 0;  // use window RefreshRate
	sd.BufferDesc.RefreshRate.Denominator = 0;  // use window RefreshRate
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;  // we should not require scaling - becouse we are in widnow
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; // interlaced (через строчная разверкат) or not - default
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // Использовать этот буффер как rendering target - Вся pipeline будет рендерить все в этот буффер
	sd.BufferCount = 1; // Один backbuffer (2 в  сумме)
	// Sampling mode
	sd.SampleDesc.Count = 1; // AntiAliasing - no AA for today
	sd.SampleDesc.Quality = 0;
	// etc
	sd.OutputWindow = hWnd; // Дускриптор окна, в которое будет происходить отрисовка
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; // Выключение swap еффекта - выключаем, это обеспечивает лучший вид и производительность
	sd.Flags = 0;

	// create device and front/back duffers, and swap chain and rednering context
	D3D11CreateDeviceAndSwapChain(
		nullptr, // using default video card
		D3D_DRIVER_TYPE_HARDWARE, // DriverType - hardware driver
		nullptr, // pp - handle to the Software driver, if we want use it
		0, // Flags
		nullptr,  //  Feature Levels - default
		0, // Feature Levels
		D3D11_SDK_VERSION, // SDK Version
		&sd, //pp - A pointer to a swap chain description (see DXGI_SWAP_CHAIN_DESC) that contains initialization parameters for the swap chain.
		&pSwap, // pp - Returns the address of a pointer to the IDXGISwapChain object that represents the swap chain used for rendering.
		&pDevice, // pp - Returns the address of a pointer to an ID3D11Device object that represents the device created. If this parameter is NULL, no ID3D11Device will be returned'.
		nullptr, // Resulting Feature level - Returns a pointer to a D3D_FEATURE_LEVEL, which represents the first element in an array of feature levels supported by the device. Supply NULL as an input if you don't need to determine which feature level is supported.
		&pContext //pp - Returns the address of a pointer to an ID3D11DeviceContext object that represents the device context. If this parameter is NULL, no ID3D11DeviceContext will be returned.
	);
	// gain access to texture subresource in swap chain (back buffer)
	ID3D11Resource* pBackBuffer = nullptr;
	pSwap->GetBuffer(
		0, // index of the buffer, that we want to get. 0 - backbuffer
		__uuidof(ID3D11Resource), // uuid of the interface that we want to recieve
		reinterpret_cast<void**>(&pBackBuffer) // pp that we can use to fill our pointer
	);
	// once we have a handler to device recource we can use it to create render target view on it
	pDevice->CreateRenderTargetView(
		pBackBuffer, // backbuffer
		nullptr, // additional options
		&pTarget // pp to our target
	);
	pBackBuffer->Release();
}

Graphics::~Graphics()
{
	if (pTarget != nullptr)
	{
		pTarget->Release();
	}
	if (pContext != nullptr)
	{
		pContext->Release();
	}
	if (pSwap != nullptr)
	{
		pSwap->Release();
	}
	if (pDevice != nullptr)
	{
		pDevice->Release();
	}
}

void Graphics::EndFrame()
{
	pSwap->Present( // Показ первого экрана из swap-chain
		1u, // Если вам кажется что ваша игра не может проигрываться плавно на 60fps то можно поставить здесь коэф 2
		0u); // Не используем никаких специальных флагов
}

void Graphics::ClearBuffer(float red, float green, float blue)
{
	const float color[] = { red, green, blue, 1.0f };
	pContext->ClearRenderTargetView(pTarget, color);
}

