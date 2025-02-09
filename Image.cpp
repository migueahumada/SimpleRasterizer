#include "Image.h"
#include <iostream>

void Image::create(int width, int height, int bpp)
{
	m_width = width; //8
	m_height = height; //8
	m_bpp = bpp;
	m_pixels = new unsigned char[getPitch() * m_height];

}

void Image::encode(const char* filename)
{
	//Creates file
	std::fstream file(filename,std::ios::binary | std::ios::out);
	
	if (!file.is_open()) //Not open? do nothing
	{
		return;
	}

	BitMapFileHeader fileHeader;
	memset(&fileHeader, 0, sizeof(BitMapFileHeader));
	BitMapSaveHeader bmpInfo;
	memset(&bmpInfo, 0, sizeof(BitMapSaveHeader));

	int padding = getPitch() % 4;
	int lineMemoryWidth = getPitch();

	if (padding)
	{
		padding = 4 - padding;
		lineMemoryWidth += padding;
	}

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

	int ready = 1;

}

Color Image::getPixel(unsigned int x, unsigned int y) const{

	if (!m_pixels)
	{
		return Color{0,0,0,0};
	}

	Color color;
	int pixelPos = (y * getPitch()) + (x * getBytesPP());

	color.r = m_pixels[pixelPos + 2];
	color.g = m_pixels[pixelPos + 1];
	color.b = m_pixels[pixelPos + 0];
	if (getBytesPP() == 4)
	{
		color.a = m_pixels[pixelPos + 3];
	}
	else {
		color.a = 255;
	}

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

//Color Image::getColor(float u, float v)
//{
//	adjustTextureAddress(u, v, TEXTURE_ADDRESS::MIRROR);
//	int x = static_cast<int>(u * (m_width - 1));
//	int y = static_cast<int>(v * (m_height - 1));
//
//	return getPixel(x,y);
//}



//void Image::bitBlit(Image& dest, int x, int y, Color* pColorKey)
//{
//
//	//Recorro el pixel array de SOURCE IMAGE
//	for (int i = 0; i < m_height; i++)
//	{
//		for (int j = 0; j < m_width; j++)
//		{
//			//Guardo el pixel de la SOURCE IMAGE
//			Color pxColor = getPixel(j, i);
//			if (pColorKey)
//			{
//				if (pxColor.r == pColorKey->r &&
//					pxColor.g == pColorKey->g &&
//					pxColor.b == pColorKey->b)
//				{
//					continue;
//				}
//			}
//			//i y j es el pixel source
//			//x y y son un offset, por eso se le suma
//			//getpixel tiene el pixel de la imagen source
//			//entonces si es 1,1 y mi offset es de 3,3 entonces
//			//la imagen empezará en 4,4
//			dest.setPixel(j+x,i+y,getPixel(j,i));
//		}
//	}
//}

void Image::bitBlit(Image& source, int x, int y, int srcInitX, 
					int srcInitY, int srcEndX, int srcEndY, 
					Color* pColorKey)
{
	//Si el offset es mayor al tamaño, si la imagen está hasta quien sabe donde y no se ve.
	if (x >= getWidth() || y >= getHeight())
		return;

	//Si no especificamos donde termina la imagen source, lo tomamos como completa.
	if (srcEndX == 0)
		srcEndX = source.getWidth();
	if (srcEndY == 0)
		srcEndY = source.getWidth();
	
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
		realHeight -= (y + realHeight) - getHeight();
	}

	for (int i = 0; i < realHeight; ++i)
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

void Image::line(int x0, int y0, int x1, int y1, const Color& color)
{
	clipLine(x0, y0,x1,y1,color);

	float dx = x1 - x0; 
	float dy = y1 - y0; 

	float steps = std::max(std::abs(dx), std::abs(dy)); 

	float xInc = dx / steps; 
	float yInc = dy / steps; 

	float x = x0;
	float y = y0;

	//

	for (int i = 0; i < int(steps); i++)
	{
		setPixel(int(x),int(y),color);
		x += xInc;
		y += yInc;
	}
}

void Image::bresehamLine(int x0, int y0, int x1, int y1, const Color& color)
{
	clipLine(x0, y0, x1, y1, color);

	int dx = abs(x1 - x0);
	int dy = abs(y1 - y0);

	int sx = x0 < x1 ? 1 : -1;
	int sy = y0 < y1 ? 1 : -1;

	int err = dx - dy;
	int e2;

	while (x0 != x1 || y0 != y1)
	{
		setPixel(x0, y0, color);
		e2 = 2 * err;
		if (e2 > -dy)
		{
			err -= dy;
			x0 += sx;
		}
		if (e2 < dx)
		{
			err += dx;
			y0 += sy;
		}
	}
}

int Image::computeRegionCode(int x, int y, int xMin, int yMin, int xMax, int yMax) {

	int code = INSIDE; // Es 0000

	if (x < xMin)					// punto a la izq de la pantalla en x?
	{									// 0000
		code |= LEFT;					// 0001	OR
	}									// 0001 = 1 = LEFT
	else if (x > xMax)				// punto la dcha de la pantalla en x?
	{									// 0000
		code |= RIGHT;					// 0010	OR
	}									// 0010 = 2 = RIGHT

	if (y < yMin)					// punto arriba de pantalla en y?
	{									// 0010
		code |= BOTTOM;					// 0100	OR
	}									// 0110 = 6 = BOTTOM & RIGHT
	else if (y > yMax)				// punto abajo de pantalla en y?
	{									// 0001
		code |= TOP;					// 1000	OR
	}									// 1001 = 9 = TOP & LEFT

	return code;

}

bool Image::clipLine(int& x0, int& y0, int& x1, int& y1, const Color& color)
{
	int xMin = 0;
	int yMin = 0;
	int xMax = m_width;
	int yMax = m_height;

	int code0 = computeRegionCode(x0, y0, xMin, yMin, xMax, yMax);	// 1001 TOP & LEFT
	int code1 = computeRegionCode(x1, y1, xMin, yMin, xMax, yMax);	// 0110 BOTTOM & RIGHT

	while (true)
	{
		if (!(code0 | code1))		// Ejemplo: puntos dentro de la pantalla
		{								// 0000
			return true;				// 0000
		}								// 0000 -> Both lines are inside

		else if (code0 & code1)		//Ejemplo: La línea está completamente fuera
		{
			return false;
		}

		else {
			//Al menos un punto está fuera del rectángulo
			int x = 0;
			int y = 0;
			int codeOut = code0 ? code0 : code1;

			if (codeOut & TOP) {								// 1001 TOP & LEFT
				// 1000 TOP
				x = x0 + (x1 - x0) * (yMax - y0) / (y1 - y0);	// 1000 OK!
				y = yMax;
			}
			else if (codeOut & BOTTOM) {						// 0101 BOT & LEFT
				x = x0 + (x1 - x0) * (yMin - y0) / (y1 - y0);	// 0100 BOT
				y = yMin;										// 0100 OK!
			}
			else if (codeOut & RIGHT) {
				y = y0 + (y1 - y0) * (xMax - x0) / (x1 - x0);
				x = xMax;
			}
			else if (codeOut & LEFT) {
				y = y0 + (y1 - y0) * (xMin - x0) / (x1 - x0);
				x = xMin;
			}
			if (codeOut == code0)
			{
				x0 = x;
				y0 = y;
				code0 = computeRegionCode(x0, y0, xMin, yMin, xMax, yMax);
			}
			else {
				x1 = x;
				y1 = y;
				code1 = computeRegionCode(x1, y1, xMin, yMin, xMax, yMax);
			}
		}
	}
	return false;
}

void Image::lineRectangle(int x0, int y0, int width, int height, const Color& color)
{
	for (int i = 0; i < height - 1; i++)
	{
		bresehamLine(x0,y0 + i,x0 + width ,y0 + i, color);
	}
}

void Image::fillTriangle(const Vertex& v0, const Vertex& v1, const Vertex& v2, const Color& color) {

	Vector3 uNom = (v1.position - v0.position).normalize();

	Vector3 vNom = (v2.position - v0.position).normalize();
	
	float maxSize = (v1.position - v0.position).size();

	Vector3 offset = v0.position;

	while (uNom.size() < maxSize && vNom.size() < maxSize) {

		bresehamLine(vNom.x + offset.x, vNom.y + offset.y, uNom.x + offset.x, uNom.y + offset.y, color);


		uNom = uNom + (v1.position - v0.position).normalize();
		vNom = vNom + (v2.position - v0.position).normalize();
	}





	

}

void Image::drawBottomTri(const Triangle& tri, const PixelShader& pixelShader)
{
	Vertex v1 = tri.v1, v2 = tri.v2, v3 = tri.v3;
	if (v3.position.x < v2.position.x) std::swap(v2, v3);

	int height = v3.position.y - v1.position.y;
	if (height <= 0) return;

	float dx_left = static_cast<float>(v2.position.x - v1.position.x) / height;
	float dx_right = static_cast<float>(v3.position.x - v1.position.x) / height;

	float du_left = (v2.u - v1.u) / height;
	float dv_left = (v2.v - v1.v) / height;
	float du_right = (v3.u - v1.u) / height;
	float dv_right = (v3.v - v1.v) / height;

	float xs = v1.position.x, xe = v1.position.x;
	float us = v1.u, vs = v1.v;
	float ue = v1.u, ve = v1.v;

	for (int y = v1.position.y; y <= v3.position.y; ++y)
	{
		int left = static_cast<int>(xs);
		int right= static_cast<int>(xe);
		if (left > right) std::swap(left, right);
		
		float u = us, v = vs;
		float du = (ue - us) / (right - left + 1);
		float dv = (ve - vs) / (right - left + 1);

		for (int x = std::max(0, left); x <= std::min(m_width - 1, right); ++x)
		{

			Color pixelColor = pixelShader(u, v);

			setPixel(x, y, pixelColor);
			u += du;
			v += dv;
		}

		xs += dx_left;
		xe += dx_right;
		us += du_left;
		vs += dv_left;
		ue += du_right;
		ve += dv_right;

	}
}

void Image::drawTopTri(const Triangle& tri, const PixelShader& pixelShader)
{
	Vertex v1 = tri.v1, v2 = tri.v2, v3 = tri.v3;
	if (v2.position.x < v1.position.x) std::swap(v1, v2);

	int height = v3.position.y - v1.position.y;
	if (height <= 0) return;

	float dx_left = static_cast<float>(v3.position.x - v1.position.x) / height;
	float dx_right = static_cast<float>(v3.position.x - v2.position.x) / height;

	float du_left = (v3.u - v1.u) / height;
	float dv_left = (v3.v - v1.v) / height;
	float du_right = (v3.u - v2.u) / height;
	float dv_right = (v3.v - v2.v) / height;

	float xs = v1.position.x, xe = v2.position.x;
	float us = v1.u, vs = v1.v;
	float ue = v2.u, ve = v2.v;

	for (int y = v1.position.y; y <= v3.position.y; ++y)
	{
		int left = static_cast<int>(xs);
		int right = static_cast<int>(xe);
		if (left > right) std::swap(left, right);

		float u = us, v = vs;
		float du = (ue - us) / (right - left + 1);
		float dv = (ve - vs) / (right - left + 1);

		for (int x = std::max(0, left); x <= std::min(m_width - 1, right); ++x)
		{

			Color pixelColor = pixelShader(u,v);

			setPixel(x, y, pixelColor);
			u += du;
			v += dv;
		}

		xs += dx_left;
		xe += dx_right;
		us += du_left;
		vs += dv_left;
		ue += du_right;
		ve += dv_right;

	}
}

void Image::drawTriangle2D(const Triangle& tri, const PixelShader& pixelShader)
{
	Vertex v1 = tri.v1, v2 = tri.v2, v3 = tri.v3;

	if (v1.position.y > v2.position.y) std::swap(v1, v2);
	if (v1.position.y > v3.position.y) std::swap(v1, v3);
	if (v2.position.y > v3.position.y) std::swap(v2, v3);
	
	if (v2.position.y == v3.position.y)
	{
		drawBottomTri({ v1, v2, v3 }, pixelShader); // Triángulo con base abajo
	}
	else if (v1.position.y == v2.position.y)
	{
		drawTopTri({ v1, v2, v3 }, pixelShader); //Triángulo con base arriba
	}
	else 
	{

		//Fórmula de interpolación despejada en x -> x = x1 + ( (x2 - x1) * (y - y1) / (y2 - y1))
		//El número 0.5 es para poder redondear el entero en vez de truncarlo.
		int new_x = v1.position.x + (int)(0.5 + (float)(v2.position.y - v1.position.y) * 
												(float)(v3.position.x - v1.position.x) / 
												(float)(v3.position.y - v1.position.y));

		FloatColor c1(v1.color), c2(v2.color), c3(v3.color);
		
		FloatColor new_color = c1 +	(c3 - c1) * ((v2.position.y - v1.position.y) /
									(v3.position.y - v1.position.y));
										
		float new_u = v1.u + ((v2.position.y - v1.position.y) * 
							 (v3.u - v1.u) / (v3.position.y - v1.position.y));
		
		float new_v = v1.u + ((v2.position.y - v1.position.y) * 
							 (v3.v - v1.v) / (v3.position.y - v1.position.y));
		
		Vertex new_vtx = { new_x, v2.position.y, 0.0f, new_color.toColor(), new_u, new_v };

		drawBottomTri({ v1, new_vtx, v2 }, pixelShader);
		drawTopTri({ v2, new_vtx, v3 }, pixelShader);
	}
	
}

void Image::bresehamCircle(int x0, int y0, int radius, const Color& color)
{
	int xMin = 0;
	int yMin = 0;
	int xMax = m_width;
	int yMax = m_height;

	int x = radius;
	int y = 0;
	int err = 0;

	while (x >= y)
	{
		setPixel(x0 + x, y0 + y, color);
		setPixel(x0 + y, y0 + x, color);
		setPixel(x0 - y, y0 + x, color);
		setPixel(x0 - x, y0 + y, color);
		setPixel(x0 - x, y0 - y, color);
		setPixel(x0 - y, y0 - x, color);
		setPixel(x0 + y, y0 - x, color);
		setPixel(x0 + x, y0 - y, color);
		
		if (err <= 0) 
		{
			y += 1;
			err += 2 * y + 1;
		}
		if (err > 0) 
		{
			x -= 1;
			err -= 2 * x + 1;
		}
	}
}

//   ******* ->
//   ******* ->
//   ******* ->
//   ******* ->
