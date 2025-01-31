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

	TRIANGLE triangle;

	triangle.v0 = { 150,100,0 };
	triangle.v1 = { 200,200,0 };
	triangle.v2 = { 100,200,0 };

	//triangle.v0 = { 150,100,0 };
	//triangle.v1 = { 270,200,0 };
	//triangle.v2 = { 100,270,0 };

	Vector3 normal;

	//Punta menos cola para sacar el vector
	Vector3 v0v1 = triangle.v1.position - triangle.v0.position;
	Vector3 v0v2 = triangle.v2.position - triangle.v0.position;

	//normal = -v0v1.cross(v0v2).normalize();
	normal = (v0v1 ^ v0v2).normalize();

	Vector3 view = {0, 0, 1};

	//float intensity = normal.dot(view);
	float intensity = normal | view;

	//Se compara el producto punto para ver is tienen la misma dirección o no
	if (intensity > 0)
	{
		////1 u
		//imgScreen.bresehamLine(triangle.v0.position.x, triangle.v0.position.y, triangle.v1.position.x, triangle.v1.position.y, { 255,0,255,255 });
		////2
		//imgScreen.bresehamLine(triangle.v1.position.x, triangle.v1.position.y, triangle.v2.position.x, triangle.v2.position.y, { 255,0,255,255 });
		////3 v
		//imgScreen.bresehamLine(triangle.v2.position.x, triangle.v2.position.y, triangle.v0.position.x, triangle.v0.position.y, { 255,0,255,255 });

		Vector3 u = triangle.v1.position - triangle.v0.position;
		Vector3 v = triangle.v2.position - triangle.v0.position;

		Vector3 w1 = v * ( (u | v) / (v.size() * v.size()) );
		Vector3 w2 = u - w1;

		//Vector proyectado
		/*imgScreen.bresehamLine(	triangle.v0.position.x,
								triangle.v0.position.y,
								w1.x + triangle.v0.position.x,
								w1.y + triangle.v0.position.y,
								{56,34,125,255});*/

		//Vector ortogonal
		/*imgScreen.bresehamLine(	w1.x + triangle.v0.position.x,
								w1.y + triangle.v0.position.y,
								triangle.v1.position.x,
								triangle.v1.position.y,
								{ 56,34,125,255 });*/

		imgScreen.fillTriangle(triangle.v0,triangle.v1,triangle.v2, { 56,34,125,255 });

		

	}
	imgScreen.encode("Screen.bmp");

	return 0;


}