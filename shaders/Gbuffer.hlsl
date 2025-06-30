//Constants
#define M_PI 3.14159265358979323846f
#define M_2PI M_PI * 2.0f
#define GAMMA 2.2f // (1.8 a 2.4)

Texture2D txColor : register(t0);
Texture2D txNormal : register(t1);
Texture2D txRoughness : register(t2);
Texture2D txMetallic : register(t3);

SamplerState samPoint : register(s0);
SamplerState samLinear : register(s1);
SamplerState samAniso : register(s2);


struct VertexInput
{
    float3 position : POSITION;
    float3 color : COLOR0;
    float3 normal : NORMAL0;
    float3 tangent : TANGENT0;
    float2 texCoord : TEXCOORD0;
};

struct PixelInput
{
    float4 position : SV_POSITION;
    float3 posW : TEXCOORD0;
    float3 color : TEXCOORD1;
    float2 texCoord : TEXCOORD2;
    float3 normal : TEXCOORD3;
    float3 tangent : TEXCOORD4;
    float3 bitangent : TEXCOORD5;
};

struct ShadowPixel
{
    float4 position : SV_Position;
    float3 posW : TEXCOORD0;
};

struct GBuffer
{
    float4 position : SV_Target0;
    float4 normal   : SV_Target1;
    float4 color    : SV_Target2;
};

cbuffer MatrixCollection : register(b0)
{
    float4x4 World;
    float4x4 View;
    float4x4 Projection;
        
    float4x4 lightView;
    float4x4 lightProjection;
    
    float3 viewPos;
    //float time;
}

PixelInput gbuffer_vertex_main(VertexInput Input)
{
    PixelInput Output = (PixelInput) 0;
    
    Output.position = float4(Input.position, 1.0f);
    Output.position = mul(Output.position, World);
    Output.posW     = Output.position.xyz;
    
    Output.position = mul(Output.position, View);
    Output.position = mul(Output.position, Projection);
    
    Output.normal = normalize(mul(Input.normal, (float3x3) World).xyz);
    Output.tangent = normalize(mul(Input.tangent, (float3x3) World).xyz);
    Output.bitangent = -cross(Output.normal, Output.tangent);
    
    Output.color = Input.color;
    Output.texCoord = Input.texCoord;
    
    return Output;
}

GBuffer gbuffer_pixel_main(PixelInput Input)
{
    GBuffer Output = (GBuffer)0;
    
    float4 diffColor = txColor.Sample(samLinear, Input.texCoord);
    float4 normal = txNormal.Sample(samLinear, Input.texCoord) * 2.0 - 1.0f;
    float roughness = txRoughness.Sample(samLinear, Input.texCoord).r;
    float metallic = txMetallic.Sample(samLinear, Input.texCoord).r;
    
    float3x3 TBN = float3x3(Input.tangent, Input.bitangent, Input.normal);
    normal.xyz = normalize(mul(normal.xyz, TBN));
    
    Output.position = float4(Input.posW, metallic);
    Output.normal = float4(normal.xyz * 0.5f + 0.5f, roughness);
    Output.color = diffColor;
    
    return Output;

}

ShadowPixel shadow_map_vertex_main(VertexInput Input)
{
    ShadowPixel Output = (ShadowPixel)0;
    float4 pos = float4(Input.position, 1.0f);
    Output.posW = pos.xyz;
    pos = mul(pos, World);
    pos = mul(pos, lightView);
    pos = mul(pos, lightProjection);
    Output.position = pos;
    
    return Output;
}

float4 shadow_map_pixel_main(ShadowPixel Input) : SV_Target
{
    return float4(0.0f, 0.0f, 0.0f, 1.0f);

}