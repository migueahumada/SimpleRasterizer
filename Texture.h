#pragma once
#include "Image.h"
#include "MathObjects.h"
#include "GraphicsAPI.h"
#include <d3d11_2.h>

namespace TEXTURE_ADDRESS {
	enum E {
		CLAMP = 0,
		WRAP,
		MIRROR,
		MIRROR_ONCE //Espejo en solament en un eje
	};
}

namespace BLEND_MODE {
	enum E {
		NONE = 0,
		ALPHA_BLEND, // Es la mezcla
		ADDITIVE // Es lo aditivo
	};
}

namespace SAMPLER_FILTER {
	enum E {
		POINT = 0,
		LINEAR
	};
}


class Texture
{
public:
	Texture() = default;
	~Texture();

	void createImage(const Image& img);
	void createImage(const Image& img, GraphicsAPI& pGraphicsAPI, DXGI_FORMAT format = DXGI_FORMAT_B8G8R8A8_UNORM);

	void adjustTextureAddress(TEXTURE_ADDRESS::E textureAddressMode, float& u, float& v);
	FloatColor getColor(float u, float v, TEXTURE_ADDRESS::E textureAddress);
	
	FloatColor sample(	float u,
						float v,
						TEXTURE_ADDRESS::E textureAdress = TEXTURE_ADDRESS::CLAMP,
						SAMPLER_FILTER::E filter = SAMPLER_FILTER::LINEAR);

	void draw(	Image& img,
				int x,
				int y,
				int srcInitX,
				int srcInitY,
				int srcEndX,
				int srcEndY,
				TEXTURE_ADDRESS::E textureAddressMode,
				BLEND_MODE::E blendMode);
	Image m_image;

	ID3D11Texture2D* m_pTexture = nullptr;

	ID3D11ShaderResourceView* m_pSRV = nullptr;
	ID3D11RenderTargetView* m_pRTV = nullptr;
	ID3D11DepthStencilView* m_pDSV = nullptr;

};

