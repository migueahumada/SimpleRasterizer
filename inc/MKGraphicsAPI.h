#pragma once
#include "Module.h"
#include <d3d11_2.h>
#include "HelperMacros.h"

class GraphicsBuffer;
class VertexShader;
class PixelShader;

using Path = std::wstring;


namespace MKEngine{
  class MKGraphicsAPI : public Module<MKGraphicsAPI>
  {
  public:
    explicit MKGraphicsAPI(void* pHwnd);

    virtual ~MKGraphicsAPI();

    virtual void onStartUp() override;

    virtual void onShutDown() override;

		


	private:
		void QueryInterfaces(int width, int height);
	public:
		ID3D11Texture2D* CreateTexture(
			int width,
			int height,
			DXGI_FORMAT format = DXGI_FORMAT_B8G8R8A8_UNORM,
			D3D11_USAGE usage = D3D11_USAGE_DEFAULT,
			UINT bindFlags = D3D11_BIND_SHADER_RESOURCE,
			UINT cpuAccessFlags = 0,
			UINT mipLevels = 1,
			ID3D11ShaderResourceView** ppSRV = nullptr,
			ID3D11RenderTargetView** ppRTV = nullptr,
			ID3D11DepthStencilView** ppDSV = nullptr,
			ID3D11DepthStencilView** ppDSV_RO = nullptr);


		SPtr<VertexShader> CreateVertexShader(const Path& filePath,
			const String& entryFunction);

		SPtr<PixelShader> CreatePixelShader(const Path& filePath,
			const String& entryFunction);

		ID3D11InputLayout* CreateInputLayout(
			Vector<D3D11_INPUT_ELEMENT_DESC> pInputElementDesc,
			const WPtr<VertexShader>& pVertexShader);

		SPtr<GraphicsBuffer> CreateVertexBuffer(const Vector<char>& data);

		SPtr<GraphicsBuffer> CreateIndexBuffer(const Vector<char>& data);

		SPtr<GraphicsBuffer> CreateConstantBuffer(const Vector<char>& data);

		void writeToBuffer(const WPtr<GraphicsBuffer>& pBuffer,
			const Vector<char>& data);

  protected:
		friend class MKRenderer;

    void* m_pHwnd = nullptr;

    ID3D11Device1* m_pDevice = nullptr;
    ID3D11DeviceContext1* m_pDeviceContext = nullptr;

    IDXGISwapChain1* m_pSwapChain = nullptr;
    ID3D11RenderTargetView* m_pBackBufferRTV = nullptr;
    ID3D11DepthStencilView* m_pBackBufferDSV = nullptr;

  };

	MKGraphicsAPI& g_graphicsAPI();
}

