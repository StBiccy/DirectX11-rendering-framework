#include "BaseCamera.h"

BaseCamera::BaseCamera(XMFLOAT3 rotation, XMFLOAT3 position, XMFLOAT3 to, XMFLOAT3 up, float windowWidth, float windowHeight, float nearDepth, float farDepth)
{
    _windowWidth = windowWidth;
    _windowHight = windowHeight;
    //Camera
    float aspect = _windowWidth / _windowHight;

    _rotation = rotation;

    _eye = position;
    _to = to;
    _up = up;

    XMStoreFloat4x4(&_view, XMMatrixLookToLH(XMLoadFloat3(&_eye), XMLoadFloat3(&_to), XMLoadFloat3(&_up)));

    //Projection
    XMMATRIX perspective = XMMatrixPerspectiveFovLH(XMConvertToRadians(90), aspect, nearDepth, farDepth);
    XMStoreFloat4x4(&_projection, perspective);
}

BaseCamera::~BaseCamera()
{
}

void BaseCamera::Update(float deltaTime)
{
    XMStoreFloat4x4(&_viewProj, XMLoadFloat4x4(&_view) * XMLoadFloat4x4(&_projection));
}


