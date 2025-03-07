struct VertexInput
{
    float3 position : POSITION;
    float3 color : COLOR0;
};

struct PixelInput
{
    float4 position : SV_Position;
    float3 color : TEXCOORD0;
};

PixelInput vertex_main(VertexInput Input) 
{
    PixelInput Output;
    Output.position = float4(Input.position, 1);
    Output.color = Input.color;
    
    return Output;
}

float4 pixel_main(PixelInput Input) : SV_Target
{
    return float4(Input.color,1.0f);

}