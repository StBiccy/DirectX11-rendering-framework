#pragma once
#include "Structures.h"
#include <windows.h>
#include <d3d11_4.h>
#include <d3dcompiler.h>
#include <vector>
#include "OBJLoader.h"
#include "GameObject.h"
#include "BaseCamera.h"
#include "Skybox.h"

//#include <wrl.h>

//using Microsoft::WRL::ComPtr;



class DX11Framework
{
	int _WindowWidth = 1280;
	int _WindowHeight = 768;

	ID3D11DeviceContext* _immediateContext = nullptr;
	ID3D11Device* _device;
	IDXGIDevice* _dxgiDevice = nullptr;
	IDXGIFactory2* _dxgiFactory = nullptr;
	ID3D11RenderTargetView* _frameBufferView = nullptr;
	IDXGISwapChain1* _swapChain;
	D3D11_VIEWPORT _viewport;

	ID3D11RasterizerState* _fillState;
	ID3D11RasterizerState* _wireframeState;
	ID3D11SamplerState* _bilinearSampleState;
	ID3D11ShaderResourceView* _skyboxTexture;
	ID3D11ShaderResourceView* _carTexture;
	ID3D11ShaderResourceView* _crateTexture;
	ID3D11ShaderResourceView* _crateSpecMap;
	ID3D11ShaderResourceView* _crateNormMap;
	ID3D11VertexShader* _vertexShader;
	ID3D11InputLayout* _inputLayout;
	ID3D11PixelShader* _pixelShader;
	ID3D11Buffer* _constantBuffer;
	ID3D11Buffer* _cubeVertexBuffer;
	ID3D11Buffer* _cubeIndexBuffer;
	ID3D11Buffer* _pyramidVertexBuffer;
	ID3D11Buffer* _pyramidIndexBuffer;
	ID3D11Buffer* _lineVertexBuffer;

	HWND _windowHandle;

	GameObject _car;
	Skybox* _skybox;

	std::vector<BaseCamera*> _cams;
	byte _currentCam = 0;

	XMFLOAT4X4 _World;
	XMFLOAT4X4 _World2;
	XMFLOAT4X4 _World3;
	XMFLOAT4X4 _World4;
	XMFLOAT4X4 _View;
	XMFLOAT4X4 _Projection;

	ConstantBuffer _cbData;

	ID3D11Texture2D* _depthStencilBuffer;
	ID3D11DepthStencilView* _depthStencilView;

	XMFLOAT4 _diffuseLight;
	XMFLOAT4 _diffuseMaterial;
	XMFLOAT3 _lightDir;

	XMFLOAT4 _ambiantLight;
	XMFLOAT4 _ambiantMaterial;

	XMFLOAT4 _specularLight;
	XMFLOAT4 _specularMaterial;
	float _specPower;

	float _speed = 10;

public:
	HRESULT Initialise(HINSTANCE hInstance, int nCmdShow);
	HRESULT CreateWindowHandle(HINSTANCE hInstance, int nCmdShow);
	HRESULT CreateD3DDevice();
	HRESULT CreateSwapChainAndFrameBuffer();
	HRESULT InitShadersAndInputLayout();
	HRESULT InitVertexIndexBuffers();
	HRESULT InitPipelineVariables();
	HRESULT InitRunTimeData();
	~DX11Framework();
	void Update();
	void Draw();
};