TextureCube skybox : register(t0);
SamplerState bilinierSampler : register(s0);

cbuffer ConstantBuffer : register(b0)
{
    float4x4 Projection;
    float4x4 View;
    float4x4 World;
    float4 AmbiantLight;
    float4 AmbiantMaterial;
    float4 DiffuseLight;
    float4 DiffuseMaterial;
    float4 specularLight;
    float4 specularMaterial;
    float3 cameraPosition;
    float specPower;
    float3 LightDir;
    float count;
    uint hasTex;
    uint hasNormMap;
    uint hasSpecMap;
}

struct SkyboxVS_Out
{
    float4 position : POSITION;
    float3 texCoord : TEXCOORD;
};

SkyboxVS_Out VS_main(float3 Position : POSITION, float3 Normal : NORMAL, float2 TexCoord : TEXCOORD)
{
    SkyboxVS_Out output = (SkyboxVS_Out) 0;
    
    output.position = mul(output.position, View);
    output.position = mul(output.position, Projection);
    output.position = mul(output.position, World);
    
    output.position = output.position.xyww;
    
    output.texCoord = Position;
    
    return output;
}

float4 PS_main(SkyboxVS_Out input) : SV_TARGET
{

}