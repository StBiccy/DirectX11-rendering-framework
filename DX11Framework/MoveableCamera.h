#pragma once
#include "BaseCamera.h"
#include <windows.h>
#include "Structures.h"

class MoveableCamera : public BaseCamera
{
	float _speed;
public:
	MoveableCamera(XMFLOAT3 rotation, XMFLOAT3 position, XMFLOAT3 at, XMFLOAT3 up, float windowWidth, float windowHeight, float nearDepth, float farDepth);

	void SetSpeed(float speed) { _speed = speed; }
	void Update(float deltaTime, ConstantBuffer& cbData, XMFLOAT4X4& View);
};

