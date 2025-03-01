#pragma once
#include <d3d11_2.h> //Versión básica de D3D11
#define SAFE_RELEASE(p) { if(p){p->Release(); p = nullptr;} }
#include <cassert>
class GraphicsAPI
{
public:
	GraphicsAPI(void* pWindowHandle);
	virtual ~GraphicsAPI();

private:
	void QueryInterfaces(int width,int height);
public:
	ID3D11Texture2D* CreateTexture(
									int width,
									int height,
									DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM, // Formato de los pixeles
									D3D11_USAGE usage = D3D11_USAGE_DEFAULT, // Si va a ser mutable o no mutable
									UINT bindFlags = D3D11_BIND_SHADER_RESOURCE, // Cómo la voy a poder utilizar
									UINT cpuAccessFlags = 0,
									UINT mipLevels = 1);

public:
	void* m_pWndHandl = nullptr;

	ID3D11Device1* m_pDevice = nullptr;
	ID3D11DeviceContext1* m_pDeviceContext = nullptr;

	IDXGISwapChain1* m_pSwapChain = nullptr;
	ID3D11RenderTargetView* m_pBackBufferRTV = nullptr;
	ID3D11DepthStencilView* m_pBackBufferDSV = nullptr;

};

