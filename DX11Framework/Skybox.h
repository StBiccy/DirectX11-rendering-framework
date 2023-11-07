#pragma once
#include <DirectXMath.h>
using namespace DirectX;

class Skybox
{
private:
	XMFLOAT4X4 _World;

public:
	void SetWorld(XMFLOAT4X4 world) { _World = world; }
};

