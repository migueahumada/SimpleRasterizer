#include <iostream>
#include "Image.h"
#include "Texture.h" //Added

#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

/* We will use this renderer to draw into this window every frame. */
static SDL_Window* g_window = nullptr;
static SDL_Renderer* g_renderer = nullptr;
static SDL_Texture* g_texture = nullptr;

static Image g_imgScreen;
static Image g_img;
static Texture g_samplerTexture;

static Color pixelShader(float u, float v)
{
	Color color = g_samplerTexture.sample(u, v, TEXTURE_ADDRESS::WRAP,SAMPLER_FILTER::LINEAR).toColor();
	return color;
}

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
	SDL_SetAppMetadata("Example Renderer Clear", "1.0", "com.example.renderer-clear");

	if (!SDL_Init(SDL_INIT_VIDEO)) {
		SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}

	if (!SDL_CreateWindowAndRenderer("Soft Rasterizer", 780, 1000, SDL_WINDOW_RESIZABLE, &g_window, &g_renderer)) {
		SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}

	g_imgScreen.create(512, 512, 32);
	g_imgScreen.clearColor({ 255,0,0,255 });
	g_img.decode("Player.bmp");	
	g_samplerTexture.createImage(g_img);

	Triangle tri;
	tri.v1 = { -300,800,0, Color{0, 255, 255, 255}, 1, 0 };
	tri.v2 = { 200,300,0, Color{255, 0, 255, 255}, 0, 1 };
	tri.v3 = { 300,300,0, Color{255, 255, 0, 255}, 1, 1 };

	g_imgScreen.drawTriangle2D(tri, pixelShader);

	tri.v1 = { 200,200,0, Color{255, 0, 0, 255}, 0, 0 };
	tri.v2 = { 300,200,0, Color{0, 255, 0, 255}, 1, 0 };
	tri.v3 = { 200,300,0, Color{0, 255, 0, 255}, 0, 1 };

	g_imgScreen.drawTriangle2D(tri, pixelShader);
	
	g_texture = SDL_CreateTexture(	g_renderer, 
									SDL_PIXELFORMAT_BGRA32,
									SDL_TEXTUREACCESS_STREAMING, 
									g_imgScreen.getWidth(), 
									g_imgScreen.getHeight());
	
	if (!g_texture)
	{
		SDL_Log("Coudln't create straming texture! %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}

	
	
	return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
	if (event->type == SDL_EVENT_QUIT) {
		return SDL_APP_SUCCESS;  /* end the program, reporting success to the OS. */
	}
	return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void* appstate)
{
	FloatColor color = {0.0f,0.0f,0.0f,0.0f};
	SDL_SetRenderDrawColorFloat(g_renderer, color.r, color.g, color.b, color.a);  /* new color, full alpha. */
	SDL_RenderClear(g_renderer);
	
	g_imgScreen.setPixel(rand() % g_imgScreen.getWidth(), rand() % g_imgScreen.getHeight(), Color{ 255,255,255,255 });

	SDL_Surface* surface = nullptr;

	SDL_Surface* imgSurface = SDL_CreateSurfaceFrom(g_imgScreen.getWidth(),
													g_imgScreen.getHeight(), 
													SDL_PIXELFORMAT_BGRA32,
													g_imgScreen.getPixels(),
													g_imgScreen.getPitch());

	//La función lock -> driver hará un espacio de memoria igual en el hardware, se usará ese para escribir y 
	//cuado haga un unlock esa info se copiará a la tarjeta de video.
	if (SDL_LockTextureToSurface(g_texture, nullptr, &surface))
	{
		SDL_BlitSurface(imgSurface,nullptr , surface, nullptr);
		//Copy to GPU
		SDL_UnlockTexture(g_texture);
	}

	SDL_DestroySurface(imgSurface);

	SDL_FRect dstRect = { 0,0, g_imgScreen.getWidth(), g_imgScreen.getHeight() };
	SDL_RenderTexture(g_renderer, g_texture, nullptr, &dstRect);

	//Display in screen
	SDL_RenderPresent(g_renderer);

	return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
	/* SDL will clean up the window/renderer for us. */
}

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

	auto pixelShader = [&playerTexture](float u, float v ) -> Color 
	{
		Color color = playerTexture.getColor(u, v, TEXTURE_ADDRESS::MIRROR).toColor();
		return color;
		/*FloatColor color = { u, v, 0.0f, 1.0f };
		return color.toColor();*/
	};

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
	tri.v1 = { -5, 30,0, Color{255, 0, 0, 255}, 0, 0 };
	tri.v2 = { 100,80,0, Color{0, 255, 0, 255}, 1, 1 };
	tri.v3 = { 70,150,0, Color{0, 0, 255, 255}, 0, 1 };

	imgScreen.drawTriangle2D(tri, pixelShader);

	tri.v1 = { 200,200,0, Color{255, 0, 0, 255}, 0, 0 };
	tri.v2 = { 300,200,0, Color{0, 255, 0, 255}, 1, 0 };
	tri.v3 = { 200,300,0, Color{0, 255, 0, 255}, 0, 1 };

	imgScreen.drawTriangle2D(tri, pixelShader);

	tri.v1 = { 300,200,0, Color{0, 255, 255, 255}, 1, 0 };
	tri.v2 = { 200,300,0, Color{255, 0, 255, 255}, 0, 1 };
	tri.v3 = { 300,300,0, Color{255, 255, 0, 255}, 1, 1 };

	imgScreen.drawTriangle2D(tri, pixelShader);

	imgScreen.bresehamCircle(240, 400,120, Color{66,77,123,255});

	imgScreen.encode("Screen.bmp");

	return 0;


}