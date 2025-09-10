/*
* Simple GBuffer implementation with this layout
* Pos | x | y | z | M |
* Col | x | y | z | R |
* Nm  | R | G | B | A |
*/

#pragma once
#include <d3d11_2.h>
#include "HelperMacros.h"
#include "MathObjects.h"
#include "Texture.h"
#include "Module.h"

#define NUM_LIGHTS 64
struct __declspec(align(16)) LightData
{
  Vector3 lightPosition;
  float p1;
};

struct __declspec(align(16)) LightsConstantBuffer
{


};

struct __declspec(align(16)) MatrixCollection
{
  Matrix4 world;
  Matrix4 view;
  Matrix4 projection;

  Matrix4 ligthView;
  Matrix4 lightProjection;

  Vector3 viewDir;
  float pl1;
  Vector3 lightPosition;
  float pl2;
  Vector3 placeholder;
  float pl3;
  Vector3 placeholder2;
  float pl4;

  Vector3 rayVector;
  float pl5;

  //LightData lightArray [NUM_LIGHTS];
  //float time;
};




namespace ShadingState{
  enum E {
    FORWARD,
    DEFERRED
  };
}

class GraphicsAPI;
class GraphicsBuffer;
struct MatrixCollection;
class VertexShader;
class PixelShader;
class Texture;
class World;
class Camera;
class Character;

namespace RasterStates{
  enum E {
    DEFAULT,
    WIREFRAME,
    NO_CULLING
  };
};

namespace SamplerStates {
  enum E {
    POINT,
    LINEAR,
    ANISOTROPIC
  };
};

namespace DefaultTextures
{
  enum E {
    WHITE,
    BLACK,
    NORMAL
  };
};

//TODO: Destruir los raster states y sampler states
class Renderer : public Module<Renderer>
{

public:
  Renderer(const WPtr<Camera>& pCamera,
    const WPtr<World>& pWorld);
  virtual ~Renderer() = default;

  void CompileShaders();
  void InitInputLayout();
  void InitCamera(int width, int height);
  void InitWVP();
  void InitConstantBuffer();
  void InitRasterizerStates();
  void InitSampleFilters();
  void InitGBuffer(int width, int height);
  void SetDefaultTextures();
  
  
  void SetPasses();
  
  void SetForwardPass();

  void SetShadowPass();
  void SetGeometryPass();
  void SetSSAOPass();
  void SetLightingPass();

  //Forward+
  //iF PARA forward O DEFERRED.
  //Scene Graph para decir qué es lo que tengo que dibujar.
  //Def -> Opacos y después alpha testing. Alpha testing en el shader de las sombras
  
  //Sombras
  //Primero alpha tsitng de opacos->
  //Todo de transparencia se tiene que hacer pase depsués de los opacos.

  inline Vector<Texture>& getGBuffer() {
    return m_GBuffer;
  }

  inline Texture& getShadowMap(){
    return m_dsShadowMap;
  }

  MatrixCollection& GetWVP() {
    return m_WVP;
  }

private:

  void OnStartUp() override;

  void OnShutdown() override;

  void RenderActor(const WPtr<Character>& character, bool bDrawWithTextures);
  void RenderShadows(const WPtr<Character>& character);
  void CreateDefaultSRV(UINT color, 
                        DefaultTextures::E defaultTextureType, DXGI_FORMAT format = DXGI_FORMAT_B8G8R8A8_UNORM);

 

private:
  
  WPtr<Camera> m_pCamera;
  WPtr<World> m_pWorld;

  ID3D11InputLayout* m_pInputLayout = nullptr;
  ID3D11InputLayout* m_pDefLightingInputLayout = nullptr;

  MatrixCollection m_WVP;
  SPtr<GraphicsBuffer> m_pCB_WVP;
  
  SPtr<VertexShader> m_pGBuffer_VS;
  SPtr<PixelShader> m_pGBuffer_PS;

  SPtr<VertexShader> m_pVS_SSFS;
  
  SPtr<PixelShader> m_pPS_DefLighting;
  SPtr<PixelShader> m_pPS_AO;

  SPtr<VertexShader> m_pVS_ShadowMap;
  SPtr<PixelShader> m_pPS_ShadowMap;

  SPtr<VertexShader> m_pVS_Forward;
  SPtr<PixelShader> m_pPS_Forward;

  UnorderedMap<RasterStates::E, ID3D11RasterizerState1*> m_RasterStates;
  UnorderedMap<SamplerStates::E, ID3D11SamplerState*> m_SamplerStates;

  UnorderedMap<DefaultTextures::E, ID3D11ShaderResourceView*> m_DefaultTextures; 

  Vector<Texture> m_GBuffer;
  Texture m_dsShadowMap;
  //TODOOOO
  Texture m_forwardRTV;

  ShadingState::E m_currentShadingState{ShadingState::DEFERRED};

  


};

Renderer& g_renderer();
