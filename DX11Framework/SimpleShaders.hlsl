Texture2D diffuseTex : register(t0);
SamplerState bilinearSampler : register(s0);

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
    uint hasTexture;
}

struct VS_Out
{
    float4 position : SV_POSITION;
    float3 posW : POSITION0;
    float3 normal : NORMAL;
    float4 color : COLOR;
    float3 worldNormalPosition : POSTITIONWN;
    float2 texCoord : TEXCOOORD;
};

VS_Out VS_main(float3 Position : POSITION, float3 Normal: NORMAL, float2 TexCoord : TEXCOORD)
{   
    VS_Out output = (VS_Out)0;

    
    float4 Pos4 = float4(Position, 1.0f);
    output.position = mul(Pos4, World);
    output.posW = output.position;
    output.position = mul(output.position, View);
    output.position = mul(output.position, Projection);
    
    output.worldNormalPosition = mul(float4(Normal, 0), World);
    output.texCoord = TexCoord;
    
    return output;
}

float4 PS_main(VS_Out input) : SV_TARGET
{
    float4 texColor = diffuseTex.Sample(bilinearSampler, input.texCoord);
    
    float3 normalW = normalize(input.worldNormalPosition);
    
    float DiffuseAmount = saturate(dot(normalW, -normalize(LightDir)));    
    float4 diffuse = DiffuseAmount * (texColor * DiffuseLight);
    float4 Ambiant = AmbiantLight * texColor;     
    
    
    float3 viewerDir = input.posW - cameraPosition;
    float SpecIntesity = saturate(-dot(normalize(reflect(LightDir, normalW)), normalize(viewerDir)));

    SpecIntesity = pow(SpecIntesity, specPower); 

    float4 Specualr = SpecIntesity * (specularMaterial * specularLight);
    return diffuse + Ambiant + Specualr;
}