#pragma once
#include "Module.h"
#include <d3d11_2.h>
#include "HelperMacros.h"
#include "MathObjects.h"
#include "Texture.h"


class MKGraphicsAPI;
class GraphicsBuffer;
struct MatrixCollection;
class VertexShader;
class PixelShader;
class Texture;
class World;
class Camera;
class Character;

namespace MKEngine{
  
  namespace RasterStates {
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

  class MKRenderer : public Module<MKRenderer>
  {
  public:
    MKRenderer();
    virtual ~MKRenderer();

    virtual void OnStartUp() override{}

    virtual void OnShutdown() override{}

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

    inline Vector<Texture>& getGBuffer() {
      return m_GBuffer;
    }

    inline Texture& getShadowMap() {
      return m_dsShadowMap;
    }

  private:

    void RenderActor(const WPtr<Character>& character);
    void RenderShadows(const WPtr<Character>& character);
    void CreateDefaultSRV(UINT color,
                          DefaultTextures::E defaultTextureType, 
                          DXGI_FORMAT format = DXGI_FORMAT_B8G8R8A8_UNORM);


  protected:
    
    WPtr<Camera> m_pCamera;
    WPtr<World> m_pWorld;

    //InputLayouts
    ID3D11InputLayout* m_pInputLayout = nullptr;
    ID3D11InputLayout* m_pDefLightingInputLayout = nullptr;

    //World-View-Projection Camera Matrix
    MatrixCollection m_WVP;
    SPtr<GraphicsBuffer> m_pCB_WVP;

    //Shaders
    SPtr<VertexShader> m_pGBuffer_VS;
    SPtr<PixelShader> m_pGBuffer_PS;

    SPtr<VertexShader> m_pVS_SSFS;

    SPtr<PixelShader> m_pPS_DefLighting;
    SPtr<PixelShader> m_pPS_AO;

    SPtr<VertexShader> m_pVS_ShadowMap;
    SPtr<PixelShader> m_pPS_ShadowMap;
    
    //RasterStates
    UnorderedMap<RasterStates::E, ID3D11RasterizerState1*> m_RasterStates;
    
    //SamplerState
    UnorderedMap<SamplerStates::E, ID3D11SamplerState*> m_SamplerStates;
    
    //DefaultTexture
    UnorderedMap<DefaultTextures::E, ID3D11ShaderResourceView*> m_DefaultTextures;

    //Texture
    Vector<Texture> m_GBuffer;
    Texture m_dsShadowMap;



  };


}

