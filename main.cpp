#include <iostream>
#include "Image.h"
#include "Texture.h" //Added


/*
	Caso 1:
		- Width: 776 px
		- Height: 1096 px
		- Bpp = 24 bits = 3 bytes
		- Pitch = 776 * 3 = 2328 bytes
		- Pixel[1,1] = (1 * 2328) + (1 * 3) = 2331
		|     |     |.....|     | -> 2328 bytes
		|     |     |.....|     |
			  ^ -> 2331 bytes recorridos

		|     |     |.....|     |
	*/

int main() {

	//----------Imagen de la pantalla--------
	Image imgScreen;
	imgScreen.create(512, 512, 32);
	//imgScreen.clearColor({ 255,0,255,255 });

	//----------Generar Terreno--------
	Image imgTerrain;
	imgTerrain.decode("Terrain.bmp");
	/*Texture terrainTexture;
	terrainTexture.createImage(imgTerrain);*/

	/*for (int y = 0; y < (imgScreen.getHeight() / imgTerrain.getHeight()) + 1; y++)
		for (int x = 0; x < (imgScreen.getWidth() / imgTerrain.getWidth()) + 1; x++)
			imgScreen.bitBlit(imgTerrain, imgTerrain.getWidth() * x, imgTerrain.getHeight() * y);*/


	//----------Imagen del player--------
	Image imgPlayer;
	imgPlayer.decode("Coin.bmp");
	Texture playerTexture;
	playerTexture.createImage(imgPlayer);

	
	//playerTexture.draw(imgScreen,0,0,150,150,500, 500,TEXTURE_ADDRESS::MIRROR, BLEND_MODE::ADDITIVE);

	//imgScreen.line(565, 788, 400, 230, Color{255,0,0,255});
	
	//imgScreen.bresehamLine(565, 788, 180, 890, Color{255,255,0,255});

	//imgScreen.bresehamLine(300, 300, 600, 300, Color{255,255,0,255});
	
	//imgScreen.lineRectangle(600,600,400,400, Color{ 0,255,255,255 });

	Triangle triangle;

	triangle.v1 = { 150,100,0, Color{255, 255, 255, 255}, 0, 0 };
	triangle.v2 = { 200,200,0, Color{255, 255, 255, 255}, 1, 1 };
	triangle.v3 = { 100,200,0, Color{255, 255, 255, 255}, 0, 1 };

	Vector3 normal;

	//Punta menos cola para sacar el vector
	Vector3 v1v2 = triangle.v2.position - triangle.v1.position;
	Vector3 v1v3 = triangle.v3.position - triangle.v1.position;

	//normal = -v0v1.cross(v0v2).normalize();
	normal = (v1v2 ^ v1v3).normalize();

	Vector3 view = {0, 0, 1};

	float intensity = normal | view;

	//Se compara el producto punto para ver is tienen la misma dirección o no
	if (intensity > 0)
	{

		//imgScreen.fillTriangle(triangle.v1,triangle.v2,triangle.v3, { 56,34,125,255 });

	}

	// <----------------------PIXEL SHADER CALLBACK FUNCTIONS----------------------->
	auto pixelShader = [&playerTexture](float u, float v) -> Color
	{
		Color color = playerTexture.sample(u,v,TEXTURE_ADDRESS::WRAP, SAMPLER_FILTER::LINEAR).toColor();
		return color;
	};

	auto greyPixelShader = [&playerTexture](float u, float v) -> Color
	{
		FloatColor color = playerTexture.sample(u, v, TEXTURE_ADDRESS::WRAP, SAMPLER_FILTER::LINEAR);
		unsigned char rV = static_cast<unsigned char>(color.toGrey() * 255);
		return Color{rV,rV ,rV };
	};

	auto kernelPixelShader = [&playerTexture](float u, float v) -> Color
	{
			float kernelIdentity[] = {
				0, 0, 0,
				0, 1, 0,
				0, 0, 0
			};

			float kernelEmboss[] = {
				-2, -1,  0,
				-1,  1,  1,
				 0,  1,  2
			};

			float kernelSharpen[] = {
				0, -1,  0,
				-1,  5, -1,
				 0, -1,  0
			};

			float kernelBlur[] = {
				0.0625f, 0.125f, 0.0625f,
				0.125f,  0.25f,  0.125f,
				0.0625f, 0.125f, 0.0625f
			};

			float kernelSobelX[] = {
				1,  0,  -1,
				2,  0,  -2,
				1,  0,  -1
			};

			float kernelSobelY[] = {
				 1,  2,   1,
				 0,  0,   0,
				-1, -2,  -1
			};

			

		float du = 1.0f / playerTexture.m_image.getWidth();
		float dv = 1.0f / playerTexture.m_image.getHeight();

		auto c00 = playerTexture.sample(u - du, v - dv, TEXTURE_ADDRESS::WRAP, SAMPLER_FILTER::LINEAR);
		auto c01 = playerTexture.sample(u - du, v ,		TEXTURE_ADDRESS::WRAP, SAMPLER_FILTER::LINEAR);
		auto c02 = playerTexture.sample(u - du, v + dv, TEXTURE_ADDRESS::WRAP, SAMPLER_FILTER::LINEAR);
		
		auto c10 = playerTexture.sample(u, v - dv,	TEXTURE_ADDRESS::WRAP, SAMPLER_FILTER::LINEAR);
		auto c11 = playerTexture.sample(u, v,		TEXTURE_ADDRESS::WRAP, SAMPLER_FILTER::LINEAR);
		auto c12 = playerTexture.sample(u, v + dv,	TEXTURE_ADDRESS::WRAP, SAMPLER_FILTER::LINEAR);
		 
		auto c20 = playerTexture.sample(u + du, v - dv, TEXTURE_ADDRESS::WRAP, SAMPLER_FILTER::LINEAR);
		auto c21 = playerTexture.sample(u + du, v,		TEXTURE_ADDRESS::WRAP, SAMPLER_FILTER::LINEAR);
		auto c22 = playerTexture.sample(u + du, v + dv, TEXTURE_ADDRESS::WRAP, SAMPLER_FILTER::LINEAR);
		
		float* kernel = kernelSobelX;
		FloatColor finalValX =	c00 * kernel[0] + c10 * kernel[1] + c20 * kernel[2] +
								c01 * kernel[3] + c11 * kernel[4] + c21 * kernel[5] +
								c02 * kernel[6] + c12 * kernel[7] + c22 * kernel[8];

		kernel = kernelSobelY;
		FloatColor finalValY =	c00 * kernel[0] + c10 * kernel[1] + c20 * kernel[2] +
								c01 * kernel[3] + c11 * kernel[4] + c21 * kernel[5] +
								c02 * kernel[6] + c12 * kernel[7] + c22 * kernel[8];
		


		//unsigned char grey = static_cast<unsigned char>(finalVal.saturate().toGrey() * 255);
		unsigned char sx = static_cast<unsigned char>(finalValX.saturate().toGrey() * 255);
		unsigned char sy = static_cast<unsigned char>(finalValX.saturate().toGrey() * 255);

		Color normal = { sx, sy, 255, 255 };


		return normal;
	};

	Triangle tri;
	/*tri.v1 = { 250,600,0, Color{255, 255, 255, 255}, 0, 0 };
	tri.v2 = { 200,400,0, Color{255, 255, 255, 255}, 1, 1 };
	tri.v3 = { 500,200,0, Color{255, 255, 255, 255}, 0, 1 };

	imgScreen.drawTriangle2D(tri);*/

	tri.v1 = { 0,0,0, Color{0, 255, 255, 255}, 0, 0 };
	tri.v2 = { (float)imgScreen.getWidth(),0,0, Color{255, 0, 255, 255}, 1, 0};
	tri.v3 = { 0,(float)imgScreen.getHeight(),0, Color{255, 255, 0, 255}, 0, 1};
	

	imgScreen.drawTriangle2D(tri, kernelPixelShader);

	tri.v1 = { (float)imgScreen.getWidth(),0,0, Color{0, 255, 255, 255}, 1, 0};
	tri.v2 = { 0,(float)imgScreen.getHeight(),0, Color{255, 0, 255, 255}, 0, 1};
	tri.v3 = { (float)imgScreen.getWidth(),(float)imgScreen.getHeight(),0, Color{255, 255, 0, 255}, 1, 1};

	imgScreen.drawTriangle2D(tri, kernelPixelShader);

	//imgScreen.bresehamCircle(240, 400,120, Color{66,77,123,255});

	imgScreen.encode("Screen.bmp");

	return 0;


}