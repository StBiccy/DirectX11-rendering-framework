Texture2D diffuseTex : register(t0);
Texture2D specTex : register(t1);
Texture2D normTex : register(t2);
SamplerState bilinearSampler : register(s0);

cbuffer ConstantBuffer : register(b0)
{
    float4x4 Projection;
    float4x4 View;
    float4x4 World;
    float3 cameraPosition;
	float count;
    uint hasTex;
    uint hasNormMap;
    uint hasSpecMap;
}

cbuffer LightBuffer : register(b1)
{
    float4 AmbiantLight;
    float4 AmbiantMaterial;
    float4 DiffuseLight;
    float4 DiffuseMaterial;
    float4 SpecularLight;
    float4 SpecularMaterial;
    float3 LightDir;
    float SpecPower;

}

struct VS_Out
{
    float4 position : SV_POSITION;
    float3 posW : POSITION0;
    float3 normal : NORMAL;
    float4 color : COLOR;
    float3 worldNormalPosition : POSTITIONWN;
    float2 texCoord : TEXCOOORD;
    float3 tangent : TANGENT0;
    float3 bitangent : BITANGENT0;
    float3x3 TBNMatrix : TBN;
};

VS_Out VS_main(float3 Position : POSITION, float3 Normal : NORMAL, float2 TexCoord : TEXCOORD, float3 Tangent : TANGENT, float3 Bitangent : BITANGENT)
{   
    VS_Out output = (VS_Out)0;
     
    float4 Pos4 = float4(Position, 1.0f);
    output.position = mul(Pos4, World);
    output.posW = output.position;
    output.position = mul(output.position, View);
    output.position = mul(output.position, Projection);
    
    output.worldNormalPosition = normalize(mul(float4(Normal, 0), World));
    
 
    output.tangent = Tangent;
    output.bitangent = Tangent * Normal;
    
    output.normal = Normal;
    output.texCoord = TexCoord;
    
    return output;
}

float4 PS_main(VS_Out input) : SV_TARGET
{
    float3 normalW;
    if(hasNormMap == 1)
    {
        //obtain normal  from normal map in range [0,1]
        float3 T = normalize(mul(float4(input.tangent, 0), World));
        float3 B = normalize(mul(float4(input.bitangent, 0), World));
        
        input.TBNMatrix = float3x3(T, B, input.worldNormalPosition);
        
        normalW = normTex.Sample(bilinearSampler, input.texCoord).rgb;
        //transform normal vector  to range [-1,1]        
        normalW = normalize(normalW);
        normalW = normalize(mul(input.TBNMatrix, normalW));
    }
    else
    {
        normalW = normalize(input.worldNormalPosition);
    }
    
    float DiffuseAmount = saturate(dot(normalW, -normalize(LightDir))); 
    float4 diffuse;
    float4 ambiant;
    
    if(hasTex == 1)
    {
        float4 texColor = diffuseTex.Sample(bilinearSampler, input.texCoord);
        diffuse = DiffuseAmount * (texColor * DiffuseLight);
        ambiant = AmbiantLight * texColor;
    }
    else
    {
        diffuse = DiffuseAmount * (DiffuseMaterial * DiffuseLight);
        ambiant = AmbiantLight * AmbiantMaterial;
    }
    
    //Specular Lighting Calculation
    float3 viewerDir = input.posW - cameraPosition;
    float SpecIntesity = saturate(-dot(normalize(reflect(LightDir, normalW)), normalize(viewerDir)));
    SpecIntesity = pow(SpecIntesity, SpecPower); 
    
    float4 specualr;
    
    if(hasSpecMap == 1)
    {
        float4 specMap = specTex.Sample(bilinearSampler, input.texCoord);
        specualr = SpecIntesity * (specMap * SpecularLight);
    }
    else
    {
        specualr = SpecIntesity * (SpecularMaterial * SpecularLight);
    }
    

    return diffuse + ambiant + specualr;
}
