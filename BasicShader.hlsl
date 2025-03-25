Texture2D txColor : register(t0);

SamplerState samPoint   : register(s0);
SamplerState samLinear  : register(s1);
SamplerState samAniso   : register(s2);

struct VertexInput
{
    float3 position : POSITION;
    float3 color : COLOR0;
    float2 texCoord : TEXCOORD0;
};

struct PixelInput
{
    float4 position : SV_POSITION;
    float3 color : TEXCOORD0;
    float2 texCoord : TEXCOORD1;
};

cbuffer MatrixCollection : register(b0)
{
    float4x4 World;
    float4x4 View;
    float4x4 Projection;
}

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
    float4 color = txColor.Sample(samAniso, Input.texCoord);
    
    return color;

}