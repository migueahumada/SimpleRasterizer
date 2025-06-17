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
class Renderer
{
public:
  Renderer(const WPtr<GraphicsAPI>& pGraphicsAPI, 
                    const WPtr<Camera>& pCamera,
                    const WPtr<World>& pWorld);
  
  Renderer(const Renderer& r) = delete;
  Renderer(Renderer&& r) = delete;

  Renderer& operator=(const Renderer&) = delete;
  Renderer& operator=(Renderer&&) = delete;

  void CompileShaders();
  void InitInputLayout();
  void InitCamera(int width, int height);
  void InitWVP();
  void InitConstantBuffer();
  void InitRasterizerStates();
  void InitSampleFilters();
  void InitGBuffer(int width, int height);
  void SetDefaultTextures();

  void SetShadowPass();
  void SetGeometryPass();
  void SetSSAOPass();
  void SetLightingPass();

private:

  void RenderActor(const WPtr<Character>& character);
  void RenderShadows(const WPtr<Character>& character);
  void CreateDefaultSRV(UINT color, 
                        DefaultTextures::E defaultTextureType, DXGI_FORMAT format = DXGI_FORMAT_B8G8R8A8_UNORM);

private:
  
  WPtr<GraphicsAPI> m_pGraphicsAPI;
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

  UnorderedMap<RasterStates::E, ID3D11RasterizerState1*> m_RasterStates;
  UnorderedMap<SamplerStates::E, ID3D11SamplerState*> m_SamplerStates;

  UnorderedMap<DefaultTextures::E, ID3D11ShaderResourceView*> m_DefaultTextures; 

  Vector<Texture> m_GBuffer;
  Texture m_dsShadowMap;
  
};

