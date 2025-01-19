#pragma once
#include <iostream>
#include <fstream>

/*
*	Necesitaremos:
*	- Width
*	- Height
* 
*	Métodos:
*	- Encode -> para codificar información
*	- Decode -> para descodificar información
*/

/*
*	Para representar color necesitaremos una estructura que
*	guarde lo que llevaría un color , o sea RGBA.
*	Se guarda en char por que el tamaño es de 1 byte.
*	Es unsigned para que tome valores de 0 a 255
*/

struct Color { // 4 bytes
	unsigned char r; //1 byte
	unsigned char g; //1 byte
	unsigned char b; //1 byte
	unsigned char a; //1 byte
};

/// <summary>
/// Struct of the file header
/// </summary>
/// <param name="signature:"> 'BM' </param>
/// <param name="fileSize:">size of file in bytes</param>
/// <returns>A struct of the file header</returns>
#pragma pack(push,2)
struct BitMapFileHeader { //size = 14
	unsigned short bfType;
	unsigned long bfSize;
	unsigned short bfReserved1;
	unsigned short bfReserved2;
	unsigned long bfOffBits;
};
#pragma pack(pop)

struct BitMapInfoHeader { //size = 16
	unsigned long bcSize; 
	unsigned long bcWidth;
	unsigned long bcHeight;
	unsigned short bcPlanes;
	unsigned short bcBitCount;
};

struct BitMapSaveHeader {
	BitMapInfoHeader coreHeader;
	unsigned long	biCompression;
	unsigned long	biSizeImage;
	long			biXPelsPerMeter;
	long			biYPelsPerMeter;
	unsigned long	biClrUsed;
	unsigned long	biClrImportant;
};

class Image
{
public:
	Image();
	~Image();
	
	void create(int width, int height, int bpp);
	void encode(const char* filename);
	void decode(const char* filename);

	int getWidth() const {
		return m_width;
	}

	int getHeight() const {
		return m_height;
	}

	//The amount of data in a line of the pixel array in bytes.
	int getPitch() const {
		return getBytesPP() * m_width;
	}

	//Bytes per pixel
	int getBytesPP() const {
		return m_bpp / 8;
		//return m_bpp >> 3
	}

	const int getPadBufferSize(){
		return m_bpp / 8;
	}

	Color getPixel(unsigned int x, unsigned int y) const;
	void setPixel(unsigned int x, unsigned int y, const Color& color);
	void clearColor(const Color& color);

	Color getColor(float u, float v);
	void adjustTextureAddress(float& u, float& v);

	void bitBlit(Image& dest, int x, int y, Color* pColorKey);
	void bitBlit(Image& dest,
				int x,
				int y,
				int srcInitX,
				int srcInitY,
				int srcEndX,
				int srcEndY,
				Color* pColorKey);
protected:
	int m_width;
	int m_height;

	//m_pixels -> is an array of colors
	//Instead of representing the pixels as a
	// Color struct, they will be represented as
	// 1 byte of memory and we will set the amount of 
	// information, each pixel will have
	//This way each pixel will be 1 byte
	unsigned char* m_pixels;

	// Pixel(1 byte of memory) * (bitsPerPixel(24 bits) / 8)
	// Size of a pixel -> pixel(bytes) * (bpp/8) = size of info in a pixel
	// Now we know the size of a pixel


	//char -> 1 byte -> 1111 1111 -> greatest 255

	//Bits per pixel -> how much info is in one pixel
	int m_bpp;


};
//Caso 1
// |Color|Color|
// |Color|Color|
// 
// -Bits Per Pixel: 8 bits | 1 byte
// -PixelSize: 1 byte
// -Pitch: 2 bytes
// -Padding: 0 bytes

//Caso 2
// |Color|Color|     |     |
// |Color|Color|     |     |
// |Color|Color|     |     |
// -Resolution: 2 x 3
// -Bits Per Pixel: 8 bits | 1 byte 
// -PixelSize: 1 byte
// -Pitch: 2 bytes
// -Padding: 0 bytes

//(1,1)
// (1 * 4 bytes)



//|Color| -> 24 bits -> 3 bytes
//|Color|Color| -> 6 bytes

//|Color|Color|Color|Color| -> 12 bytes
//|  3  |  3  |  3  |  3  | -> 12 bytes



/*
*
***********************
***********************
***********************
***********************
***********************
***********************
***********************
***********************
***********************
***********************
*
*
*/

