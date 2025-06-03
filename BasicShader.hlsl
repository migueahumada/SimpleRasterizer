//Constants
#define M_PI 3.14159265358979323846f
#define M_2PI M_PI * 2.0f
#define GAMMA 2.2f // (1.8 a 2.4)

Texture2D txColor : register(t0);
Texture2D txNormal : register(t1);
Texture2D txRoughness : register(t2);
Texture2D txMetallic : register(t3);

SamplerState samPoint   : register(s0);
SamplerState samLinear  : register(s1);
SamplerState samAniso   : register(s2);

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

cbuffer MatrixCollection : register(b0)
{
    float4x4 World;
    float4x4 View;
    float4x4 Projection;
    float3 viewPos;
    //float time;
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
    
    float tanThetaHSqr  = (1.0f - cosThetaHSqr) / cosThetaHSqr;
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

PixelInput vertex_main(VertexInput Input) 
{
    PixelInput Output = (PixelInput)0;
    
    //float3 lightPos = float3(10.0f, 0.0f, 0.0f);
    //float3 lightDir = -normalize(lightPos);
    
    Output.position  = float4(Input.position, 1.0f);
    Output.position  = mul(Output.position, World);
    Output.posW      = Output.position.xyz;
    Output.position  = mul(Output.position, View);
    Output.position  = mul(Output.position, Projection);
    
    Output.normal    = normalize(mul(Input.normal, (float3x3) World).xyz);
    Output.tangent   = normalize(mul(Input.tangent, (float3x3) World).xyz);
    Output.bitangent = -cross(Output.normal, Output.tangent);
    
    Output.color     = Input.color;
    Output.texCoord  = Input.texCoord;
    
    return Output;
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
    
    float  D = ndf_GGX(NdotH, roughness);
    float3 F = Fresnel_Schlick(specularColor, NdotV);
    float  G = GeometrySmith(NdotL, NdotV, roughness);
    
    //return float4(G.xxx, 1.0f);
    float3 specular = ((D * F * G) / (4.0f * NdotL * NdotV));
    
    return diffuse + specular;
}

float4 pixel_main(PixelInput Input) : SV_Target
{
    float4 diffColor = pow(txColor.Sample(samLinear, Input.texCoord),GAMMA);
    float4 normal = txNormal.Sample(samLinear, Input.texCoord) * 2.0 - 1.0f;
    float roughness = txRoughness.Sample(samLinear, Input.texCoord).r;
    float metallic = txMetallic.Sample(samLinear, Input.texCoord).r;
    
    float3x3 TBN = float3x3(Input.tangent, Input.bitangent, Input.normal);
    normal.xyz = normalize(mul(normal.xyz, TBN));
    
    float3 lightPos = float3(-20.0f, 2.0f, 2.0f);
    
    float3 lightDir = normalize(lightPos - Input.posW);
    
    //el 0.04 es un promedio de los objetos dieléctricos
    float3 specularColor = lerp(0.04f, diffColor.rgb, metallic);
    
    float3 color = BRDF_Cook_Torrence(normal.xyz,
                                    lightDir,
                                    normalize(Input.posW - viewPos),
                                    normalize(reflect(lightDir, normal.xyz)),
                                    diffColor.rgb,
                                    specularColor,
                                    roughness);
    
    // Lo = kD + kS + kA
    return float4(pow(color, 1.0f / GAMMA), 1.0f);

}





PixelInput vertex_Gourard(VertexInput Input)
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
    
    //Gourard
    float3 lightPos = float3(10.0f, 10.0f, -3.0f);
    float3 lightDir = normalize(lightPos - Output.posW);
    Output.diffuse = dot(Output.normal, lightDir);
 
    return Output;
}

float4 pixel_Gourard(PixelInput Input) : SV_Target
{
    
    float4 color = txColor.Sample(samLinear, Input.texCoord);
    color.rgb *= Input.diffuse;
    
    return color;
}

float4 pixel_basic(PixelInput Input) : SV_Target
{
    float4 color = txColor.Sample(samLinear, Input.texCoord);
    
    return color;
}

float4 pixel_DirLight(PixelInput Input) : SV_Target
{
    //Light position
    float3 lightPos = float3(10.0f,10.0f,-3.0f);
    
    //Light Direction
    float3 lightDir = normalize(lightPos);

    //Fator de Incidencia de Lambert
    float DiffuseIncidence = dot(lightDir, Input.normal);
    
    float4 color = txColor.Sample(samLinear, Input.texCoord);
    color.rgb *= DiffuseIncidence;
    
    return color;

}



/*
float4 pixel_main(PixelInput Input) : SV_Target
{
    float4 normal = txNormal.Sample(samLinear, Input.texCoord) * 2.0 - 1.0f;
    float3x3 TBN = float3x3(Input.tangent, Input.bitangent, Input.normal);
    normal.xyz = normalize(mul(normal.xyz, TBN));
    
    float3 lightPos = float3(-20.0f, 2.0f, 2.0f);
    
    
    //Luz de direccional
    //float3 lightDir = normalize(lightPos);
    
    //Luz de punto
    //float3 lightDir = normalize(lightPos - Input.posW);
    
    float3 lightDir = normalize(lightPos - Input.posW);
    float3 spotDir = normalize(float3(1.0f, 0, 0));
    
    //Spotlight
    //float spotAngle = (50.0f * 20*sin(time)) * (3.14159265383f / 180.0f);
    float spotAngle = 25.0f  * (3.14159265383f / 180.0f);
    spotAngle = cos(spotAngle);
    
    float DiffuseIncidence = dot(lightDir, normal.xyz);
    float IncidenceToLight = dot(-spotDir, lightDir);
    
    //Specular
    //Compute light reflection
    float3 ReflectVector = reflect(lightDir, normal.xyz);
    float3 myViewDir = normalize(viewPos - Input.posW);
    
    float3 H = normalize(lightDir + myViewDir);
    
    float SpecularIncidence = max(0.0f, dot(myViewDir, H));
    SpecularIncidence = pow(SpecularIncidence, 38.0f);
    
    //F�rmula de la luz
    // R = 2(n * L) N - L
    
    if (IncidenceToLight < spotAngle)
    {
        DiffuseIncidence = 0.0f;
    }
    
    float4 color = txColor.Sample(samLinear, Input.texCoord);
    
    
    
    color.rgb += SpecularIncidence;
    color.rgb *= DiffuseIncidence;
    color.rgb += 0.2;
    
    // Lo = kD + kS + kA
    //return float4(normal.xyz, 1);
    //return float4(Input.tangent, 1.0f);
    return color;

}
*/