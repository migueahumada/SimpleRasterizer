#include "GraphicsAPI.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <vector>
#include <cassert>

#include <fstream>

#include <d3dcompiler.h>


template <typename T>
using Vector = std::vector<T>;

GraphicsAPI::GraphicsAPI(void* pWndHandle) : m_pWndHandl(pWndHandle)
{
	//Get the size of the window
	
	HWND hwnd = static_cast<HWND>(pWndHandle);
	
	RECT rc;
	GetClientRect(hwnd, &rc);

	Vector<IDXGIAdapter1*> vAdapters;
	IDXGIAdapter1* pAdapter = nullptr;
	IDXGIFactory1* pFactory = nullptr;

	//hAY INTERFACES QUE SALEN DE UNA INTERFAZ MÁS VIEJA
	//DX significa que es una direct graphics interface
	//Crear un facto
	CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&pFactory);

	for (UINT i = 0; pFactory->EnumAdapters1(i,&pAdapter) != DXGI_ERROR_NOT_FOUND; ++i)
	{
		DXGI_ADAPTER_DESC1 desc;
		pAdapter->GetDesc1(&desc);
		vAdapters.push_back(pAdapter);
	}
	//Orden de destrucción se destruye de manera inverasa al orden de creación
	SAFE_RELEASE(pAdapter);
	SAFE_RELEASE(pFactory);

	
	Vector<D3D_FEATURE_LEVEL> vFeatureLevels = {	D3D_FEATURE_LEVEL_11_1, 
													D3D_FEATURE_LEVEL_11_0, 
													D3D_FEATURE_LEVEL_10_1,
													D3D_FEATURE_LEVEL_10_0,
													D3D_FEATURE_LEVEL_9_3};

	D3D_FEATURE_LEVEL selectedFeatureLevel;

	ID3D11Device* pDevice = nullptr;
	ID3D11DeviceContext* pDeviceContext = nullptr;

	UINT deviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if defined(_DEBUG)
	deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif 
	

	//Qué singifica crear un dispositivo? 
	//Es como un panel de control
	HRESULT hr = D3D11CreateDevice(	vAdapters[0],
									D3D_DRIVER_TYPE_UNKNOWN, // el tipo de driver que quiero usar
									nullptr, 
									deviceFlags, 
									vFeatureLevels.data(), // quÉ nivel de caracte´risticas de directx quiro usar 
									vFeatureLevels.size(),
									D3D11_SDK_VERSION, //Que sdk usar 
									&pDevice, // Si todo sale bien, el device se crea aquí 
									&selectedFeatureLevel, //Aquí se dice qué feature levels se seleccciono
									&pDeviceContext); //Saca el contexto
	//WCHAR
	if (FAILED(hr))
	{
		MessageBox(hwnd, L"Failed to create device", L"Error", MB_OK);
		return;
	}

	pDevice->QueryInterface(__uuidof(ID3D11Device1), (void**)&m_pDevice);
	pDeviceContext->QueryInterface(__uuidof(ID3D11DeviceContext1), (void**)&m_pDeviceContext);

	SAFE_RELEASE(pDeviceContext);
	SAFE_RELEASE(pDevice);

	//uTILIZAR RECURSOS -> ES EL CONTEXT
	//EL QUE SE ENCARGADE HACER EL TRBAJO ES EL DEVICE CONTEXT
	DXGI_SWAP_CHAIN_DESC1 scDesc;
	memset(&scDesc, 0, sizeof(scDesc));

	scDesc.Width = rc.right;
	scDesc.Height = rc.bottom;
	scDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; //Unrom->unsigned normalized sin signo.
	scDesc.Stereo = false; // Para monitores viejos 3d ya no se usa.

	scDesc.SampleDesc.Count = 1; // Multisample Antialiasing -> MSAA
	scDesc.SampleDesc.Quality = 0;

	scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;// Para que se pueda mostrar como target render otuputs
	scDesc.BufferCount = 2; // 2 buffers para que se pueda hacer el swap
	scDesc.Scaling = DXGI_SCALING_NONE; // No quiero escalados de ningún tipo.
	scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL; // el efecto de swap que quiero usar

	IDXGIDevice1* pDXGIDevice = nullptr;
	m_pDevice->QueryInterface(__uuidof(IDXGIDevice1), (void**)&pDXGIDevice);

	IDXGIAdapter* pDXGIAdapter = nullptr;
	pDXGIDevice->GetAdapter(&pDXGIAdapter);

	IDXGIFactory2* pFactory2 = nullptr;
	pDXGIAdapter->GetParent(__uuidof(IDXGIFactory2), (void**)&pFactory2);

	hr = pFactory2->CreateSwapChainForHwnd(	m_pDevice,
											hwnd,
											&scDesc,
											nullptr,
											nullptr,
											&m_pSwapChain);

	pDXGIDevice->SetMaximumFrameLatency(3);

	if(FAILED(hr))
	{
		MessageBox(hwnd, L"Failed to create swap chain", L"Error", MB_OK);
		return;
	}
	QueryInterfaces(scDesc.Width, scDesc.Height);

	//Setup the viewport 
	D3D11_VIEWPORT vp;
	vp.Width = static_cast<float>(scDesc.Width);
	vp.Height = static_cast<float>(scDesc.Height);
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;

	m_pDeviceContext->RSSetViewports(1, &vp);

	
}

GraphicsAPI::~GraphicsAPI()
{
}

void GraphicsAPI::QueryInterfaces(int width, int height)
{
	if (!m_pSwapChain)
	{
		return;
	}

	ID3D11Texture2D* pBackBuffer = nullptr;
	m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer);
	assert(pBackBuffer);
	m_pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &m_pBackBufferRTV);
	
	SAFE_RELEASE(pBackBuffer);

	ID3D11Texture2D* pDepthStencil = nullptr;
	
	pDepthStencil = CreateTexture(	width, 
									height, 
									DXGI_FORMAT_D24_UNORM_S8_UINT, 
									D3D11_USAGE_DEFAULT, 
									D3D11_BIND_DEPTH_STENCIL);
	if (!pDepthStencil)
	{
		MessageBox(nullptr, L"Failed to create Depth stencil buffer", L"Error",MB_OK);
		return;
	}

	m_pDevice->CreateDepthStencilView(pDepthStencil, nullptr, &m_pBackBufferDSV);

	SAFE_RELEASE(pDepthStencil);
	
}

ID3D11Texture2D* GraphicsAPI::CreateTexture(int width, 
											int height, 
											DXGI_FORMAT format, 
											D3D11_USAGE usage, 
											UINT bindFlags, 
											UINT cpuAccessFlags, 
											UINT mipLevels)
{
	ID3D11Texture2D* pTexture = nullptr;

	D3D11_TEXTURE2D_DESC desc;
	memset(&desc, 0, sizeof(D3D11_TEXTURE2D_DESC));

	desc.ArraySize = 1;
	desc.BindFlags = bindFlags;
	desc.CPUAccessFlags = cpuAccessFlags;
	desc.Format = format;
	desc.Height = height;
	desc.Width = width;
	desc.MipLevels = mipLevels;
	desc.MiscFlags = 0;

	desc.SampleDesc.Count = 1; //MSAA
	desc.SampleDesc.Quality = 0; //MSAA

	desc.Usage = usage;
	
	if (FAILED(m_pDevice->CreateTexture2D(&desc, nullptr, &pTexture)))
	{
		return nullptr;
	}

	return pTexture;
}


UPtr<VertexShader> GraphicsAPI::CreateVertexShader(const Path& filePath, const String& entryFunction)
{
	UPtr<VertexShader> pVertexShader = make_unique<VertexShader>();
	if(!pVertexShader->Compile(filePath, entryFunction, "vs_5_0"))
	{
		return nullptr;
	}

	HRESULT hr = m_pDevice->CreateVertexShader(	pVertexShader->GetBlob()->GetBufferPointer(),
												pVertexShader->GetBlob()->GetBufferSize(),
												nullptr,
												&pVertexShader->m_pVertexShader);

	if (FAILED(hr))
	{
		MessageBox(nullptr, L"Failed to create vertex shader", L"Error", MB_OK);
		return nullptr;
	}

	return pVertexShader;
}

UPtr<PixelShader> GraphicsAPI::CreatePixelShader(const Path& filePath, const String& entryFunction)
{
	UPtr<PixelShader> pPixelShader = make_unique<PixelShader>();
	if (!pPixelShader->Compile(filePath, entryFunction, "ps_5_0"))
	{
		return nullptr;
	}

	HRESULT hr = m_pDevice->CreatePixelShader(pPixelShader->GetBlob()->GetBufferPointer(),
		pPixelShader->GetBlob()->GetBufferSize(),
		nullptr,
		&pPixelShader->m_pPixelShader);

	if (FAILED(hr))
	{
		MessageBox(nullptr, L"Failed to create pixel shader", L"Error", MB_OK);
		return nullptr;
	}

	return pPixelShader;
}
