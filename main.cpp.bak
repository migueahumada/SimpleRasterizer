#include <iostream>
#include "Image.h"
#include "Texture.h" //Added

#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>


#define _USE_MATH_DEFINES
#include <math.h>

#define WIDTH 1280
#define HEIGHT 720

namespace AXIS {
	enum E {
		X,
		Y,
		Z
	};
}



struct Face {
	
	Triangle tri1;
	Triangle tri2;
};

static Vector3 rot(const Vector3& vec, float angle, AXIS::E axis) {

	float angleRadians = angle * (M_PI / 180.0f);

	switch (axis)
	{
	case AXIS::X:
		return Vector3{ vec.x * 1 + vec.y * 0 + vec.z * 0,
						vec.x * 0 + vec.y * cos(angleRadians) + vec.z * -sin(angleRadians),
						vec.x * 0 + vec.y * sin(angleRadians) + vec.z * cos(angleRadians) };
		break;
	case AXIS::Y:
		return Vector3{ vec.x * cos(angleRadians) + vec.y * 0 + vec.z * sin(angleRadians),
						vec.x * 0 + vec.y * 1 + vec.z * 0,
						vec.x * -sin(angleRadians) + vec.y * 0 + vec.z * cos(angleRadians) };
		break;
	case AXIS::Z:
		return Vector3{ vec.x * cos(angleRadians) + vec.y * -sin(angleRadians) + vec.z * 0,
						vec.x * sin(angleRadians) + vec.y * cos(angleRadians) + vec.z * 0,
						vec.x * 0 + vec.y * 0 + vec.z * 1 };
		break;
	default:
		break;
	}


}

//Given in Degrees
static void rotateX(Vector3& vec, float angle) {

	float angleRadians = angle * (M_PI / 180.0f);

	vec.x = vec.x * 1 + vec.y * 0 + vec.z * 0;
	vec.y = vec.x * 0 + vec.y * cos(angleRadians) + vec.z * -sin(angleRadians);
	vec.z = vec.x * 0 + vec.y * sin(angleRadians) + vec.z * cos(angleRadians);


}
//Given in Degrees
static void rotateY(Vector3& vec, float angle) {

	float angleRadians = angle * (M_PI / 180.0f);

	vec.x = vec.x * cos(angleRadians) + vec.y * 0 + vec.z * sin(angleRadians);
	vec.y = vec.x * 0 + vec.y * 1 + vec.z * 0;
	vec.z = vec.x * -sin(angleRadians) + vec.y * 0 + vec.z * cos(angleRadians);

}

static void rotateZ(Vector3& vec, float angle) {

	float angleRadians = angle * (M_PI / 180.0f);

	vec.x = vec.x * cos(angleRadians) + vec.y * -sin(angleRadians) + vec.z * 0;
	vec.y = vec.x * sin(angleRadians) + vec.y * cos(angleRadians) + vec.z * 0;
	vec.z = vec.x * 0 + vec.y * 0 + vec.z * 1;

}

struct Cube {
	Cube() = default;
	~Cube() = default;

	void initCube(float x, float y, float z, float width, float height, float depth, const Color& color) {
		
		//Front Face
		faces[0].tri1.v1 = { x, y, z, color, 0, 0 };
		faces[0].tri1.v2 = { x + width, y , z, color, 1, 0 };
		faces[0].tri1.v3 = { x, y + height, z, color, 0, 1 };

		faces[0].tri2.v1 = { x + width, y, z, color, 1, 0 };
		faces[0].tri2.v2 = { x + width, y + height, z, color, 1, 1 };
		faces[0].tri2.v3 = { x , y + height, z, color, 0, 1 };

		// TODO: Cambiar estooo
		// Back Face
		faces[1].tri1.v1 = { x, y, z + depth, color, 0, 0 };
		faces[1].tri1.v2 = { x, y + height, z + depth, color, 0, 1 };
		faces[1].tri1.v3 = { x + width, y , z + depth, color, 1, 0 };
			  
		faces[1].tri2.v1 = { x + width, y, z + depth, color, 1, 0 };
		faces[1].tri2.v2 = { x , y + height, z + depth, color, 0, 1 };
		faces[1].tri2.v3 = { x + width, y + height, z + depth, color, 1, 1 };

		// Left Face
		faces[2].tri1.v1 = { x, y, z + depth, color, 0, 0 };
		faces[2].tri1.v2 = { x, y , z, color, 1, 0 };
		faces[2].tri1.v3 = { x, y + height, z + depth, color, 0, 1 };

		faces[2].tri2.v1 = { x, y, z, color, 1, 0 };
		faces[2].tri2.v2 = { x , y + height, z , color, 1, 1 };
		faces[2].tri2.v3 = { x, y + height, z + depth, color, 0, 1 };

		// Right Face
		faces[3].tri1.v1 = { x, y, z + depth, color, 0, 0 };
		faces[3].tri1.v2 = { x, y + height, z + depth, color, 0, 1 };
		faces[3].tri1.v3 = { x, y , z, color, 1, 0 };
			  
		faces[3].tri2.v1 = { x, y, z, color, 1, 0 };
		faces[3].tri2.v2 = { x, y + height, z + depth, color, 0, 1 };
		faces[3].tri2.v3 = { x , y + height, z , color, 1, 1 };

		// Top Face
		faces[4].tri1.v1 = { x, y , z+ depth, color, 0, 0 };
		faces[4].tri1.v2 = { x + width, y , z + depth, color, 1, 0 };
		faces[4].tri1.v3 = { x , y , z, color, 0, 1 };

		faces[4].tri2.v1 = { x + width, y , z + depth, color, 1, 0 };
		faces[4].tri2.v2 = { x + width, y , z , color, 1, 1 };
		faces[4].tri2.v3 = { x , y, z , color, 0, 1 };

		// Bottom Face
		faces[5].tri1.v1 = { x, y + height, z , color, 0, 0 };
		faces[5].tri1.v2 = { x + width , y + height , z, color, 1, 0 };
		faces[5].tri1.v3 = { x , y + height, z + depth, color, 0, 1 };
			  
		faces[5].tri2.v1 = { x + width, y + height, z , color, 1, 0 };
		faces[5].tri2.v2 = { x , y + height, z + depth, color, 0, 1 };
		faces[5].tri2.v3 = { x + width, y + height, z + depth, color, 1, 1 };


	}

	void draw(Image& imgScreen, PixelShader pixelShader, const Vector3& screenCenter, const Vector3& view) {

		static int count = 6;
		
		//Rotate

		for (int i = 0; i < MAX_FACES; i++)
		{
			count--;
			//g_triangle.v1.position = rot(g_triangle.v1.position - g_screenCenter, 5.0f, AXIS::Y) + g_screenCenter;
			faces[i].tri1.v1.position = rot(faces[i].tri1.v1.position - screenCenter, 5.0f, AXIS::X) + screenCenter;
			faces[i].tri1.v2.position = rot(faces[i].tri1.v2.position - screenCenter, 5.0f, AXIS::X) + screenCenter;
			faces[i].tri1.v3.position = rot(faces[i].tri1.v3.position - screenCenter, 5.0f, AXIS::X) + screenCenter;
																								  
			faces[i].tri2.v1.position = rot(faces[i].tri2.v1.position - screenCenter, 5.0f, AXIS::X) + screenCenter;
			faces[i].tri2.v2.position = rot(faces[i].tri2.v2.position - screenCenter, 5.0f, AXIS::X) + screenCenter;
			faces[i].tri2.v3.position = rot(faces[i].tri2.v3.position - screenCenter, 5.0f, AXIS::X) + screenCenter;

			faces[i].tri1.v1.position = rot(faces[i].tri1.v1.position - screenCenter, 5.0f, AXIS::Y) + screenCenter;
			faces[i].tri1.v2.position = rot(faces[i].tri1.v2.position - screenCenter, 5.0f, AXIS::Y) + screenCenter;
			faces[i].tri1.v3.position = rot(faces[i].tri1.v3.position - screenCenter, 5.0f, AXIS::Y) + screenCenter;
																								  
			faces[i].tri2.v1.position = rot(faces[i].tri2.v1.position - screenCenter, 5.0f, AXIS::Y) + screenCenter;
			faces[i].tri2.v2.position = rot(faces[i].tri2.v2.position - screenCenter, 5.0f, AXIS::Y) + screenCenter;
			faces[i].tri2.v3.position = rot(faces[i].tri2.v3.position - screenCenter, 5.0f, AXIS::Y) + screenCenter;
			
			faces[i].tri1.v1.position = rot(faces[i].tri1.v1.position - screenCenter, 5.0f, AXIS::Z) + screenCenter;
			faces[i].tri1.v2.position = rot(faces[i].tri1.v2.position - screenCenter, 5.0f, AXIS::Z) + screenCenter;
			faces[i].tri1.v3.position = rot(faces[i].tri1.v3.position - screenCenter, 5.0f, AXIS::Z) + screenCenter;
																								  
			faces[i].tri2.v1.position = rot(faces[i].tri2.v1.position - screenCenter, 5.0f, AXIS::Z) + screenCenter;
			faces[i].tri2.v2.position = rot(faces[i].tri2.v2.position - screenCenter, 5.0f, AXIS::Z) + screenCenter;
			faces[i].tri2.v3.position = rot(faces[i].tri2.v3.position - screenCenter, 5.0f, AXIS::Z) + screenCenter;
		}

		for (int i = 0; i < MAX_FACES; i++)
		{
			float intensity1;
			float intensity2;

			//Triangulo 1
			Vector3 tri1v1v2 = faces[i].tri1.v2.position - faces[i].tri1.v1.position;
			Vector3 tri1v1v3 = faces[i].tri1.v3.position - faces[i].tri1.v1.position;

			faces[i].tri1.normal = (tri1v1v2 ^ tri1v1v3).normalize();

			intensity1 = faces[i].tri1.normal | view;

			Vector3 tri2v1v2 = faces[i].tri2.v2.position - faces[i].tri2.v1.position;
			Vector3 tri2v1v3 = faces[i].tri2.v3.position - faces[i].tri2.v1.position;

			faces[i].tri2.normal = (tri2v1v2 ^ tri2v1v3).normalize();

			intensity2 = faces[i].tri2.normal | view;

			//if (intensity1 < 0 && intensity2 < 0)
			{
				imgScreen.drawTriangle2D(faces[i].tri1, pixelShader);
				imgScreen.drawTriangle2D(faces[i].tri2, pixelShader);
			}

			//Triangulo 2
			

			/*if (intensity < 0)
			{
				imgScreen.drawTriangle2D(faces[i].tri2, pixelShader);
			}*/

			//Triangulo 2

			//-------------------------------------------
			//Get the normal vector of the triangle
			/*

			Vector3 v1v2 = g_triangle.v2.position - g_triangle.v1.position;
			Vector3 v1v3 = g_triangle.v3.position - g_triangle.v1.position;
			g_triangle.normal = (v1v2 ^ v1v3).normalize();

			Vector3 view = Vector3{ 0, 0, 1 } + g_screenCenter;



			float intensity = g_triangle.normal | view;

			g_cube.draw(g_imgScreen, pixelShader, g_screenCenter);

			if (intensity > 0)
			{
				//g_imgScreen.drawTriangle2D(g_triangle, pixelShader);
			}

			*/
			//-----------------------------------------------
			//imgScreen.drawTriangle2D(faces[i].tri1, pixelShader);
			//imgScreen.drawTriangle2D(faces[i].tri2, pixelShader);
		}

	};

	static const int MAX_FACES = 6;
	Face faces[MAX_FACES];
	
	
};

static SDL_Window* g_window = nullptr;
static SDL_Renderer* g_renderer = nullptr;
static SDL_Texture* g_texture = nullptr;

static Image g_imgScreen;
static Image g_img;
static Texture g_sampleTexture;

static Triangle g_triangle;
static Vector3 g_screenCenter = { WIDTH / 2.0f, HEIGHT / 2.0f,0 };
static Cube g_cube;





static Color pixelShader(float u, float v)
{
	Color color = g_sampleTexture.sample(u, v, TEXTURE_ADDRESS::WRAP, SAMPLER_FILTER::LINEAR).toColor();
	return color;
}

static Color greyPixelShader(float u, float v)
{
	FloatColor color = g_sampleTexture.sample(u, v, TEXTURE_ADDRESS::WRAP, SAMPLER_FILTER::LINEAR);
	unsigned char rV = static_cast<unsigned char>(color.toGrey() * 255);
	return Color{ rV,rV ,rV };
}

static Color kernelPixelShader(float u, float v)
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

	float du = 1.0f / g_sampleTexture.m_image.getWidth();
	float dv = 1.0f / g_sampleTexture.m_image.getHeight();

	auto c00 = g_sampleTexture.sample(u - du, v - dv, TEXTURE_ADDRESS::WRAP, SAMPLER_FILTER::LINEAR);
	auto c01 = g_sampleTexture.sample(u - du, v, TEXTURE_ADDRESS::WRAP, SAMPLER_FILTER::LINEAR);
	auto c02 = g_sampleTexture.sample(u - du, v + dv, TEXTURE_ADDRESS::WRAP, SAMPLER_FILTER::LINEAR);
				   
	auto c10 = g_sampleTexture.sample(u, v - dv, TEXTURE_ADDRESS::WRAP, SAMPLER_FILTER::LINEAR);
	auto c11 = g_sampleTexture.sample(u, v, TEXTURE_ADDRESS::WRAP, SAMPLER_FILTER::LINEAR);
	auto c12 = g_sampleTexture.sample(u, v + dv, TEXTURE_ADDRESS::WRAP, SAMPLER_FILTER::LINEAR);
				   
	auto c20 = g_sampleTexture.sample(u + du, v - dv, TEXTURE_ADDRESS::WRAP, SAMPLER_FILTER::LINEAR);
	auto c21 = g_sampleTexture.sample(u + du, v, TEXTURE_ADDRESS::WRAP, SAMPLER_FILTER::LINEAR);
	auto c22 = g_sampleTexture.sample(u + du, v + dv, TEXTURE_ADDRESS::WRAP, SAMPLER_FILTER::LINEAR);

	float* kernel = kernelSobelX;
	FloatColor finalValX = c00 * kernel[0] + c10 * kernel[1] + c20 * kernel[2] +
		c01 * kernel[3] + c11 * kernel[4] + c21 * kernel[5] +
		c02 * kernel[6] + c12 * kernel[7] + c22 * kernel[8];

	kernel = kernelSobelY;
	FloatColor finalValY = c00 * kernel[0] + c10 * kernel[1] + c20 * kernel[2] +
		c01 * kernel[3] + c11 * kernel[4] + c21 * kernel[5] +
		c02 * kernel[6] + c12 * kernel[7] + c22 * kernel[8];

	//unsigned char grey = static_cast<unsigned char>(finalVal.saturate().toGrey() * 255);
	unsigned char sx = static_cast<unsigned char>(finalValX.saturate().toGrey() * 255);
	unsigned char sy = static_cast<unsigned char>(finalValX.saturate().toGrey() * 255);

	Color normal = { sx, sy, 255, 255 };

	return normal;
}

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{

	if (!SDL_Init(SDL_INIT_VIDEO)) {
		SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}

	if (!SDL_CreateWindowAndRenderer("Soft Rasterizer", WIDTH, HEIGHT, 0, &g_window, &g_renderer)) {
		SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}

	g_imgScreen.create(WIDTH, HEIGHT, 32);
	g_imgScreen.clearColor({0,0,0,255});

	g_img.decode("Coin.bmp");
	g_sampleTexture.createImage(g_img);

	g_texture = SDL_CreateTexture(	g_renderer,
									SDL_PIXELFORMAT_BGRA32,
									SDL_TEXTUREACCESS_STREAMING,
									g_imgScreen.getWidth(),
									g_imgScreen.getHeight());
	if (!g_texture)
	{
		SDL_Log("Couldn't create a streaming texture: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}

	//g_triangle.v1 = { 0, 0, 0, Color{0, 255, 255, 255}, 0, 0 };
	//g_triangle.v2 = { 200, 0, 0, Color{255, 0, 255, 255}, 1, 0 };
	//g_triangle.v3 = { 0, 200, 0, Color{255, 255, 0, 255}, 0, 1 };

	//g_triangle.v1.position = g_triangle.v1.position + g_screenCenter;
	//g_triangle.v2.position = g_triangle.v2.position + g_screenCenter;
	//g_triangle.v3.position = g_triangle.v3.position + g_screenCenter;

	g_cube.initCube(g_screenCenter.x, g_screenCenter.y, g_screenCenter.z, 200,200,200,Color{34,45,56,255});

	return SDL_APP_CONTINUE;  
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
	if (event->type == SDL_EVENT_QUIT) {
		return SDL_APP_SUCCESS;  
	}
	return SDL_APP_CONTINUE;  
}

SDL_AppResult SDL_AppIterate(void* appstate)
{

	FloatColor color = { 0.0f, 0.0f, 0.0f, 0.0f };
	SDL_SetRenderDrawColorFloat(g_renderer,color.r,color.g,color.b,color.a);
	SDL_RenderClear(g_renderer);
	g_imgScreen.clearColor({ 0, 0, 0, 255 });

	//g_imgScreen.setPixel(rand() % g_imgScreen.getWidth(), rand() % g_imgScreen.getHeight(), Color{255,0,0,255 });

	/*rotateY(g_triangle.v1.position, 3.0f);
	rotateY(g_triangle.v2.position, 3.0f);
	rotateY(g_triangle.v3.position, 3.0f);*/
	

	//g_triangle.v1.position = rot(g_triangle.v1.position - g_screenCenter, 5.0f, AXIS::Y) + g_screenCenter;
	//g_triangle.v2.position = rot(g_triangle.v2.position - g_screenCenter, 5.0f, AXIS::Y) + g_screenCenter;
	//g_triangle.v3.position = rot(g_triangle.v3.position - g_screenCenter, 5.0f, AXIS::Y) + g_screenCenter;

	////Get the normal vector of the triangle
	//Vector3 v1v2 = g_triangle.v2.position - g_triangle.v1.position;
	//Vector3 v1v3 = g_triangle.v3.position - g_triangle.v1.position;
	//g_triangle.normal = (v1v2 ^ v1v3).normalize();

	Vector3 view = Vector3{ 0, 0, 1 };

	//float intensity = g_triangle.normal | view;

	
	g_cube.draw(g_imgScreen, pixelShader, g_screenCenter, view);
	
	//if (intensity > 0)
	//{
	//	//g_imgScreen.drawTriangle2D(g_triangle, pixelShader);
	//	
	//}

	SDL_Surface* surface = nullptr;

	SDL_Surface* imgSurface = SDL_CreateSurfaceFrom(g_imgScreen.getWidth(),
		g_imgScreen.getHeight(),
		SDL_PIXELFORMAT_BGRA32,
		g_imgScreen.getPixels(),
		g_imgScreen.getPitch());

	if (SDL_LockTextureToSurface(g_texture, nullptr, &surface))
	{
		SDL_BlitSurface(imgSurface, nullptr, surface, nullptr);

		SDL_UnlockTexture(g_texture);
	}

	SDL_DestroySurface(imgSurface);

	//Draw texture to screen
	SDL_FRect dstRect = {0,0,g_imgScreen.getWidth(),g_imgScreen.getHeight()};
	SDL_RenderTexture(g_renderer, g_texture, nullptr, &dstRect);
	
	//Show in screen
	SDL_RenderPresent(g_renderer);

	


	return SDL_APP_CONTINUE;  
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
	
}