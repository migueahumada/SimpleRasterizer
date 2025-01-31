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

struct Vector3 {
	float x;
	float y;
	float z;

	Vector3 operator+(const Vector3& v) const{
		return {x + v.x, y + v.y, z + v.z };
	}

	Vector3 operator-(const Vector3& v) const {
		return { x - v.x, y - v.y, z - v.z };
	}

	Vector3 operator*(float scalar) const {
		return { x * scalar, y * scalar, z * scalar };
	}

	Vector3 operator/(float scalar) const {
		return {x / scalar, y / scalar, z / scalar };
	}

	Vector3 operator-() const {
		return { -x , -y , -z };
	}

	Vector3 cross(const Vector3& v) const {
		return { y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x };
	}

	Vector3 operator^(const Vector3& v) const {
		return cross(v);
	}

	float size() const{
		return sqrt(x * x + y * y + z * z);
	}

	Vector3 normalize() const {
		float invLength = 1.0f / size();
		//float length = sqrt(x * x + y * y + z * z);
		return{	x * invLength,
				y * invLength,
				z * invLength };
	}

	float dot(const Vector3& v) const {
		return x * v.x + y * v.y + z * v.z;
	}

	float operator|(const Vector3& v) const {
		return dot(v);
	}
	
};

struct VERTEX 
{
	//Screen coordinates
	Vector3 position;
	
};

struct TRIANGLE {
	VERTEX v0;
	VERTEX v1;
	VERTEX v2;
};

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

	imgScreen.bresehamLine(300, 300, 600, 300, Color{255,255,0,255});
	
	imgScreen.lineRectangle(600,600,400,400, Color{ 0,255,255,255 });

	TRIANGLE triangle;

	triangle.v0 = { 150,100,0 };
	triangle.v1 = { 200,200,0 };
	triangle.v2 = { 100,200,0 };

	Vector3 normal;

	//Punta menos cola para sacar el vector
	Vector3 v0v1 = triangle.v1.position - triangle.v0.position;
	Vector3 v0v2 = triangle.v2.position - triangle.v0.position;

	//normal = -v0v1.cross(v0v2).normalize();
	normal = -(v0v1 ^ v0v2).normalize();

	Vector3 view = {0, 0, 1};

	//float intensity = normal.dot(view);
	float intensity = normal | view;

	//Se compara el producto punto para ver is tienen la misma dirección o no
	if (intensity > 0)
	{
		imgScreen.bresehamLine(triangle.v0.position.x, triangle.v0.position.y, triangle.v1.position.x, triangle.v1.position.y, { 255,0,255,255 });
		imgScreen.bresehamLine(triangle.v1.position.x, triangle.v1.position.y, triangle.v2.position.x, triangle.v2.position.y, { 255,0,255,255 });
		imgScreen.bresehamLine(triangle.v2.position.x, triangle.v2.position.y, triangle.v0.position.x, triangle.v0.position.y, { 255,0,255,255 });

	}
	imgScreen.encode("Screen.bmp");

	return 0;


}