#pragma once
#include <windows.h>
#include <d3d11_4.h>
#include <d3dcompiler.h>
#include "JSONParser.h"
#include "OBJLoader.h"
#include "Structures.h"

#include "GameObject.h"
#include "MoveableCamera.h"
#include "Skybox.h"
//#include <wrl.h>

//using Microsoft::WRL::ComPtr;



class DX11Framework
{
	int _WindowWidth = 1280;
	int _WindowHeight = 768;

	JSONParser parser;

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
	ID3D11ShaderResourceView* _tenguTexture;
	ID3D11ShaderResourceView* _tenguNormMap;

	ID3D11VertexShader* _vertexShader;
	ID3D11InputLayout* _inputLayout;
	ID3D11PixelShader* _pixelShader;

	ID3D11Buffer* _constantBuffer;
	ID3D11Buffer* _lightBuffer;

	ID3D11Buffer* _cubeVertexBuffer;
	ID3D11Buffer* _cubeIndexBuffer;
	ID3D11Buffer* _pyramidVertexBuffer;
	ID3D11Buffer* _pyramidIndexBuffer;
	ID3D11Buffer* _lineVertexBuffer;
	ID3D11BlendState* _transparency;

	HWND _windowHandle;

	GameObject _car;
	GameObject _tengu;
	Skybox* _skybox;
	std::vector<MoveableCamera*> _cams;
	byte _currentCam = 0;

#pragma region Matracies
	XMFLOAT4X4 _World;
	XMFLOAT4X4 _World2;
	XMFLOAT4X4 _World3;
	XMFLOAT4X4 _World4;
	XMFLOAT4X4 _World5;
	XMFLOAT4X4 _View;
	XMFLOAT4X4 _Projection;
#pragma endregion

#pragma region Buffer Data
	ConstantBuffer _cbData;
	LightBuffer _lbData;
#pragma endregion

	ID3D11Texture2D* _depthStencilBuffer;
	ID3D11DepthStencilView* _depthStencilView;

	

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