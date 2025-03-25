#pragma once

#include <d3d11_2.h> //Versi�n b�sica de D3D11
#include <cstring>
#include <fstream>
#include "HelperMacros.h"
#include "Shader.h"
#include "GraphicsBuffer.h"

//This should be cstring

using Path = std::wstring;
using String = std::string;


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
									DXGI_FORMAT format = DXGI_FORMAT_B8G8R8A8_UNORM, // Formato de los pixeles
									D3D11_USAGE usage = D3D11_USAGE_DEFAULT, // Si va a ser mutable o no mutable
									UINT bindFlags = D3D11_BIND_SHADER_RESOURCE, // C�mo la voy a poder utilizar
									UINT cpuAccessFlags = 0,
									UINT mipLevels = 1,
									ID3D11ShaderResourceView** ppSRV = nullptr,
									ID3D11RenderTargetView** ppRTV = nullptr,
									ID3D11DepthStencilView** ppDSV = nullptr);


	UPtr<VertexShader> CreateVertexShader(	const Path& filePath,
											const String& entryFunction);

	UPtr<PixelShader> CreatePixelShader(const Path& filePath,
										const String& entryFunction);
	ID3D11InputLayout* CreateInputLayout(	Vector<D3D11_INPUT_ELEMENT_DESC> pInputElementDesc,
											const UPtr<VertexShader>& pVertexShader);

	UPtr<GraphicsBuffer> CreateVertexBuffer(const Vector<char>& data);
	UPtr<GraphicsBuffer> CreateIndexBuffer(const Vector<char>& data);
	UPtr<GraphicsBuffer> CreateConstantBuffer(const Vector<char>& data);

	void writeToBuffer(const UPtr<GraphicsBuffer>& pBuffer, const Vector<char>& data);

public:
	void* m_pWndHandl = nullptr;

	ID3D11Device1* m_pDevice = nullptr;
	ID3D11DeviceContext1* m_pDeviceContext = nullptr;

	IDXGISwapChain1* m_pSwapChain = nullptr;
	ID3D11RenderTargetView* m_pBackBufferRTV = nullptr;
	ID3D11DepthStencilView* m_pBackBufferDSV = nullptr;
	

};

