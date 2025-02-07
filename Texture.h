#pragma once
#include "Image.h"
#include "MathObjects.h"

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
	~Texture() = default;

	void createImage(const Image& img);
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

};

