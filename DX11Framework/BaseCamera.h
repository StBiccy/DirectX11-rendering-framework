#pragma once
#include <DirectXMath.h>
using namespace DirectX;
class BaseCamera
{
private:
	XMFLOAT3 _eye;
	XMFLOAT3 _at;
	XMFLOAT3 _up;
	
	float _windowWidth;
	float _windowHight;
	float _nearDepth;
	float _farDepth;

	XMFLOAT4X4 _view;
	XMFLOAT4X4 _projection;
	XMFLOAT4X4 _viewProj;

public:
	BaseCamera(XMFLOAT3 position, XMFLOAT3 at, XMFLOAT3 up, float windowWidth, float windowHeight, float nearDepth, float farDepth);
	~BaseCamera();

	void Update(float deltaTime);

	XMFLOAT3 GetAt() { return _at; }
	void SetAt(XMFLOAT3 at) { _at = at; }
	XMFLOAT3 GetEye() { return _eye; }
	void SetEye(XMFLOAT3 eye) { _eye = eye; }
	XMFLOAT3 GetUp() { return _up; }
	void SetUp(XMFLOAT3 up) { _up = up; }


	XMFLOAT4X4 GetView() { return _view; }
	XMFLOAT4X4 GetProj() { return _projection; }
	XMFLOAT4X4 getViewProj() { return  _viewProj; }
};

