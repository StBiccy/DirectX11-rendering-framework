#pragma once
#include <DirectXMath.h>
#include <vector>
#include <string.h>

using namespace DirectX;

struct SimpleVertex
{
	XMFLOAT3 Position;
	XMFLOAT3 Normal;
	XMFLOAT2 TexCoord;
	XMFLOAT3 Tangent;
	XMFLOAT3 Bitangent;

	bool operator<(const SimpleVertex other) const
	{
		return memcmp((void*)this, (void*)&other, sizeof(SimpleVertex)) > 0;
	};
};

struct ConstantBuffer
{
	XMMATRIX Projection;
	XMMATRIX View;
	XMMATRIX World;
	XMFLOAT3 cameraPosition;
	float count;
	unsigned int hasTex;
	unsigned int hasNormMap;
	unsigned int hasSpecMap;
};

struct LightBuffer
{
	XMFLOAT4 AmbiantLight;
	XMFLOAT4 AmbiantMaterial;
	XMFLOAT4 DiffuseLight;
	XMFLOAT4 DiffuseMaterial;
	XMFLOAT4 SpecularLight;
	XMFLOAT4 SpecularMaterial;
	XMFLOAT3 LightDir;
	float SpecPower;

};
