#include "DX11Framework.h"
#include "DDSTextureLoader.h"
#include <string>

//#define RETURNFAIL(x) if(FAILED(x)) return x;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;

    switch (message)
    {
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

HRESULT DX11Framework::Initialise(HINSTANCE hInstance, int nShowCmd)
{
    HRESULT hr = S_OK;

    hr = CreateWindowHandle(hInstance, nShowCmd);
    if (FAILED(hr)) return E_FAIL;

    hr = CreateD3DDevice();
    if (FAILED(hr)) return E_FAIL;

    hr = CreateSwapChainAndFrameBuffer();
    if (FAILED(hr)) return E_FAIL;

    hr = InitShadersAndInputLayout();
    if (FAILED(hr)) return E_FAIL;

    hr = InitVertexIndexBuffers();
    if (FAILED(hr)) return E_FAIL;

    hr = InitPipelineVariables();
    if (FAILED(hr)) return E_FAIL;

    hr = InitRunTimeData();
    if (FAILED(hr)) return E_FAIL;

    return hr;
}

HRESULT DX11Framework::CreateWindowHandle(HINSTANCE hInstance, int nCmdShow)
{
    const wchar_t* windowName  = L"DX11Framework";

    WNDCLASSW wndClass;
    wndClass.style = 0;
    wndClass.lpfnWndProc = WndProc;
    wndClass.cbClsExtra = 0;
    wndClass.cbWndExtra = 0;
    wndClass.hInstance = 0;
    wndClass.hIcon = 0;
    wndClass.hCursor = 0;
    wndClass.hbrBackground = 0;
    wndClass.lpszMenuName = 0;
    wndClass.lpszClassName = windowName;

    RegisterClassW(&wndClass);

    _windowHandle = CreateWindowExW(0, windowName, windowName, WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 
        _WindowWidth, _WindowHeight, nullptr, nullptr, hInstance, nullptr);

    return S_OK;
}

HRESULT DX11Framework::CreateD3DDevice()
{
    HRESULT hr = S_OK;

    D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0,
    };

    ID3D11Device* baseDevice;
    ID3D11DeviceContext* baseDeviceContext;

    DWORD createDeviceFlags = 0;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
    hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_BGRA_SUPPORT | createDeviceFlags, featureLevels, ARRAYSIZE(featureLevels), D3D11_SDK_VERSION, &baseDevice, nullptr, &baseDeviceContext);
    if (FAILED(hr)) return hr;

    ///////////////////////////////////////////////////////////////////////////////////////////////

    hr = baseDevice->QueryInterface(__uuidof(ID3D11Device), reinterpret_cast<void**>(&_device));
    hr = baseDeviceContext->QueryInterface(__uuidof(ID3D11DeviceContext), reinterpret_cast<void**>(&_immediateContext));

    baseDevice->Release();
    baseDeviceContext->Release();

    ///////////////////////////////////////////////////////////////////////////////////////////////

    hr = _device->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&_dxgiDevice));
    if (FAILED(hr)) return hr;

    IDXGIAdapter* dxgiAdapter;
    hr = _dxgiDevice->GetAdapter(&dxgiAdapter);
    hr = dxgiAdapter->GetParent(__uuidof(IDXGIFactory2), reinterpret_cast<void**>(&_dxgiFactory));
    dxgiAdapter->Release();

    return S_OK;
}

HRESULT DX11Framework::CreateSwapChainAndFrameBuffer()
{
    HRESULT hr = S_OK;

    DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
    swapChainDesc.Width = 0; // Defer to WindowWidth
    swapChainDesc.Height = 0; // Defer to WindowHeight
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; //FLIP* modes don't support sRGB backbuffer
    swapChainDesc.Stereo = FALSE;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = 2;
    swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
    swapChainDesc.Flags = 0;

    hr = _dxgiFactory->CreateSwapChainForHwnd(_device, _windowHandle, &swapChainDesc, nullptr, nullptr, &_swapChain);
    if (FAILED(hr)) return hr;

    ///////////////////////////////////////////////////////////////////////////////////////////////

    ID3D11Texture2D* frameBuffer = nullptr;

    hr = _swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&frameBuffer));
    if (FAILED(hr)) return hr;

    D3D11_RENDER_TARGET_VIEW_DESC framebufferDesc = {};
    framebufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; //sRGB render target enables hardware gamma correction
    framebufferDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

    hr = _device->CreateRenderTargetView(frameBuffer, &framebufferDesc, &_frameBufferView);


    D3D11_TEXTURE2D_DESC depthBufferDesc = {};
    frameBuffer->GetDesc(&depthBufferDesc);

    depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    _device->CreateTexture2D(&depthBufferDesc, nullptr, &_depthStencilBuffer);
    _device->CreateDepthStencilView(_depthStencilBuffer, nullptr, &_depthStencilView);

    frameBuffer->Release();


    return hr;
}

HRESULT DX11Framework::InitShadersAndInputLayout()
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

    hr =  D3DCompileFromFile(L"SimpleShaders.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VS_main", "vs_5_0", dwShaderFlags, 0, &vsBlob, &errorBlob);
    if (FAILED(hr))
    {
        MessageBoxA(_windowHandle, (char*)errorBlob->GetBufferPointer(), nullptr, ERROR);
        errorBlob->Release();
        return hr;
    }

    hr = _device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &_vertexShader);

    if (FAILED(hr)) return hr;

    D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA,   0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,   0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    hr = _device->CreateInputLayout(inputElementDesc, ARRAYSIZE(inputElementDesc), vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &_inputLayout);
    if (FAILED(hr)) return hr;

    ///////////////////////////////////////////////////////////////////////////////////////////////

    ID3DBlob* psBlob;

    hr = D3DCompileFromFile(L"SimpleShaders.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PS_main", "ps_5_0", dwShaderFlags, 0, &psBlob, &errorBlob);
    if (FAILED(hr))
    {
        MessageBoxA(_windowHandle, (char*)errorBlob->GetBufferPointer(), nullptr, ERROR);
        errorBlob->Release();
        return hr;
    }

    hr = _device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &_pixelShader);

    vsBlob->Release();
    psBlob->Release();

    return hr;
}

HRESULT DX11Framework::InitVertexIndexBuffers()
{
    HRESULT hr = S_OK;

    SimpleVertex CubeVertexData[] =
    {
        //Position                     //Normal           
        { XMFLOAT3(-1.00f,  1.00f, -1), XMFLOAT3(-1.00f,  1.00f, -1),  XMFLOAT2(0.0f,1.0f)},
        { XMFLOAT3(1.00f,  1.00f, -1),  XMFLOAT3(1.00f,  1.00f, -1),  XMFLOAT2(1.0f,1.0f)},
        { XMFLOAT3(-1.00f, -1.00f, -1), XMFLOAT3(-1.00f, -1.00f, -1),  XMFLOAT2(0.0f,0.0f)},
        { XMFLOAT3(1.00f, -1.00f, -1),  XMFLOAT3(1.00f, -1.00f, -1),  XMFLOAT2(1.0f,0.0f)},

        { XMFLOAT3(-1.00f,  1.00f, 1), XMFLOAT3(-1.00f,  1.00f, 1),  XMFLOAT2(1.0f,1.0f)},
        { XMFLOAT3(1.00f,  1.00f, 1),  XMFLOAT3(1.00f,  1.00f, 1),  XMFLOAT2(0.0f,1.0f)},
        { XMFLOAT3(-1.00f, -1.00f, 1), XMFLOAT3(-1.00f, -1.00f, 1),  XMFLOAT2(1.0f,0.0f)},
        { XMFLOAT3(1.00f, -1.00f, 1),  XMFLOAT3(1.00f, -1.00f, 1),  XMFLOAT2(0.0f,0.0f)},
    };

    D3D11_BUFFER_DESC cubeVertexBufferDesc = {};
    cubeVertexBufferDesc.ByteWidth = sizeof(CubeVertexData);
    cubeVertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
    cubeVertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA cubedVertexData = { CubeVertexData };

    hr = _device->CreateBuffer(&cubeVertexBufferDesc, &cubedVertexData, &_cubeVertexBuffer);
    if (FAILED(hr)) return hr;

    ///////////////////////////////////////////////////////////////////////////////////////////////

    WORD CubeIndexData[] = 
    {
        //Indices
        0, 1, 2,
        2, 1, 3,

        7, 6, 3,
        6, 2, 3,

        0, 6, 4,
        6, 0, 2,

        6, 5, 4,
        7, 5, 6,

        4, 1, 0,
        4, 5, 1,

        1, 5, 7,
        3, 1,7,
    };

    D3D11_BUFFER_DESC CubeBufferDesc = {};
    CubeBufferDesc.ByteWidth = sizeof(CubeIndexData);
    CubeBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
    CubeBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA cubeIndexData = { CubeIndexData };

    hr = _device->CreateBuffer(&CubeBufferDesc, &cubeIndexData, &_cubeIndexBuffer);
    if (FAILED(hr)) return hr;

    ////////////////////////////////////////////////////////////////////////////////////////////////

    //SimpleVertex PyramidVertexData[] =
    //{
    //    //Position                     //Color             
    //    { XMFLOAT3(1.00f,  -1.00f, -1), XMFLOAT4(1.0f,  0.0f, 0.0f,  0.0f)},
    //    { XMFLOAT3(-1.00f, -1.00f, -1), XMFLOAT4(0.0f,  0.0f, 1.0f,  0.0f)},
    //    { XMFLOAT3(1.00f,  -1.00f, 1), XMFLOAT4(1.0f,  0.0f, 0.0f,  0.0f)},
    //    { XMFLOAT3(-1.00f, -1.00f, 1), XMFLOAT4(0.0f,  0.0f, 1.0f,  0.0f)},

    //    { XMFLOAT3(0.00f, 1.00f, 0), XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f)}
    //};

    //D3D11_BUFFER_DESC pyramidVertexBufferDesc = {};
    //pyramidVertexBufferDesc.ByteWidth = sizeof(PyramidVertexData);
    //pyramidVertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
    //pyramidVertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    //D3D11_SUBRESOURCE_DATA pyramidVertexData = { PyramidVertexData };

    //hr = _device->CreateBuffer(&pyramidVertexBufferDesc, &pyramidVertexData, &_pyramidVertexBuffer);
    //if (FAILED(hr)) return hr;

    ///////////////////////////////////////////////////////////////////////////////////////////////

    /*WORD PyramidIndexData[] =
    {
        //Indices
        2, 1, 0,
        2, 3, 1,

        0, 1, 4,
        2, 0, 4,
        4, 3, 2,
        4, 1, 3
    };

    D3D11_BUFFER_DESC PyramidBufferDesc = {};
    PyramidBufferDesc.ByteWidth = sizeof(PyramidIndexData);
    PyramidBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
    PyramidBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA pyramidIndexData = { PyramidIndexData };

    hr = _device->CreateBuffer(&PyramidBufferDesc, &pyramidIndexData, &_pyramidIndexBuffer);
    if (FAILED(hr)) return hr;*/

    //////

 /*   SimpleVertex LineList[] =
    {
        { XMFLOAT3(0,3,0), XMFLOAT4(1,1,1,1)},
        { XMFLOAT3(0,5,0), XMFLOAT4(1,1,1,1)},
    };*/

    //D3D11_BUFFER_DESC lineVertexBufferDesc = {};
    //lineVertexBufferDesc.ByteWidth = sizeof(LineList);
    //lineVertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
    //lineVertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    //D3D11_SUBRESOURCE_DATA lineVertexData = { LineList };

    //hr = _device->CreateBuffer(&lineVertexBufferDesc, &lineVertexData, &_lineVertexBuffer);
    //if (FAILED(hr)) return hr;

    return S_OK;
}

HRESULT DX11Framework::InitPipelineVariables()
{
    HRESULT hr = S_OK;

    //Input Assembler
    _immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    _immediateContext->IASetInputLayout(_inputLayout);

    //Rasterizer
    D3D11_RASTERIZER_DESC rasterizerDesc = {};

    //fill state
    rasterizerDesc.FillMode = D3D11_FILL_SOLID;
    rasterizerDesc.CullMode = D3D11_CULL_BACK;

    hr = _device->CreateRasterizerState(&rasterizerDesc, &_fillState);
    if (FAILED(hr)) return hr;

    //wireframe state
    rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
    rasterizerDesc.CullMode = D3D11_CULL_NONE;

    hr = _device->CreateRasterizerState(&rasterizerDesc, &_wireframeState);
    if (FAILED(hr)) return hr;

    _immediateContext->RSSetState(_wireframeState);


    //Viewport Values
    _viewport = { 0.0f, 0.0f, (float)_WindowWidth, (float)_WindowHeight, 0.0f, 1.0f };
    _immediateContext->RSSetViewports(1, &_viewport);

    //Constant Buffer
    D3D11_BUFFER_DESC constantBufferDesc = {};
    constantBufferDesc.ByteWidth = sizeof(ConstantBuffer);
    constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    hr = _device->CreateBuffer(&constantBufferDesc, nullptr, &_constantBuffer);
    if (FAILED(hr)) { return hr; }

    _immediateContext->VSSetConstantBuffers(0, 1, &_constantBuffer);
    _immediateContext->PSSetConstantBuffers(0, 1, &_constantBuffer);

    //Sampler State
    D3D11_SAMPLER_DESC bilinearSamplerDesc = {};
    bilinearSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    bilinearSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    bilinearSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    bilinearSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    bilinearSamplerDesc.MaxLOD = 1;
    bilinearSamplerDesc.MinLOD = 0;
    
    hr = _device->CreateSamplerState(&bilinearSamplerDesc, &_bilinearSampleState);
    if (FAILED(hr)) { return hr; }

    _immediateContext->PSSetSamplers(0, 1, &_bilinearSampleState);

    return S_OK;
}

HRESULT DX11Framework::InitRunTimeData()
{
    HRESULT hr;
    //Light
    _diffuseLight = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
    _diffuseMaterial = XMFLOAT4(0.0f, 0.0f, 0.5f, 1.0f);
    _lightDir = XMFLOAT3(0.0, 0.0f, 1.0f);

    _ambiantLight = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
    _ambiantMaterial = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

    _specularMaterial = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    _specularLight = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
    _specPower = 10.0f;

    //Load Textures;
    hr = CreateDDSTextureFromFile(_device, L"Textures\\Crate_COLOR.dds", nullptr, &_createTexture);
    _immediateContext->PSSetShaderResources(0, 1, &_createTexture);

    //Camera
    float aspect = _viewport.Width / _viewport.Height;

    XMFLOAT3 Eye = XMFLOAT3(0, 0, -3.0f);
    XMFLOAT3 At = XMFLOAT3(0, 0, 0);
    XMFLOAT3 Up = XMFLOAT3(0, 1, 0);

    //set camera position in the shader
    _cbData.cameraPosition = Eye;

    XMStoreFloat4x4(&_View, XMMatrixLookAtLH(XMLoadFloat3(&Eye), XMLoadFloat3(&At), XMLoadFloat3(&Up)));

    //Projection
    XMMATRIX perspective = XMMatrixPerspectiveFovLH(XMConvertToRadians(90), aspect, 0.01f, 100.0f);
    XMStoreFloat4x4(&_Projection, perspective);

    return S_OK;
}

DX11Framework::~DX11Framework()
{
    if(_immediateContext)_immediateContext->Release();
    if(_device)_device->Release();
    if(_dxgiDevice)_dxgiDevice->Release();
    if(_dxgiFactory)_dxgiFactory->Release();
    if(_frameBufferView)_frameBufferView->Release();
    if (_depthStencilBuffer)_depthStencilBuffer->Release();
    if (_depthStencilView)_depthStencilView->Release(); 
    if(_swapChain)_swapChain->Release();

    if (_fillState)_fillState->Release();
    if(_wireframeState)_wireframeState->Release();
    if(_vertexShader)_vertexShader->Release();
    if(_inputLayout)_inputLayout->Release();
    if(_pixelShader)_pixelShader->Release();
    if(_constantBuffer)_constantBuffer->Release();
    if(_cubeVertexBuffer)_cubeVertexBuffer->Release();
    if(_cubeIndexBuffer)_cubeIndexBuffer->Release();
    if (_pyramidVertexBuffer)_pyramidVertexBuffer->Release();
    if (_pyramidIndexBuffer)_pyramidIndexBuffer->Release();
}


void DX11Framework::Update()
{
    //Static initializes this value only once    
    static ULONGLONG frameStart = GetTickCount64();

    ULONGLONG frameNow = GetTickCount64();
    float deltaTime = (frameNow - frameStart) / 1000.0f;
    frameStart = frameNow;

    static float simpleCount = 0.0f;
    simpleCount += deltaTime;

    XMStoreFloat4x4(&_World, XMMatrixIdentity() * XMMatrixRotationY(simpleCount) * XMMatrixTranslation(0,0,4));
    XMStoreFloat4x4(&_World2, XMMatrixIdentity() * XMMatrixRotationY(simpleCount)* XMMatrixTranslation(5, 0, 0) * XMLoadFloat4x4(&_World));
    XMStoreFloat4x4(&_World3, XMMatrixIdentity() * XMMatrixRotationY(simpleCount) * XMMatrixTranslation(2,0, 0) * XMLoadFloat4x4(&_World2));
    XMStoreFloat4x4(&_World4, XMMatrixIdentity() * XMMatrixTranslation(0, -2, 0));
    

    if (GetAsyncKeyState(VK_F1) & 0x0001)
    {
        _immediateContext->RSSetState(_fillState);
    }

    _cbData.count = simpleCount;

}

void DX11Framework::Draw()
{    
    _cbData.DiffuseLight = _diffuseLight;
    _cbData.DiffuseMaterial = _diffuseMaterial;
    _cbData.LightDir = _lightDir;
    _cbData.AmbiantLight = _ambiantLight;
    _cbData.AmbiantMaterial = _ambiantMaterial;

    _cbData.specularLight = _specularLight;
    _cbData.specularMaterial = _specularMaterial;
    _cbData.specPower = _specPower;

    _immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    //Present unbinds render target, so rebind and clear at start of each frame
    float backgroundColor[4] = { 0.025f, 0.025f, 0.025f, 1.0f };  
    _immediateContext->OMSetRenderTargets(1, &_frameBufferView, _depthStencilView);
    _immediateContext->ClearRenderTargetView(_frameBufferView, backgroundColor);
    _immediateContext->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0.0f);
   
    //Store this frames data in constant buffer struct
    _cbData.World = XMMatrixTranspose(XMLoadFloat4x4(&_World));
    _cbData.View = XMMatrixTranspose(XMLoadFloat4x4(&_View));
    _cbData.Projection = XMMatrixTranspose(XMLoadFloat4x4(&_Projection));

    //Write constant buffer data onto GPU
    D3D11_MAPPED_SUBRESOURCE mappedSubresource;
    _immediateContext->Map(_constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
    memcpy(mappedSubresource.pData, &_cbData, sizeof(_cbData));
    _immediateContext->Unmap(_constantBuffer, 0);

    //Set object variables and draw
    UINT stride = {sizeof(SimpleVertex)};
    UINT offset =  0 ;
    _immediateContext->IASetVertexBuffers(0, 1, &_cubeVertexBuffer, &stride, &offset);
    _immediateContext->IASetIndexBuffer(_cubeIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

    _immediateContext->VSSetShader(_vertexShader, nullptr, 0);
    _immediateContext->PSSetShader(_pixelShader, nullptr, 0);

    _immediateContext->DrawIndexed(36, 0, 0);

    //////

    _cbData.World = XMMatrixTranspose(XMLoadFloat4x4(&_World3));

    //Write constant buffer data onto GPU
    _immediateContext->Map(_constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
    memcpy(mappedSubresource.pData, &_cbData, sizeof(_cbData));
    _immediateContext->Unmap(_constantBuffer, 0);

    _immediateContext->IASetVertexBuffers(0, 1, &_cubeVertexBuffer, &stride, &offset);
    _immediateContext->IASetIndexBuffer(_cubeIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

    _immediateContext->DrawIndexed(36, 0, 0);

    ///////

    _cbData.World = XMMatrixTranspose(XMLoadFloat4x4(&_World2));
    //Write constant buffer data onto GPU
    _immediateContext->Map(_constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
    memcpy(mappedSubresource.pData, &_cbData, sizeof(_cbData));
    _immediateContext->Unmap(_constantBuffer, 0);


    _immediateContext->DrawIndexed(36, 0, 0);

    //////
    //_cbData.World = XMMatrixTranspose(XMLoadFloat4x4(&_World4));
    //_immediateContext->Map(_constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
    //memcpy(mappedSubresource.pData, &_cbData, sizeof(_cbData));
    //_immediateContext->Unmap(_constantBuffer, 0);


    //_immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
    //_immediateContext->IASetVertexBuffers(0, 1, &_lineVertexBuffer, &stride, &offset);
    //_immediateContext->Draw(2, 0);


    //Present Backbuffer to screen
    _swapChain->Present(0, 0);
}