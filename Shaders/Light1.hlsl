Texture2D txColor : register(t0);

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
};

cbuffer MatrixCollection : register(b0)
{
    float4x4 World;
    float4x4 View;
    float4x4 Projection;
    
    float3 ViewDir;
    //float time;
}

PixelInput vertex_main(VertexInput Input)
{
    PixelInput Output = (PixelInput) 0;
    
    Output.position = float4(Input.position, 1.0f);
    Output.position = mul(Output.position, World);
    
    Output.posW = Output.position.xyz;
    
    Output.position = mul(Output.position, View);
    Output.position = mul(Output.position, Projection);
    
    Output.normal = mul(Input.normal, World).xyz;
    
    Output.color = Input.color;
    Output.texCoord = Input.texCoord;
    
    return Output;
}

float4 pixel_main(PixelInput Input) : SV_Target
{
    float3 lightPos = float3(-20.0f, 2.0f, 2.0f);
    
    
    //Luz de direccional
    //float3 lightDir = normalize(lightPos);
    
    //Luz de punto
    //float3 lightDir = normalize(lightPos - Input.posW);
    
    float3 lightDir = normalize(lightPos - Input.posW);
    float3 spotDir = normalize(float3(1.0f, 8.0f, 0.0F));
    
    //Spotlight
    //float spotAngle = (50.0f * 20*sin(time)) * (3.14159265383f / 180.0f);
    float spotAngle = 90.0f * (3.14159265383f / 180.0f);
    spotAngle = cos(spotAngle);
    
    float DiffuseIncidence = dot(lightDir, Input.normal);
    float IncidenceToLight = dot(-spotDir, lightDir);
    
    //Specular
    //Compute light reflection
    float3 ReflectVector = reflect(lightDir, Input.normal);
    float3 myViewDir = -ViewDir;
    
    float3 H = normalize(lightDir + myViewDir);
    
    float SpecularIncidence = max(0.0f, dot(myViewDir, H));
    SpecularIncidence = pow(SpecularIncidence, 15);
    
    //F�rmula de la luz
    // R = 2(n * L) N - L
    
    if (IncidenceToLight < spotAngle)
    {
        DiffuseIncidence = 0.0f;
    }
    
    float4 color = txColor.Sample(samLinear, Input.texCoord);
    color.rgb *= DiffuseIncidence;
    
    // Lo = kD + kS + kA
    
    return color + SpecularIncidence;

}

float4 pixel_basic(PixelInput Input) : SV_Target
{
    float4 color = txColor.Sample(samLinear,Input.texCoord);
    return color;
}