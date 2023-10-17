cbuffer ConstantBuffer : register(b0)
{
    float4x4 Projection;
    float4x4 View;
    float4x4 World;
    float4 DiffuseLight;
    float4 DiffuseMaterial;
    float3 LightDir;
	float count;
}

struct VS_Out
{
    float4 position : SV_POSITION;
    float3 posW : POSITION0;
    float3 normalW : NORMAL;
    float4 diffuseAmount;
};

VS_Out VS_main(float3 Position : POSITION, float3 Normal: NORMAL)
{   
    VS_Out output = (VS_Out)0;

    Position.y += sin(count);
    
    float4 Pos4 = float4(Position, 1.0f);
    output.position = mul(Pos4, World);
    output.posW = output.position;
    output.position = mul(output.position, View);
    output.position = mul(output.position, Projection);
    
    float3 normDir = Normal, World;
    
    output.normalW = normalize(normDir);
    float d = dot(LightDir, output.normalW);
    output.diffuseAmount = cos()
    
    return output;
}

float4 PS_main(VS_Out input) : SV_TARGET
{
    return float4(0,0,0,0);
}