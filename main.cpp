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

	
	playerTexture.draw(imgScreen,0,0,150,150,500, 500,TEXTURE_ADDRESS::MIRROR, BLEND_MODE::ADDITIVE);

	imgScreen.line(565, 788, 400, 230, Color{255,0,0,255});
	
	imgScreen.bresehamLine(565, 788, 180, 890, Color{255,255,0,255});

	imgScreen.encode("Screen.bmp");

	return 0;


}