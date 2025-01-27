#include "Texture.h"

void Texture::createImage(const Image& img) {
	m_image = img;
}

void Texture::adjustTextureAddress(TEXTURE_ADDRESS::E textureAddressMode, float& u, float& v)
{
	//TODO: Explicar todo lo que se hace en esta parte plsss
	switch (textureAddressMode)
	{
	case TEXTURE_ADDRESS::CLAMP:
		u = u < 0.0f ? 0.0f : u;
		u = u > 1.0f ? 1.0f : u;
		v = v < 0.0f ? 0.0f : v;
		v = v > 1.0f ? 1.0f : v;
		break;

	case TEXTURE_ADDRESS::WRAP:
		u = std::fmodf(u, 1.0f);
		v = std::fmodf(v, 1.0f);
		break;

	case TEXTURE_ADDRESS::MIRROR:
		u = std::fmodf(u, 2.0f);
		v = std::fmodf(v, 2.0f);

		if (u < 0.0f)
		{
			u = 2.0f + u;
		}
		if (v < 0.0f)
		{
			v = 2.0f + v;
		}
		if (u > 1.0f)
		{
			u = 2.0f - u;
		}
		if (v > 1.0f)
		{
			v = 2.0f - v;
		}


		break;

	case TEXTURE_ADDRESS::MIRROR_ONCE:
		if ((u > -1.0f && u < 0.0f) || (u > 1.0f && u < 2.0f))
		{
			u = 1.0f - std::fmodf(u, 1.0f);
		}
		else {
			u = u < 0.0f ? 0.0f : u;
			u = u > 1.0f ? 1.0f : u;
		}

		if ((v > -1.0f && v < 0.0f) || (v > 1.0f && v < 2.0f))
		{
			v = 1.0f - std::fmodf(v, 1.0f);
		}
		else {
			v = v < 0.0f ? 0.0f : v;
			v = v > 1.0f ? 1.0f : v;
		}

		break;

	default:
		break;
	}
}

FloatColor Texture::getColor(float u, float v, TEXTURE_ADDRESS::E textureAddress)
{

	adjustTextureAddress(textureAddress, u, v );
	int x = static_cast<int>(u * (m_image.getWidth() - 1));
	int y = static_cast<int>(v * (m_image.getHeight() - 1));
	return FloatColor(m_image.getPixel(x, y));

}

FloatColor Texture::sample(float u, float v, TEXTURE_ADDRESS::E textureAddress, SAMPLER_FILTER::E filter)
{
	switch (filter)
	{
	case SAMPLER_FILTER::POINT:
	{
		adjustTextureAddress(textureAddress, u, v);
		int x = static_cast<int>(u * (m_image.getWidth() - 1));
		int y = static_cast<int>(v * (m_image.getHeight() - 1));
		return FloatColor(m_image.getPixel(x, y));
	}
	case SAMPLER_FILTER::LINEAR:
	{
		adjustTextureAddress(textureAddress, u, v);
		float x = u * (m_image.getWidth() - 1);
		float y = v * (m_image.getHeight() - 1);

		int x0 = static_cast<int>(x);
		int y0 = static_cast<int>(y);
		int x1 = std::min(x0 + 1, m_image.getWidth() - 1);
		int y1 = std::min(y0 + 1, m_image.getHeight() - 1);

		float dx = x - x0;
		float dy = y - y0;

		FloatColor c00(m_image.getPixel(x0, y0));
		FloatColor c10(m_image.getPixel(x1, y0));
		FloatColor c01(m_image.getPixel(x0, y1));
		FloatColor c11(m_image.getPixel(x1, y1));

		FloatColor c0 = c00 * (1.0f - dx) + c10 * dx;
		FloatColor c1 = c01 * (1.0f - dx) + c11 * dx;

		return c0 * (1.0f - dy) + c1 * dy;
	}

	default: return FloatColor(0.0f, 0.0f, 0.0f, 0.0f);

	}
}

void Texture::draw(	Image& img, 
					int x, 
					int y, 
					int srcInitX, 
					int srcInitY, 
					int srcEndX, 
					int srcEndY, 
					TEXTURE_ADDRESS::E textureAddressMode,
					BLEND_MODE::E blendMode){

	//Caso en el cual la imagen está fuera de la pantalla
	if (x >= img.getWidth() || y >= img.getHeight()){ return; }
	if (0 == srcEndX) srcEndX = m_image.getWidth();
	if (0 == srcEndY) srcEndY = m_image.getHeight();
	if (x < 0){ srcInitX -= x; x = 0;}
	if (y < 0){ srcInitY -= y; y = 0; }

	int realWidth = srcEndX - srcInitX;
	int realHeight = srcEndY - srcInitY;
	if (realWidth < 0 || realHeight < 0){ return; }
	if (x + realWidth > img.getWidth()){ realWidth -= (x + realWidth) - img.getWidth(); }
	if (y + realHeight > img.getHeight()){ realHeight -= (y + realHeight) - img.getHeight(); }

	for (int i = 0; i < realHeight; ++i)
	{
		for (int j = 0; j < realWidth; ++j)
		{
			FloatColor srcColor = sample(static_cast<float>(j) / realWidth, static_cast<float>(i) / realHeight, textureAddressMode);
			FloatColor dstColor = FloatColor(img.getPixel(x + j, y + i));
			FloatColor blendedColor;
			switch (blendMode)
			{
			case BLEND_MODE::ALPHA_BLEND:
				blendedColor = srcColor * srcColor.a + dstColor * (1.0f - srcColor.a);
				break;
			case BLEND_MODE::ADDITIVE:
				blendedColor = (srcColor * srcColor.a) + dstColor;
				blendedColor.saturate();
				//blendedColor = srcColor + dstColor;
				break;
			case BLEND_MODE::NONE:
			default:
				blendedColor = srcColor;
				break;
			}
			
			img.setPixel(x + j, y + i, blendedColor.toColor());

		}
	}
}
