#include "Graphics.h"
#include "dxerr.h"
#include <sstream>
#include <d3dcompiler.h> // works with D3DCompiler.lib to load compiled shaders
#include <DirectXMath.h>

namespace dx = DirectX;

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib") // For compiled HLSL shaders (.cso) loading

// graphics exception checking/throwing macros (some with dxgi infos)
#define GFX_EXCEPT_NOINFO(hr) Graphics::HrException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_NOINFO(hrcall) if( FAILED( hr = (hrcall) ) ) throw Graphics::HrException( __LINE__,__FILE__,hr )

#ifndef NDEBUG
#define GFX_EXCEPT(hr) Graphics::HrException( __LINE__,__FILE__,(hr),infoManager.GetMessages() )
#define GFX_THROW_INFO(hrcall) infoManager.Set(); if( FAILED( hr = (hrcall) ) ) throw GFX_EXCEPT(hr)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException( __LINE__,__FILE__,(hr),infoManager.GetMessages() )
#define GFX_THROW_INFO_ONLY(call) infoManager.Set(); (call); {auto v = infoManager.GetMessages(); if(!v.empty()) {throw Graphics::InfoException( __LINE__,__FILE__,v);}}
#else
#define GFX_EXCEPT(hr) Graphics::HrException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_INFO(hrcall) GFX_THROW_NOINFO(hrcall)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException(__LINE__, __FILE__, (hr))
#define GFX_THROW_INFO_ONLY(call) (call)
#endif

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
	sd.OutputWindow = hWnd; // Дескриптор окна, в которое будет происходить отрисовка
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; // Выключение swap еффекта - выключаем, это обеспечивает лучший вид и производительность
	sd.Flags = 0;

	UINT swapCreateFlags = 0u;
#ifndef NDEBUG
	swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// for checkong results of d3d functions
	HRESULT hr;

	// create device and front/back duffers, and swap chain and rednering context
	GFX_THROW_INFO(D3D11CreateDeviceAndSwapChain(
		nullptr, // using default video card
		D3D_DRIVER_TYPE_HARDWARE, // DriverType - hardware driver
		nullptr, // pp - handle to the Software driver, if we want use it
		swapCreateFlags, // Flags D3D11_CREATE_DEVICE_DEBUG - get debug info in std::out
		nullptr,  //  Feature Levels - default
		0, // Feature Levels
		D3D11_SDK_VERSION, // SDK Version
		&sd, //pp - A pointer to a swap chain description (see DXGI_SWAP_CHAIN_DESC) that contains initialization parameters for the swap chain.
		&pSwap, // pp - Returns the address of a pointer to the IDXGISwapChain object that represents the swap chain used for rendering.
		&pDevice, // pp - Returns the address of a pointer to an ID3D11Device object that represents the device created. If this parameter is NULL, no ID3D11Device will be returned'.
		nullptr, // Resulting Feature level - Returns a pointer to a D3D_FEATURE_LEVEL, which represents the first element in an array of feature levels supported by the device. Supply NULL as an input if you don't need to determine which feature level is supported.
		&pContext //pp - Returns the address of a pointer to an ID3D11DeviceContext object that represents the device context. If this parameter is NULL, no ID3D11DeviceContext will be returned.
	));
	// gain access to texture subresource in swap chain (back buffer)
	Microsoft::WRL::ComPtr<ID3D11Resource> pBackBuffer;
	GFX_THROW_INFO(pSwap->GetBuffer(
		0, // index of the buffer, that we want to get. 0 - backbuffer
		__uuidof(ID3D11Resource), // uuid of the interface that we want to recieve
		&pBackBuffer // pp that we can use to fill our pointer
	));
	// once we have a handler to device recource we can use it to create render target view on it
	GFX_THROW_INFO(pDevice->CreateRenderTargetView(
		pBackBuffer.Get(), // backbuffer
		nullptr, // additional options
		&pTarget // pp to our target
	));
}

void Graphics::EndFrame()
{
	HRESULT hr;
#ifndef NDEBUG
	infoManager.Set();
#endif
	if (FAILED(hr =
		pSwap->Present( // Показ первого экрана из swap-chain
			1u, // Если вам кажется что ваша игра не может проигрываться плавно на 60fps то можно поставить здесь коэф 2
			0u // Не используем никаких специальных флагов
		)))
	{
		if (hr == DXGI_ERROR_DEVICE_REMOVED)
		{
			throw GFX_DEVICE_REMOVED_EXCEPT(pDevice->GetDeviceRemovedReason());
		}
		else
		{
			throw GFX_EXCEPT(hr);
		}
	}
}

void Graphics::ClearBuffer(float red, float green, float blue)
{
	const float color[] = { red, green, blue, 1.0f };
	pContext->ClearRenderTargetView(pTarget.Get(), color);
}

// function for clearing buffer with color

void Graphics::DrawTestTriangle( float angle)
{
	HRESULT hr;

	/***********<create Vertex shader *********************/
	struct Vertex
	{
		struct
		{
			float x;
			float y;
		} pos;
		struct
		{
			unsigned char r;
			unsigned char g;
			unsigned char b;
			unsigned char a;
		} color;
	};

	//>>> create VERTEX buffer (1 2d triangle at center of screen)
	Vertex vertices[] =	{
		{ 0.0f, 0.5f, 255, 0, 0, 0 },
		{ 0.5f, -0.5f, 0, 255, 0, 0 },
		{ -0.5f, -0.5f, 0, 0, 255, 0 },
		{ -0.3f, 0.3f, 0, 255, 0, 0 },
		{ 0.3f, 0.3f, 0, 0, 255, 0 },
		{ 0.0f, -0.8f, 255, 0, 0, 0 }
	};

	vertices[0].color.g = 255;

	D3D11_BUFFER_DESC vertexBufferDesc = {};
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.CPUAccessFlags = 0u;
	vertexBufferDesc.MiscFlags = 0u;
	vertexBufferDesc.ByteWidth = sizeof(vertices);
	vertexBufferDesc.StructureByteStride = sizeof(Vertex);

	D3D11_SUBRESOURCE_DATA subresData = {}; // Собственно данные для буффера
	subresData.pSysMem = vertices; // !!! Загрузка данных VERTEX буффера

	Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
	GFX_THROW_INFO(pDevice->CreateBuffer(&vertexBufferDesc, &subresData, &pVertexBuffer));

	// Bind VERTEX buffer to pipeline
	const UINT stride = sizeof(Vertex);
	const UINT offset = 0u;
	pContext->IASetVertexBuffers( // Input Assemble Set Vertex Buffers
		0u, // startSlot
		1u, // first and one buffer
		pVertexBuffer.GetAddressOf(),
		&stride,
		&offset
	);

	//>>> create INDEX buffer
	const unsigned short indices[] =
	{
		0, 1, 2,
		0, 2, 3,
		0, 4, 1,
		2, 1, 5
	};

	D3D11_BUFFER_DESC indexBufferDesc = {};
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.CPUAccessFlags = 0u;
	indexBufferDesc.MiscFlags = 0u;
	indexBufferDesc.ByteWidth = sizeof(vertices);
	indexBufferDesc.StructureByteStride = sizeof(unsigned short);

	D3D11_SUBRESOURCE_DATA indexBufferSubresData = {}; // Собственно данные для буффера
	indexBufferSubresData.pSysMem = indices; // !!! Загрузка данных INDEX буффера

	Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer; // такой же тип данных как у pVertexBuffer
	GFX_THROW_INFO(pDevice->CreateBuffer(&indexBufferDesc, &indexBufferSubresData, &pIndexBuffer));

	// Bind INDEX buffer to pipeline
	pContext->IASetIndexBuffer(
		pIndexBuffer.Get(),
		DXGI_FORMAT_R16_UINT, // 16 бит unsigned short
		0u // offset
	);

	//>>> create CONSTANT buffer - матрица преобразований над каждым из вертексов,
	// преобразования будут производиться над каждым из вертексов на отдельных ядрах видеокарты
	struct ConstantBuffer
	{
		dx::XMMATRIX transform; // 4x4 float matrix, but all methods of it are optimized for SIMD (много операций за один цикл работы процессора)
	};

	const ConstantBuffer constantBuffer =
	{
		{
			dx::XMMatrixRotationZ(angle) * dx::XMMatrixScaling(3.0f / 4.0f, 1.0f, 1.0f) // убираем эффект растягивания изображения при повороте из-за пропорций экрана
		}
	};

	D3D11_BUFFER_DESC constantBufferDesc = {};
	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC; // dynamic - allows the use of the Lock function. It is also possible to update with UpdateSubresource on a non-dynamic
	constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // разрешить процессору запись в буффер каждый фрейм
	constantBufferDesc.MiscFlags = 0u;
	constantBufferDesc.ByteWidth = sizeof(constantBuffer);
	constantBufferDesc.StructureByteStride = 0u;

	D3D11_SUBRESOURCE_DATA constantBufferSubresData = {};
	constantBufferSubresData.pSysMem = &constantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer;
	GFX_THROW_INFO(pDevice->CreateBuffer(&constantBufferDesc, &constantBufferSubresData, &pConstantBuffer));

	// bind constant buffer to vertex shader - ему ну нужно быть привязанным к input assembler ему нужно быть привязанным к шейдеру который будет его использовать
	pContext->VSSetConstantBuffers(
		0u, // с какого начать буффера
		1u, // сколько создаем буфферов
		pConstantBuffer.GetAddressOf()
	);

	//>>> create vertex SHADER
	Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
	Microsoft::WRL::ComPtr<ID3DBlob> pVertexShaderBlob; // byte blob
	GFX_THROW_INFO(D3DReadFileToBlob(L"VertexShader.cso", &pVertexShaderBlob)); // use D3DCompiler.lib to load compiled shader
	GFX_THROW_INFO(pDevice->CreateVertexShader(
		pVertexShaderBlob->GetBufferPointer(), // pointer to the byteCode
		pVertexShaderBlob->GetBufferSize(),
		nullptr,
		&pVertexShader
	));
	// bind vertex shader
	pContext->VSSetShader(pVertexShader.Get(), nullptr, 0u);


	//>>> input (vertex) layout (2d position only)
	Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
	const D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{
			"Position", // SemanticName - должен совпадать с тем что указано как semantic name в параметрах hlsl функции
			0, // SemanticIndex
			DXGI_FORMAT_R32G32_FLOAT, // Format - ничего с цветами не связано, просто 2 32битные float
			0, // InputSlot
			0, // AlignedByteOffset
			D3D11_INPUT_PER_VERTEX_DATA, // InputSlotClass
			0 // InstanceDataStepRate
		},
		{
			"Color",
			0,
			DXGI_FORMAT_R8G8B8A8_UNORM,
			0,
			8u,
			D3D11_INPUT_PER_VERTEX_DATA,
			0
		}
	};
	GFX_THROW_INFO(pDevice->CreateInputLayout(
		inputElementDesc,
		(UINT)std::size(inputElementDesc),
		pVertexShaderBlob->GetBufferPointer(), // pointer to the byteCode
		pVertexShaderBlob->GetBufferSize(),
		&pInputLayout
	));

	// bind vertex layout
	pContext->IASetInputLayout(pInputLayout.Get());

	// Set primitive topology to triangle list (groups of 3 vericies)
	pContext->IASetPrimitiveTopology( // IA - InputAssembler
		D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
	);

	/*********** end create Vertex shader>*********************/


	/***********<create Pixel shader *********************/
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
	Microsoft::WRL::ComPtr<ID3DBlob> pPixelShaderBlob; // byte blob
	GFX_THROW_INFO(D3DReadFileToBlob(L"PixelShader.cso", &pPixelShaderBlob));
	GFX_THROW_INFO(pDevice->CreatePixelShader(
		pPixelShaderBlob->GetBufferPointer(),
		pPixelShaderBlob->GetBufferSize(),
		nullptr,
		&pPixelShader
	));
	// bind pixel shader
	pContext->PSSetShader(pPixelShader.Get(), 0, 0);
	/*********** end create Pixel shader>*********************/

	// bind render target
	pContext->OMSetRenderTargets(1u, pTarget.GetAddressOf(), nullptr);

	// configure viewport
	D3D11_VIEWPORT viewport;
	viewport.Width = 800;
	viewport.Height = 600;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	pContext->RSSetViewports(1u, &viewport); // Rasterizer - Stage set viewports

	// draw indexed
	GFX_THROW_INFO_ONLY(pContext->DrawIndexed((UINT)std::size(indices), 0u, 0u));
}

// Graphics exception stuff
Graphics::HrException::HrException(int line, const char * file, HRESULT hr, std::vector<std::string> infoMsgs)
	:
	Exception(line, file),
	hr(hr)
{
	// join all info messages with newlines into single string
	for (const auto& m : infoMsgs)
	{
		info += m;
		info.push_back('\n');
	}
	// remove final newline if exists
	if (!info.empty())
	{
		info.pop_back();
	}
}

const char * Graphics::HrException::what() const
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
		<< std::dec << " (" << (unsigned long)GetErrorCode() << ")" << std::endl
		<< "[Error String] " << GetErrorString() << std::endl
		<< "[Description] " << GetErrorDescription() << std::endl;
	if (!info.empty())
	{
		oss << "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
	}
	oss << GetOriginString();

	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Graphics::HrException::GetType() const
{
	return "Chili Graphics Exception";
}

HRESULT Graphics::HrException::GetErrorCode() const
{
	return hr;
}

std::string Graphics::HrException::GetErrorString() const
{
	return DXGetErrorString(hr);
}

std::string Graphics::HrException::GetErrorDescription() const
{
	char buf[512];
	DXGetErrorDescription(hr, buf, sizeof(buf));
	return buf;
}

std::string Graphics::HrException::GetErrorInfo() const
{
	return info;
}


const char* Graphics::DeviceRemovedException::GetType() const
{
	return "Chili Graphics Exception [Device Removed] (DXGI_ERROR_DEVICE_REMOVED)";
}

Graphics::InfoException::InfoException(int line, const char * file, std::vector<std::string> infoMsgs) noexcept
	:
	Exception(line, file)
{
	// join all info messages with newlines into single string
	for (const auto& m : infoMsgs)
	{
		info += m;
		info.push_back('\n');
	}
	// remove final newline if exists
	if (!info.empty())
	{
		info.pop_back();
	}
}


const char* Graphics::InfoException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
	oss << GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Graphics::InfoException::GetType() const noexcept
{
	return "Chili Graphics Info Exception";
}

std::string Graphics::InfoException::GetErrorInfo() const noexcept
{
	return info;
}