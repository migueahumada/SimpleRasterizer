#pragma once
#include "Image.h"

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

struct FloatColor {
	FloatColor() = default;
	FloatColor(float _r, float _g, float _b, float _a) :
		r(_r), g(_g), b(_b), a(_a) {}
	FloatColor(const Color& color) :
		r(color.r/255.0f), 
		g(color.g/255.0f),
		b(color.b / 255.0f), 
		a(color.a / 255.0f) {}

	Color toColor() {
		
		return Color{
			static_cast<unsigned char>(r * 255.0f),
			static_cast<unsigned char>(g * 255.0f),
			static_cast<unsigned char>(b * 255.0f),
			static_cast<unsigned char>(a * 255.0f)
		};
	}

	FloatColor operator*(const FloatColor& color) {
		return FloatColor(r * color.r, g * color.g, b * color.b, a * color.a);
	}

	FloatColor operator+(const FloatColor& color) {
		return FloatColor(r + color.r, g + color.g, b + color.b, a + color.a);
	}

	FloatColor operator*(float escalar) {
		return FloatColor(r * escalar, g * escalar, b * escalar, a * escalar);
	}

	void saturate() {
		r = r > 1.0f ? 1.0f : r;
		g = g > 1.0f ? 1.0f : g;
		b = b > 1.0f ? 1.0f : b;
		a = a > 1.0f ? 1.0f : a;
	}

	float r;
	float g;
	float b;
	float a;
};

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

