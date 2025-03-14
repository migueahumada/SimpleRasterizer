struct VertexInput
{
    float3 position : POSITION;
    float3 color : COLOR0;
};

struct PixelInput
{
    float4 position : SV_POSITION;
    float3 color : TEXCOORD0;
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
    
    Output.position = float4(Input.position, 1);
    Output.position = mul(Output.position, World);
    Output.position = mul(Output.position, View);
    Output.position = mul(Output.position, Projection);
    
    Output.color = Input.color;
    
    return Output;
}

float4 pixel_main(PixelInput Input) : SV_Target
{
    return float4(Input.color,1.0f);

}