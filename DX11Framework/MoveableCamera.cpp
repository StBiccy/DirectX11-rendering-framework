#include "MoveableCamera.h"

MoveableCamera::MoveableCamera(XMFLOAT3 rotation, XMFLOAT3 position, XMFLOAT3 at, XMFLOAT3 up, float windowWidth, float windowHeight, float nearDepth, float farDepth) : BaseCamera(rotation, position, at, up, windowWidth, windowHeight, nearDepth, farDepth)
{
}

void MoveableCamera::Update(float deltaTime, ConstantBuffer& cbData, XMFLOAT4X4& View)
{
    //cameraControls
    if (GetKeyState(0x41) & 0x8000)
    {
        XMFLOAT3 currentPos = GetEye();
        XMFLOAT3 right = GetRight();
        XMStoreFloat3(&currentPos, XMLoadFloat3(&currentPos) - (XMLoadFloat3(&right) * -_speed * deltaTime));

        SetEye(currentPos);
        cbData.cameraPosition = GetEye();
        View = GetView();

        XMFLOAT4X4 world;
        XMFLOAT3 pos = GetEye();
        XMStoreFloat4x4(&world, XMMatrixIdentity() * XMMatrixTranslation(pos.x, pos.y, pos.z));

    }
    if (GetKeyState(0x44) & 0x8000)
    {
        XMFLOAT3 currentPos = GetEye();
        XMFLOAT3 right = GetRight();
        XMStoreFloat3(&currentPos, XMLoadFloat3(&currentPos) - (XMLoadFloat3(&right) * _speed * deltaTime));

        SetEye(currentPos);
        cbData.cameraPosition = GetEye();
        View = GetView();

        XMFLOAT4X4 world;
        XMFLOAT3 pos = GetEye();
        XMStoreFloat4x4(&world, XMMatrixIdentity() * XMMatrixTranslation(pos.x, pos.y, pos.z));
    }
    if (GetKeyState(0x53) & 0x8000)
    {
        XMFLOAT3 currentPos = GetEye();
        XMFLOAT3 to = GetTo();

        XMStoreFloat3(&currentPos, XMLoadFloat3(&currentPos) + (XMLoadFloat3(&to) * -_speed * deltaTime));
        SetEye(currentPos);

        cbData.cameraPosition = GetEye();
        View = GetView();

        XMFLOAT4X4 world;
        XMFLOAT3 pos = GetEye();
        XMStoreFloat4x4(&world, XMMatrixIdentity() * XMMatrixTranslation(pos.x, pos.y, pos.z));
    }
    if (GetKeyState(0x57) & 0x8000)
    {
        XMFLOAT3 currentPos = GetEye();
        XMFLOAT3 to = GetTo();

        XMStoreFloat3(&currentPos, XMLoadFloat3(&currentPos) + (XMLoadFloat3(&to) * +_speed * deltaTime));
        SetEye(currentPos);

        cbData.cameraPosition = GetEye();
        View = GetView();

        XMFLOAT4X4 world;
        XMFLOAT3 pos = GetEye();
        XMStoreFloat4x4(&world, XMMatrixIdentity() * XMMatrixTranslation(pos.x, pos.y, pos.z));
    }

    //cameraRotation
    if (GetKeyState(0x25) & 0x8000)
    {
        XMFLOAT3 up = XMFLOAT3(0, 1, 0);
        XMFLOAT3 forward = GetTo();

        XMMATRIX rotMat = XMMatrixRotationAxis(XMLoadFloat3(&up), -1 * deltaTime);
        XMStoreFloat3(&forward, XMVector3TransformNormal(XMLoadFloat3(&forward), rotMat));

        XMStoreFloat3(&up, XMVector3TransformNormal(XMLoadFloat3(&up), rotMat));


        SetTo(forward);
        SetUp(up);
        View = GetView();
    }
    if (GetKeyState(0x27) & 0x8000)
    {
        XMFLOAT3 up = XMFLOAT3(0, 1, 0);
        XMFLOAT3 forward = GetTo();
        XMMATRIX rotMat = XMMatrixRotationAxis(XMLoadFloat3(&up), 1 * deltaTime);
        XMStoreFloat3(&forward, XMVector3TransformNormal(XMLoadFloat3(&forward), rotMat));

        XMStoreFloat3(&up, XMVector3TransformNormal(XMLoadFloat3(&up), rotMat));

        SetTo(forward);
        SetUp(up);
        View = GetView();
    }
    if (GetKeyState(0x26) & 0x8000)
    {
        XMFLOAT3 up = GetUp();
        XMFLOAT3 forward = GetTo();
        XMFLOAT3 right = GetRight();

        XMMATRIX rotMat = XMMatrixRotationAxis(XMLoadFloat3(&right), 1 * deltaTime);
        XMStoreFloat3(&forward, XMVector3TransformNormal(XMLoadFloat3(&forward), rotMat));
        XMStoreFloat3(&up, XMVector3TransformNormal(XMLoadFloat3(&up), rotMat));

        SetTo(forward);
        SetUp(up);
        View = GetView();
    }
    if (GetKeyState(0x28) & 0x8000)
    {
        XMFLOAT3 up = GetUp();
        XMFLOAT3 forward = GetTo();
        XMFLOAT3 right;
        XMStoreFloat3(&right, XMVector3Cross(XMLoadFloat3(&forward), XMLoadFloat3(&up)));

        XMMATRIX rotMat = XMMatrixRotationAxis(XMLoadFloat3(&right), -1 * deltaTime);
        XMStoreFloat3(&forward, XMVector3TransformNormal(XMLoadFloat3(&forward), rotMat));
        XMStoreFloat3(&up, XMVector3TransformNormal(XMLoadFloat3(&up), rotMat));

        SetTo(forward);
        SetUp(up);
        View = GetView();
    }

    BaseCamera::Update(deltaTime);
}
