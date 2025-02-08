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
	imgScreen.create(1920, 1080, 24);
	//imgScreen.clearColor({ 255,0,255,255 });

	//----------Generar Terreno--------
	Image imgTerrain;
	imgTerrain.decode("Terrain.bmp");
	/*Texture terrainTexture;
	terrainTexture.createImage(imgTerrain);*/

	for (int y = 0; y < (imgScreen.getHeight() / imgTerrain.getHeight()) + 1; y++)
		for (int x = 0; x < (imgScreen.getWidth() / imgTerrain.getWidth()) + 1; x++)
			imgScreen.bitBlit(imgTerrain, imgTerrain.getWidth() * x, imgTerrain.getHeight() * y);

	/*for (int y = 0; y < imgScreen.getHeight(); ++y)
	{
		for (int x = 0; x < imgScreen.getWidth(); x++)
		{
			float u = static_cast<float>(x) / (imgScreen.getWidth() - 1);
			float v = static_cast<float>(y) / (imgScreen.getHeight() - 1);

			imgScreen.setPixel(x, y, terrainTexture.getColor(u,v,TEXTURE_ADDRESS::WRAP).toColor());
		}
	}*/

	//----------Imagen del player--------
	Image imgPlayer;
	imgPlayer.decode("Player.bmp");
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

		imgScreen.fillTriangle(triangle.v1,triangle.v2,triangle.v3, { 56,34,125,255 });

	}

	Triangle tri;
	tri.v1 = { 250,600,0, Color{255, 255, 255, 255}, 0, 0 };
	tri.v2 = { 200,400,0, Color{255, 255, 255, 255}, 1, 1 };
	tri.v3 = { 500,200,0, Color{255, 255, 255, 255}, 0, 1 };

	imgScreen.drawTriangle2D(tri);

	tri.v1 = { 500,900,0, Color{0, 255, 255, 255}, 0, 0 };
	tri.v2 = { 800,200,0, Color{255, 0, 255, 255}, 1, 1 };
	tri.v3 = { 200,230,0, Color{255, 255, 0, 255}, 0, 1 };

	tri.v1.position.x += 400;
	tri.v2.position.x += 400;
	tri.v3.position.x += 400;

	imgScreen.drawTriangle2D(tri);

	imgScreen.bresehamCircle(240, 400,120, Color{66,77,123,255});

	imgScreen.encode("Screen.bmp");

	return 0;


}