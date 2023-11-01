#include "BaseCamera.h"

BaseCamera::BaseCamera(XMFLOAT3 position, XMFLOAT3 at, XMFLOAT3 up, float windowWidth, float windowHeight, float nearDepth, float farDepth)
{
    _windowWidth = windowWidth;
    _windowHight = windowHeight;
    //Camera
    float aspect = _windowWidth / _windowHight;

    _eye = position;
    _at = at;
    _up = up;

    XMStoreFloat4x4(&_view, XMMatrixLookAtLH(XMLoadFloat3(&_eye), XMLoadFloat3(&_at), XMLoadFloat3(&_up)));

    //Projection
    XMMATRIX perspective = XMMatrixPerspectiveFovLH(XMConvertToRadians(90), aspect, 0.01f, 100.0f);
    XMStoreFloat4x4(&_projection, perspective);
}

BaseCamera::~BaseCamera()
{
}

void BaseCamera::Update(float deltaTime)
{
    XMStoreFloat4x4(&_viewProj, XMLoadFloat4x4(&_view) * XMLoadFloat4x4(&_projection));
}
