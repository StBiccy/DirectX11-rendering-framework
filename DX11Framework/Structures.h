#pragma once
#include <DirectXMath.h>
#include <string.h>

using namespace DirectX;

struct SimpleVertex
{
	XMFLOAT3 Position;
	XMFLOAT3 Normal;
	XMFLOAT2 TexCoord;

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
	XMFLOAT4 AmbiantLight;
	XMFLOAT4 AmbiantMaterial;
	XMFLOAT4 DiffuseLight;
	XMFLOAT4 DiffuseMaterial;
	XMFLOAT4 specularLight;
	XMFLOAT4 specularMaterial;
	XMFLOAT3 cameraPosition;
	float specPower;
	XMFLOAT3 LightDir;
	float count;
	unsigned int hasTex;
	unsigned int hasNormMap;
	unsigned int hasSpecMap;
};
