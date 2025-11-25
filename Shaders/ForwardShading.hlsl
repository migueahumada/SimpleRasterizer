Texture2D texColor : register(t0);
Texture2D texNormals : register(t1);
Texture2D texMetallic : register(t2);
Texture2D texRoughness : register(t2);

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
    float diffuse : TEXCOORD6;
};

cbuffer MatrixCollection : register (b0)
{
    float4x4 World;
    float4x4 View;
    float4x4 Projection;
};

PixelInput vertex_main(VertexInput Input) 
{
    PixelInput Output = (PixelInput)0;
    
    Output.position = float4(Input.position, 1.0f);
    Output.position = mul(Output.position, World);
    Output.position = mul(Output.position, View);
    Output.position = mul(Output.position, Projection);
    
    Output.color = Input.color;
    Output.texCoord = Input.texCoord;
    
    return Output;
}

float4 pixel_main(PixelInput Input) : SV_Target
{
    float4 color = texColor.Sample(samLinear, Input.texCoord);
    
    
    
    return color;
}