#pragma once
#include <DirectXMath.h>
using namespace DirectX;
class BaseCamera
{
private:
	XMFLOAT3 _eye;
	XMFLOAT3 _to;
	XMFLOAT3 _up;
	XMFLOAT3 _rotation;

	float _windowWidth;
	float _windowHight;
	float _nearDepth;
	float _farDepth;

	XMFLOAT4X4 _view;
	XMFLOAT4X4 _projection;
	XMFLOAT4X4 _viewProj;

public:
	BaseCamera(XMFLOAT3 rotation,XMFLOAT3 position, XMFLOAT3 at, XMFLOAT3 up, float windowWidth, float windowHeight, float nearDepth, float farDepth);
	~BaseCamera();

	void Update(float deltaTime);

	XMFLOAT3 GetTo() { return _to; }
	void SetTo(XMFLOAT3 to) 
	{ 
		_to = to; 
		XMStoreFloat4x4(&_view, XMMatrixLookToLH(XMLoadFloat3(&_eye), XMLoadFloat3(&_to), XMLoadFloat3(&_up)));
	}

	XMFLOAT3 GetEye() { return _eye; }
	void SetEye(XMFLOAT3 eye) 
	{ 
		_eye = eye; 
		XMStoreFloat4x4(&_view, XMMatrixLookToLH(XMLoadFloat3(&_eye), XMLoadFloat3(&_to), XMLoadFloat3(&_up)));
	}

	XMFLOAT3 GetUp() { return _up; }
	void SetUp(XMFLOAT3 up) 
	{ 
		_up = up; 
		XMStoreFloat4x4(&_view, XMMatrixLookToLH(XMLoadFloat3(&_eye), XMLoadFloat3(&_to), XMLoadFloat3(&_up)));
	}

	XMFLOAT3 GetRotation() { return _rotation; }
	void SetRotation(XMFLOAT3 rotation)
	{
		_rotation = rotation;
	}


	XMFLOAT4X4 GetView() { return _view; }
	XMFLOAT4X4 GetProj() { return _projection; }
	XMFLOAT4X4 getViewProj() { return  _viewProj; }
};

