#include "Skybox.h"

Skybox::Skybox(ID3D11Device* device, HWND* windHand, ID3D11Buffer* constbuff)
{
	_device = device;
    _windowHandle = windHand;
    _constantBuffer = constbuff;

    D3D11_DEPTH_STENCIL_DESC dsDescSkybox = {};
    dsDescSkybox.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
    dsDescSkybox.DepthEnable = true;
    dsDescSkybox.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;

    _device->CreateDepthStencilState(&dsDescSkybox, &_depthStencilSkybox);

    //Rasterizer
    D3D11_RASTERIZER_DESC rasterizerDesc = {};

    //fill state
    rasterizerDesc.FillMode = D3D11_FILL_SOLID;
    rasterizerDesc.CullMode = D3D11_CULL_NONE;

    _device->CreateRasterizerState(&rasterizerDesc, &_rasterizerSkybox);

    //cube
    _meshData = OBJLoader::Load("Models\\Models\\Car\\Car.obj", _device, false);

}

Skybox::~Skybox()
{
    if (_vertexShader)_vertexShader->Release();
    if (_pixelShader)_pixelShader->Release();
    if (_depthStencilSkybox)_depthStencilSkybox->Release();
    if (_rasterizerSkybox)_rasterizerSkybox->Release();
}

HRESULT Skybox::Init()
{
    HRESULT hr = S_OK;

    hr = InitShader();
    if (FAILED(hr)) return E_FAIL;

    return hr;
}

HRESULT Skybox::InitShader()
{
    HRESULT hr = S_OK;
    ID3DBlob* errorBlob;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

    ID3DBlob* vsBlob;

    hr = D3DCompileFromFile(L"SkyboxShader.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VS_main", "vs_5_0", dwShaderFlags, 0, &vsBlob, &errorBlob);
    if (FAILED(hr))
    {
        MessageBoxA(*_windowHandle, (char*)errorBlob->GetBufferPointer(), nullptr, ERROR);
        errorBlob->Release();
        return hr;
    }

    hr = _device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &_vertexShader);

    if (FAILED(hr)) return hr;

    ///////////////////////////////////////////////////////////////////////////////////////////////

    ID3DBlob* psBlob;

    hr = D3DCompileFromFile(L"SkyboxShader.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PS_main", "ps_5_0", dwShaderFlags, 0, &psBlob, &errorBlob);
    if (FAILED(hr))
    {
        MessageBoxA(*_windowHandle, (char*)errorBlob->GetBufferPointer(), nullptr, ERROR);
        errorBlob->Release();
        return hr;
    }


    hr = _device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &_pixelShader);

    vsBlob->Release();
    psBlob->Release();

    return hr;
}

void Skybox::Draw(ID3D11DeviceContext* immediateContext, ConstantBuffer& cbData)
{
    D3D11_MAPPED_SUBRESOURCE mappedSubresource;

    //immediateContext->RSSetState(_rasterizerSkybox);

    //cbData->World = XMMatrixTranspose(XMLoadFloat4x4(&_World));

    immediateContext->Map(_constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
    memcpy(mappedSubresource.pData, &cbData, sizeof(cbData));
    immediateContext->Unmap(_constantBuffer, 0);

    //immediateContext->VSSetShader(_vertexShader, nullptr, 0);
    //immediateContext->PSSetShader(_pixelShader, nullptr, 0);

    //immediateContext->OMSetDepthStencilState(_depthStencilSkybox, 0);
    //immediateContext->PSSetShaderResources(0, 1, &_texture);

    immediateContext->IASetVertexBuffers(0, 1, &_meshData.VertexBuffer, &_meshData.VBStride, &_meshData.VBOffset);
    immediateContext->IASetIndexBuffer(_meshData.IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

    immediateContext->DrawIndexed(_meshData.IndexCount, 0, 0);

}

void Skybox::Update(float DeltaTime)
{

}
