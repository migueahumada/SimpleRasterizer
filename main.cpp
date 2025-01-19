#include <iostream>
#include "Image.h"

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

	

	Image imgScreen;
	imgScreen.create(1920, 1080, 24);
	imgScreen.clearColor({0,0,255,0});
	
	
	Image img;
	img.decode("Player.bmp");
	

	Color colorKey = { 255,0,255,255 };
	imgScreen.bitBlit(img,0, 23,0,0,280,300, &colorKey);

	for (int y = 0; y < imgScreen.getHeight(); ++y)
	{
		for (int x = 0; x < imgScreen.getWidth(); x++)
		{
			float u = static_cast<float>(x) / (imgScreen.getWidth() - 1);
			float v = static_cast<float>(y) / (imgScreen.getHeight() - 1);

			imgScreen.setPixel(x, y, img.getColor(u , v ));
		}
	}
	Color refColor = img.getColor(0.5f,  0.5f);

	img.encode("TempImage.bmp");
	imgScreen.encode("Screen.bmp");
	return 0;
}