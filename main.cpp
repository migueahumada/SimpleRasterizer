#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <iostream>
#include "MathObjects.h"

#define WIDTH 1280
#define HEIGHT 720


#include "GraphicsAPI.h"

struct MODEL_VERTEX 
{
	Vector3 position;
	Vector3 color;
};

struct MatrixCollection
{
	Matrix4 world;
	Matrix4 view;
	Matrix4 projection;
};

SDL_Window* g_pWindow = nullptr;

UPtr <GraphicsAPI> g_pGraphicsAPI;
UPtr<VertexShader> g_pVertexShader;
UPtr<PixelShader> g_pPixelShader;

ID3D11InputLayout* g_pInputLayout = nullptr;
UPtr<GraphicsBuffer> g_pVertexBuffer;
UPtr<GraphicsBuffer> g_pIndexBuffer;
ID3D11RasterizerState1* g_pRS_Default = nullptr;
ID3D11RasterizerState1* g_pRS_Wireframe = nullptr;
ID3D11RasterizerState1* g_pRS_No_Cull = nullptr;

UPtr<GraphicsBuffer> g_pCB_WVP;
MatrixCollection g_WVP;

Camera g_Camera;

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{

	if (!SDL_Init(SDL_INIT_VIDEO)) {
		SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}

	g_pWindow = SDL_CreateWindow("Soft Rasterizer",
								WIDTH, 
								HEIGHT,
								SDL_WINDOW_RESIZABLE);
	
	auto pWndHandle = SDL_GetPointerProperty(	SDL_GetWindowProperties(g_pWindow),
											SDL_PROP_WINDOW_WIN32_HWND_POINTER, 
											nullptr);
	if (pWndHandle)
	{
		g_pGraphicsAPI = make_unique<GraphicsAPI>(pWndHandle);
		if(!g_pGraphicsAPI)
		{
			return SDL_APP_FAILURE;
		}
	}

	g_pVertexShader = g_pGraphicsAPI->CreateVertexShader(L"BasicShader.hlsl", "vertex_main");
	if (!g_pVertexShader)
	{
		return SDL_APP_FAILURE;
	}

	g_pPixelShader = g_pGraphicsAPI->CreatePixelShader(L"BasicShader.hlsl", "pixel_main");
	if(!g_pPixelShader)
	{
		return SDL_APP_FAILURE;
	}

	Vector<D3D11_INPUT_ELEMENT_DESC> inputElementDesc = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"COLOR"	, 0, DXGI_FORMAT_R32G32B32_FLOAT,  0, 12,D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	g_pInputLayout = g_pGraphicsAPI->CreateInputLayout(inputElementDesc, g_pVertexShader);
	if (!g_pInputLayout)
	{
		return SDL_APP_FAILURE;
	}

	
	//X -> R ed 
	//Y -> G reen
	//Z -> B lue
	MODEL_VERTEX vertices[] = 
	{
		//Esto ya sería posición de mundo.
		{ Vector3(-1.0f, 1.0f, -1.0f),	Vector3(0, 1, 0)},
		{ Vector3( 1.0f, 1.0f, -1.0f),	Vector3(0, 1, 0)},
		{ Vector3( 1.0f, 1.0f,  1.0f),	Vector3(0, 1, 0)},
		{ Vector3(-1.0f, 1.0f,  1.0f),  Vector3(0, 1, 0)},

		{ Vector3(-1.0f, -1.0f, -1.0f),	Vector3(0, 1, 0)},
		{ Vector3( 1.0f, -1.0f, -1.0f),	Vector3(0, 1, 0)},
		{ Vector3( 1.0f, -1.0f,  1.0f),	Vector3(0, 1, 0)},
		{ Vector3(-1.0f, -1.0f,  1.0f), Vector3(0, 1, 0)},

		{ Vector3(-1.0f, -1.0f,  1.0f),	Vector3(1, 0, 0)},
		{ Vector3(-1.0f, -1.0f, -1.0f),	Vector3(1, 0, 0)},
		{ Vector3(-1.0f,  1.0f, -1.0f),	Vector3(1, 0, 0)},
		{ Vector3(-1.0f,  1.0f,  1.0f), Vector3(1, 0, 0)},

		{ Vector3( 1.0f, -1.0f,  1.0f),	Vector3(1, 0, 0)},
		{ Vector3( 1.0f, -1.0f, -1.0f),	Vector3(1, 0, 0)},
		{ Vector3( 1.0f,  1.0f, -1.0f),	Vector3(1, 0, 0)},
		{ Vector3( 1.0f,  1.0f,  1.0f), Vector3(1, 0, 0)},

		{ Vector3(-1.0f, -1.0f, -1.0f),	Vector3(0, 0, 1)},
		{ Vector3(1.0f, -1.0f, -1.0f),	Vector3(0, 0, 1)},
		{ Vector3(1.0f,  1.0f, -1.0f),	Vector3(0, 0, 1)},
		{ Vector3(-1.0f,  1.0f, -1.0f), Vector3(0, 0, 1)},

		{ Vector3(-1.0f, -1.0f,  1.0f),	Vector3(0, 0, 1)},
		{ Vector3( 1.0f, -1.0f,  1.0f),	Vector3(0, 0, 1)},
		{ Vector3( 1.0f,  1.0f,  1.0f),	Vector3(0, 0, 1)},
		{ Vector3(-1.0f,  1.0f,  1.0f), Vector3(0, 0, 1)},

		
	};

	Vector<char> vertex_data;
	vertex_data.resize(sizeof(vertices));
	memcpy(vertex_data.data(), vertices, sizeof(vertices));

	g_pVertexBuffer = g_pGraphicsAPI->CreateVertexBuffer(vertex_data);
	if (!g_pVertexBuffer)
	{
		return SDL_APP_FAILURE;
	}

	unsigned short indices[] = 
	{
		3,1,0,		2,1,3,
		6,4,5,		7,4,6,
		11,9,8,		10,9,11,
		14,12,13,	15,12,14,
		19,17,16,	18,17,19,
		22,20,21,	23,20,22
	};

	Vector<char> index_data;
	index_data.resize(sizeof(indices));
	memcpy(index_data.data(), indices, sizeof(indices));

	g_pIndexBuffer = g_pGraphicsAPI->CreateIndexBuffer(index_data);
	if (!g_pIndexBuffer)
	{
		return SDL_APP_FAILURE;
	}

	g_Camera.SetLookAt(Vector3(5, -5, -5), Vector3(0, 0, 0), Vector3(0, 1, 0));
	g_Camera.SetPerspective(3.141592653f/4.0f,WIDTH,HEIGHT,0.1f,100.0f);

	g_WVP.world.Identity();
	g_WVP.view = g_Camera.getViewMatrix();
	g_WVP.projection = g_Camera.getProjectionMatrix();
	
	g_WVP.world.Transpose();
	g_WVP.view.Transpose();
	g_WVP.projection.Transpose();

	Vector<char> matrix_data;
	matrix_data.resize(sizeof(g_WVP));
	memcpy(matrix_data.data(), &g_WVP, sizeof(g_WVP));
	g_pCB_WVP = g_pGraphicsAPI->CreateConstantBuffer(matrix_data);

	CD3D11_RASTERIZER_DESC1 descRD(D3D11_DEFAULT);
	g_pGraphicsAPI->m_pDevice->CreateRasterizerState1(&descRD, &g_pRS_Default);

	descRD.FillMode = D3D11_FILL_WIREFRAME;
	g_pGraphicsAPI->m_pDevice->CreateRasterizerState1(&descRD, &g_pRS_Wireframe);

	descRD.CullMode = D3D11_CULL_NONE;
	g_pGraphicsAPI->m_pDevice->CreateRasterizerState1(&descRD, &g_pRS_No_Cull);

	/*Matrix4 posMatrix;
	posMatrix.Identity();
	posMatrix.Translate(Vector3(5.0f,6.0f,7.0f));

	Vector3 tempPos(0.0f);
	Vector3 res = posMatrix.TansformPosition(tempPos);*/

	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event * event)
{
	if (event->type == SDL_EVENT_QUIT) {
		return SDL_APP_SUCCESS;
	}
	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate)
{
	D3D11_VIEWPORT vp;
	vp.Width = WIDTH;
	vp.Height = HEIGHT;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;

	//Se setea lo que se mostrará en el Output Merge State
	g_pGraphicsAPI->m_pDeviceContext->OMSetRenderTargets(1, 
														&g_pGraphicsAPI->m_pBackBufferRTV, 
														g_pGraphicsAPI->m_pBackBufferDSV);
	//Limpiamos el render target del backbuffer con un color
	float clearColor[4] = { 0.5f,0.2f,0.5f,0.5f };
	g_pGraphicsAPI->m_pDeviceContext->ClearRenderTargetView(g_pGraphicsAPI->m_pBackBufferRTV,
															clearColor);
	
	//Limpiamos el depth stencil del backbuffer
	g_pGraphicsAPI->m_pDeviceContext->ClearDepthStencilView(g_pGraphicsAPI->m_pBackBufferDSV,
															D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 
															1.0f, 
															0);
	//Seteamos el vertex shader y el pixel shader
	g_pGraphicsAPI->m_pDeviceContext->VSSetShader(g_pVertexShader->m_pVertexShader, nullptr, 0);
	g_pGraphicsAPI->m_pDeviceContext->PSSetShader(g_pPixelShader->m_pPixelShader, nullptr, 0);

	//Seteamos lo que necesitamos para el input assembler
	//Seteamos el arreglo del input assembler y los valores que mandaremos al GPU
	g_pGraphicsAPI->m_pDeviceContext->IASetInputLayout(g_pInputLayout);
	//Le decimos al GPU que dibuje una lista de triángulos
	g_pGraphicsAPI->m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	//Le damos al IA un vertex Buffers para que dibuje los vértices
	UINT stride = sizeof(MODEL_VERTEX);
	UINT offset = 0;
	g_pGraphicsAPI->m_pDeviceContext->IASetVertexBuffers(0, 
														1, 
														&g_pVertexBuffer->m_pBuffer, 
														&stride, 
														&offset);
	
	//Le damos al IA un index buffer que dibujará vértices donde se necesiten para
	//Optimizar
	g_pGraphicsAPI->m_pDeviceContext->IASetIndexBuffer(
		g_pIndexBuffer->m_pBuffer, 
		DXGI_FORMAT_R16_UINT,
		0);

	static float rotationAngle = 0.0f;
	rotationAngle += 0.001f;

	Matrix4 translation;
	translation.Identity();
	translation.Translate(Vector3(2.5,0,0));

	Matrix4 translation2;
	translation2.Identity();
	translation2.Translate(Vector3(-2.5, 0, 0));

	Matrix4 translation3;
	translation3.Identity();
	translation3.Translate(Vector3(-2.5, 0, -5.5));

	Matrix4 rotation;
	rotation.RotateY(rotationAngle);

	g_WVP.world =  rotation * translation;

	//g_WVP.world.RotateY(rotationAngle);

	//g_Camera.SetLookAt(Vector3(5,sinf(rotationAngle)* -5, -5),Vector3(0,0,0),Vector3(0,1,0));
	//g_WVP.view = g_Camera.getViewMatrix();
	
	g_Camera.SetPerspective(3.141592653f / 4.0f, WIDTH, HEIGHT, 0.1f, 100.0f);
	g_WVP.projection = g_Camera.getProjectionMatrix();


	g_WVP.world.Transpose();
	//g_WVP.view.Transpose();
	g_WVP.projection.Transpose();

	Vector<char> matrix_data;
	matrix_data.resize(sizeof(g_WVP));
	memcpy(matrix_data.data(), &g_WVP, sizeof(g_WVP));
	
	g_pGraphicsAPI->m_pDeviceContext->VSSetConstantBuffers(0, 1, &g_pCB_WVP->m_pBuffer);

	g_pGraphicsAPI->m_pDeviceContext->RSSetState(g_pRS_Default);
	g_pGraphicsAPI->writeToBuffer(g_pCB_WVP,matrix_data);
	g_pGraphicsAPI->m_pDeviceContext->DrawIndexed(36,0,0); //Los dibujamos de manera indexada

	rotation.RotateY(-rotationAngle);
	g_WVP.world = rotation * translation2;
	g_WVP.world.Transpose();
	memcpy(matrix_data.data(), &g_WVP, sizeof(g_WVP));
	g_pGraphicsAPI->m_pDeviceContext->RSSetState(g_pRS_Wireframe);
	g_pGraphicsAPI->writeToBuffer(g_pCB_WVP, matrix_data);
	g_pGraphicsAPI->m_pDeviceContext->DrawIndexed(36, 0, 0);

	rotation.RotateY(-rotationAngle);
	g_WVP.world = rotation * translation3;
	g_WVP.world.Transpose();
	memcpy(matrix_data.data(), &g_WVP, sizeof(g_WVP));
	g_pGraphicsAPI->m_pDeviceContext->RSSetState(g_pRS_No_Cull);
	g_pGraphicsAPI->writeToBuffer(g_pCB_WVP, matrix_data);
	g_pGraphicsAPI->m_pDeviceContext->DrawIndexed(36, 0, 0);

	//Presenatmos
	g_pGraphicsAPI->m_pSwapChain->Present(0,0);

	return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void* appstate, SDL_AppResult result)
{

	if (g_pWindow)
	{
		SDL_DestroyWindow(g_pWindow);
		g_pWindow = nullptr;
	}
	SDL_Quit();
}

