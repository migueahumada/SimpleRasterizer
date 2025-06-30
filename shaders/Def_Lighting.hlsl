//Constants
#define M_PI 3.14159265358979323846f
#define M_2PI M_PI * 2.0f
#define GAMMA 2.2f // (1.8 a 2.4)
#define WHITE float4(1.0f, 1.0f, 1.0f, 1.0f);
#define BLACK float4(0.0f, 0.0f, 0.0f, 1.0f);

Texture2D gbuffer_Position : register(t0);
Texture2D gbuffer_Normal : register(t1);
Texture2D gbuffer_Color : register(t2);
Texture2D gbuffer_AO : register(t3);
Texture2D gbuffer_ShadowMap : register(t4);

SamplerState samPoint : register(s0);
SamplerState samLinear : register(s1);
SamplerState samAniso : register(s2);

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD1;
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

//Normal Distribution functions
float ndf_BlinnPhong(float NdotH, float alpha)
{
    float a2 = alpha * alpha;
    float n = (2.0f / a2) - 1.0f;
    return (n + 1.0f) / M_2PI * pow(NdotH, n);

}

float ndf_Beckmann(float NdotH, float alpha)
{
    float a2 = alpha * alpha;
    float cosThetaHSqr = NdotH * NdotH;
    
    float tanThetaHSqr = (1.0f - cosThetaHSqr) / cosThetaHSqr;
    float e = exp(-tanThetaHSqr / a2);
    
    return e / M_PI * a2 * cosThetaHSqr * NdotH;

}

float ndf_OrenNayar(float NdotL, float NdotV, float alpha)
{
    
    float alpha2 = alpha * alpha;
    float NdotL2 = NdotL * NdotL;
    float NdotV2 = NdotV * NdotV;
    
    float a = max(NdotL, NdotV);
    float b = min(NdotL, NdotV);
    
    return (1.0f / M_PI) * (1.0f - (a * b) / (alpha2 + 0.33f)) * (1.0f + (b / a));

}

float ndf_GGX(float NdotH, float alpha)
{
    float a2 = alpha * alpha;
    float cosThetaHSqr = NdotH * NdotH;
    
    float tanThetaHSqr = (1.0f - cosThetaHSqr) / cosThetaHSqr;
    float e = exp(-tanThetaHSqr / a2);
    
    return e / M_PI * a2 * cosThetaHSqr * NdotH;
}

float3 Fresnel_Schlick(float3 F0, float costTheta, float3 F90)
{
    //Fresnel Schlick
    //FO cual es valor de reflexión en el ángulo 0 -> víéndolo en el mismo ángulo
    //F90 es el valor de reflexión en el ángulo 90 -> víéndolo perpendicular
    //costhta es el viewdothalf, entre vista y la mitad entre la luz y la vista
    return F0 + (F90 - F0) * pow(1.0f - costTheta, 5.0f);
}

float3 Fresnel_Schlick(float3 F0, float costTheta)
{
    //Fresnel Schlick
    return F0 + (1.0f - F0) * pow(1.0f - costTheta, 5.0f);
}

//Distrbucion geometrica de Smith-Schlick
float GeometrySchlickGGX(float NdotX, float alpha)
{
    float k = alpha / 2.0f;
    return NdotX / (NdotX * (1.0f - k) + k);

}

float GeometrySmith(float NdotL, float NdotV, float alpha)
{
    return GeometrySchlickGGX(NdotL, alpha) *
           GeometrySchlickGGX(NdotV, alpha);

}

float3 BRDF_Blinn_Phong(float3 normal,
                        float3 lightDir,
                        float3 viewDir,
                        float3 reflectDir,
                        float3 diffuseColor,
                        float3 specularColor)
{
    //Lambert Diffuse
    float NdotL = max(dot(normal, lightDir), 0.0f);
    float3 diffuse = diffuseColor * NdotL;
    
    //Blinn-Phong Specular
    float NdotH = max(dot(normal, reflectDir), 0.0f);
    float NdotV = max(dot(normal, viewDir), 0.0f);
    float HdotV = max(dot(reflectDir, viewDir), 0.0f);
    
    float specular = pow(HdotV, 32.0f) * NdotH * NdotV;
    
    return diffuse + specular;
}

float3 BRDF_Cook_Torrence(float3 normal,
                          float3 lightDir,
                          float3 viewDir,
                          float3 reflectDir,
                          float3 diffuseColor,
                          float3 specularColor,
                          float roughness)
{
    //Lambert Diffuse
    float NdotL = max(dot(normal, lightDir), 0.0f);
    float3 diffuse = diffuseColor * NdotL;
    
    //Cook-Torrence Specular
    float NdotV = max(dot(normal, viewDir), 0.0f);
    float NdotH = max(dot(normal, reflectDir), 0.0f);
    
    //Índice de rugosidad
    /*
        specular = D * F * G / (4 * NdotL * NdotV)
    */
    
    float D = ndf_GGX(NdotH, roughness);
    float3 F = Fresnel_Schlick(specularColor, NdotV);
    float G = GeometrySmith(NdotL, NdotV, roughness);
    
    //return float4(G.xxx, 1.0f);
    float3 specular = ((D * F * G) / (4.0f * NdotL * NdotV));
    
    return diffuse + specular;
}

float4 GetPosition(float2 uv)
{
    return gbuffer_Position.Sample(samLinear, uv);
}

float4 GetNormal(float2 uv)
{
    float4 normal = gbuffer_Normal.Sample(samLinear, uv);
    normal.xyz = normalize(normal.xyz * 2.0f - 1.0f);
    return normal;

}

float2 GetRandom(float2 uv)
{
    float noiseX = frac(sin(dot(uv.xy, float2(15.8989, 76.132) * 1.0f)) * 43758.5453);
    float noiseY = frac(sin(dot(uv.xy, float2(11.9898, 62.233) * 2.0f)) * 34748.5453);
    float noiseZ = frac(sin(dot(uv.xy, float2(13.3232, 63.123) * 3.0f)) * 59998.5736);
    
    return normalize(float3(noiseX, noiseY, noiseZ));

}

float4 pixel_main(PixelInput Input) : SV_Target
{
    float4 position = GetPosition(Input.texCoord);
    float4 normal = GetNormal(Input.texCoord);
    float4 color = gbuffer_Color.Sample(samLinear, Input.texCoord);
    float4 ao = gbuffer_AO.Sample(samLinear, Input.texCoord);
    
    float4 posInLightVP = mul(float4(position.xyz,1), lightView);
    
    posInLightVP = mul(posInLightVP, lightProjection);
    posInLightVP /= posInLightVP.w;
    posInLightVP.xy = posInLightVP.xy * 0.5 + 0.5;
    
    posInLightVP.y = 1.0f - posInLightVP.y;
    
    float4 shadowMap = gbuffer_ShadowMap.Sample(samLinear, posInLightVP.xy);
    
    float shadowDepth = shadowMap.x;
    float lightDepth = posInLightVP.z - 0.005; //bias
    float shadowFactor;
    
    if (lightDepth > shadowDepth)
        shadowFactor = 0.0f;
    else
        shadowFactor = 1.0f;
    
    if (posInLightVP.x < 0.01f || posInLightVP.x > 0.99f || 
        posInLightVP.y < 0.01f || posInLightVP.y > 0.99f)
    {
        shadowFactor = 1.0f;
    }
    
    clip(color.w < 1.0f ? -1 : 1);
    
    float3 lightPos = float3(20.0f, 20.0f, 5.0f);
    
    float3 lightDir = normalize(lightPos - position.xyz);
    
    //el 0.04 es un promedio de los objetos dieléctricos
    float3 specularColor = lerp(0.04f, color.rgb, position.w);
    
    //return float4(specularColor.xyz, 1);
    
    float3 colorFinal = BRDF_Cook_Torrence(normal.xyz,
                                           lightDir,
                                           normalize(position.xyz - viewPos),
                                           normalize(reflect(lightDir, normal.xyz)),
                                           color.rgb,
                                           specularColor,
                                           normal.w);
    //return float4(specularColor, 1);
    // Lo = kD + kS + kA
    //return normal.wwww;
    return float4(pow(colorFinal * ao.xxx * shadowFactor, 1.0f / GAMMA),
    1.0f);
    //return float4(ao.xxx, 1.0f);
    //return float4(colorFinal.rgb, 1.0f);
}



float DoAmbientOcclussion(in float2 tcoord, in float2 uv, in float3 p, in float3 ncoord)
{
    float AOScale = 1.0f;
    float AOIntensity = 3.0f;
    
    float3 diff = GetPosition(tcoord + uv).xyz - p;
    float3 v = normalize(diff);
    float d = length(diff) * AOScale;
    return max(0.0f, dot(ncoord, v)) * (1.0f / (1.0f + d)) * AOIntensity;
}

float4 ao_main(PixelInput Input) : SV_Target
{
    float AOScale = 1.0f;
    float AOIntensity = 2.0f; //AO
    
    float2 vec[4] = 
    {
        float2( 1.0f,  0.0f),
        float2(-1.0f,  0.0f),
        float2( 0.0f,  1.0f),
        float2( 0.0f, -1.0f)
    };
    
    float4 pos = GetPosition(Input.texCoord);
    float3 normal = GetNormal(Input.texCoord).xyz;
    float2 random = GetRandom(Input.texCoord);
    
    float radius = AOScale / pos.z;
    float ao = 0.0f;
    
    int iterations = 4;
    for (int i = 0; i < iterations; ++i)
    {
        float2 coord1 = reflect(vec[i], random) * radius;
        float2 coord2 = float2(coord1.x * 0.707f - coord1.y * 0.707f,
                               coord1.x * 0.707f + coord1.y * 0.707f);
        
        ao += DoAmbientOcclussion(Input.texCoord, coord1 * 0.25, pos.xyz, normal);
        ao += DoAmbientOcclussion(Input.texCoord, coord2 * 0.50, pos.xyz, normal);
        ao += DoAmbientOcclussion(Input.texCoord, coord1 * 0.75, pos.xyz, normal);
        ao += DoAmbientOcclussion(Input.texCoord, coord2 * 1.00 , pos.xyz, normal);
    }
    
    ao /= iterations * 4.0f;
    return 1.0 - ao;

}