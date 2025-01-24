#include "Image.h"
#include <iostream>

Image::Image()
{

}

Image::~Image()
{
	m_pixels = nullptr;
	delete m_pixels;
}

void Image::create(int width, int height, int bpp)
{
	m_width = width; //8
	m_height = height; //8
	m_bpp = bpp;
	m_pixels = new unsigned char[getPitch() * m_height];
	//std::cout << "The resolution of the image is: " << width <<" x " << height << std::endl;
	//std::cout << "Pixel Size: " << getPitch() * m_height << " bytes" << std::endl;
	
}

void Image::encode(const char* filename)
{
	//Creates file
	std::fstream file(filename,std::ios::binary | std::ios::out);
	
	if (!file.is_open()) //Not open? do nothing
	{
		return;
	}


	int padding = getPitch() % 4;
	int lineMemoryWidth = getPitch();

	if (padding)
	{
		padding = 4 - padding;
		lineMemoryWidth += padding;
	}
	
	BitMapFileHeader fileHeader;
	memset(&fileHeader, 0, sizeof(BitMapFileHeader));
	BitMapSaveHeader bmpInfo;
	memset(&bmpInfo, 0, sizeof(BitMapSaveHeader));

	int headerSize = sizeof(BitMapFileHeader) + sizeof(BitMapSaveHeader);

	fileHeader.bfType = 0x4D42;
	fileHeader.bfSize = headerSize + (lineMemoryWidth * m_height);
	fileHeader.bfOffBits = headerSize;

	bmpInfo.coreHeader.bcSize = sizeof(BitMapSaveHeader);
	bmpInfo.coreHeader.bcWidth= m_width;
	bmpInfo.coreHeader.bcHeight = m_height;
	bmpInfo.coreHeader.bcPlanes = 1;
	bmpInfo.coreHeader.bcBitCount = m_bpp;
	bmpInfo.biXPelsPerMeter = 3780;
	bmpInfo.biYPelsPerMeter = 3780;

	file.write(reinterpret_cast<char*>(&fileHeader), sizeof(BitMapFileHeader));
	file.write(reinterpret_cast<char*>(&bmpInfo), sizeof(BitMapSaveHeader));

	char paddBuffer[3] = { 0,0,0};

	for (int line = m_height - 1; line >= 0; --line)
	{

		file.write(reinterpret_cast<char*>(&m_pixels[getPitch() * line]), getPitch());
		if (padding)
		{
			file.write(paddBuffer, padding);
		}
	}

}

void Image::decode(const char* filename)
{
	std::fstream file(filename, std::ios::in | std::ios::binary | std::ios::ate );
	
	if (!file.is_open())
	{
		return;
	}

	//Get the file size which returns the position
	auto fileSize = file.tellg();
	file.seekp(std::ios::beg);

	BitMapFileHeader fileHeader;

	file.read(reinterpret_cast<char*>(&fileHeader),sizeof(BitMapFileHeader));

	if (fileHeader.bfType != 0x4D42)
	{
		return;
	}

	BitMapInfoHeader infoHeader;
	file.read(reinterpret_cast<char*>(&infoHeader),sizeof(BitMapInfoHeader));

	file.seekg(fileHeader.bfOffBits);

	create(infoHeader.bcWidth, infoHeader.bcHeight,infoHeader.bcBitCount);

	int padding = getPitch() % 4;
	int lineMemoryWidth = getPitch();
	if (padding)
	{
		padding = 4 - padding;
		lineMemoryWidth += padding;
	}

	for (int line = m_height - 1; line >= 0; --line)
	{
		file.seekp(lineMemoryWidth*line+fileHeader.bfOffBits);
		file.read(reinterpret_cast<char*>(&m_pixels[getPitch()*(m_height - 1 - line)]), getPitch());
	
	}

	file.close();

}

Color Image::getPixel(unsigned int x, unsigned int y) const
{
	
	Color color;
	int pixelPos = (y * getPitch()) + (x * getBytesPP());

	color.r = m_pixels[pixelPos + 2];
	color.g = m_pixels[pixelPos + 1];
	color.b = m_pixels[pixelPos + 0];

	return color;
}

void Image::setPixel(unsigned int x, unsigned int y, const Color& color)
{
	int pixelPos = (y * getPitch()) + (x * getBytesPP());
	
	m_pixels[pixelPos + 2] = color.r;
	m_pixels[pixelPos + 1] = color.g;
	m_pixels[pixelPos + 0] = color.b;

	if (getBytesPP() == 4)
	{
		m_pixels[pixelPos + 3] = color.a;
	}
}

void Image::clearColor(const Color& color)
{
	for (int y = 0; y < m_height; ++y)
	{
		for (int x = 0; x < m_width; ++x)
		{
			setPixel(x,y,color);
		}
	}
}

Color Image::getColor(float u, float v)
{
	adjustTextureAddress(u, v, TEXTURE_ADDRESS::MIRROR);
	int x = static_cast<int>(u * (m_width - 1));
	int y = static_cast<int>(v * (m_height - 1));

	return getPixel(x,y);
}

void Image::adjustTextureAddress(float& u, float& v, TEXTURE_ADDRESS::E textureAddressMode)
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

void Image::bitBlit(Image& dest, int x, int y, Color* pColorKey)
{

	//Recorro el pixel array de SOURCE IMAGE
	for (int i = 0; i < m_height; i++)
	{
		for (int j = 0; j < m_width; j++)
		{
			//Guardo el pixel de la SOURCE IMAGE
			Color pxColor = getPixel(j, i);
			if (pColorKey)
			{
				if (pxColor.r == pColorKey->r &&
					pxColor.g == pColorKey->g &&
					pxColor.b == pColorKey->b)
				{
					continue;
				}
			}
			//i y j es el pixel source
			//x y y son un offset, por eso se le suma
			//getpixel tiene el pixel de la imagen source
			//entonces si es 1,1 y mi offset es de 3,3 entonces
			//la imagen empezará en 4,4
			dest.setPixel(j+x,i+y,getPixel(j,i));
		}
	}
}

void Image::bitBlit(Image& source, int x, int y, int srcInitX, 
					int srcInitY, int srcEndX, int srcEndY, 
					Color* pColorKey)
{
	//Si el offset es mayor al tamaño, si la imagen está hasta quien sabe donde y no se ve.
	if (x >= getWidth() || y >= getHeight())
		return;

	//Si no especificamos donde termina la imagen source, lo tomamos como completa.
	if (srcEndX == 0)
		srcEndX == source.getWidth();
	if (srcEndY == 0)
		srcEndY == source.getWidth();
	
	//Si la imagen en x o y está en -3, restamos donde empieza el srcimage menos el offset.
	//Terminamos con igualar el offset a 0 para saber que dibujamos en la destimage a partir
	//del 0,0.
	if (x < 0)
	{
		srcInitX = srcInitX - x;
		x = 0;
	}
	if (y < 0)
	{
		srcInitY = srcInitY - y;
		y = 0;
	}

	//Restamos los valores de donde termina con donde empieza para tomar
	//los valores que ya se habían cambiado
	int realWidth = srcEndX - srcInitX;
	int realHeight = srcEndY - srcInitY;

	//Si la imagen no tiene absolatamente nada de contenido de pixeles, en x o y, no hacemos nada
	if (realWidth < 0 || realHeight < 0)
	{
		return;
	}

	//Si la imagen ya cropeada más el offset están fuera de la pantalla,
	//se encuentra la diferencia entre el tamaño de la img menos el tamaño de la pantalla.
	if (x + realWidth > getWidth()) 
	{
		realWidth -= (x + realWidth) - getWidth();
	}

	if (y + realHeight > getHeight())
	{
		realHeight -= (x + realHeight) - getHeight();
	}

	for (int i = 0; i < realHeight; i++)
	{
		for (int j = 0; j < realWidth; j++)
		{
			Color color = source.getPixel(srcInitX+j,srcInitY+i);
			if (pColorKey)
			{
				if (color.r == pColorKey->r &&
					color.g == pColorKey->g &&
					color.b == pColorKey->b)
				{
					continue;
				}
			}
			setPixel(x+j, y+i, color);
		}
	}
	
}
