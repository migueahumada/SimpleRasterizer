Texture2D gbuffer_Position : register(t0);
Texture2D gbuffer_Normal : register(t1);
Texture2D gbuffer_Color : register(t2);

SamplerState samPoint : register(s0);
SamplerState samLinear : register(s1);
SamplerState samAniso : register(s2);

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD1;
};

PixelInput vertex_main(uint vertexID : SV_VertexID)
{
    PixelInput Output = (PixelInput) 0;
    
    float2 positions[3] = 
    {
        float2(-1.0f,-1.0f),
        float2(3.0f, -1.0f),
        float2(-1.0f, 3.0f)
    };
    
    Output.position = float4(positions[vertexID], 0.0f, 1.0f);
    Output.texCoord = (positions[vertexID] + float2(1.0f, 1.0f)) * 0.5f;
    Output.texCoord.y = 1.0f - Output.texCoord.y;
    
    return Output;
}

float4 pixel_main(PixelInput Input) : SV_Target
{
    float4 position = gbuffer_Position.Sample(samPoint, Input.texCoord);
    float4 normal = gbuffer_Normal.Sample(samPoint, Input.texCoord);
    float4 color = gbuffer_Color.Sample(samPoint, Input.texCoord);
    
    return float4(color.rgb, 1.0f);
}