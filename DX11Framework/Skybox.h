#pragma once
#include <d3d11_4.h>
#include <d3dcompiler.h>
#include "Structures.h"
#include "GameObject.h"


using namespace DirectX;

class Skybox
{
private:
	ID3D11ShaderResourceView* _texture;
	ID3D11VertexShader* _vertexShader;
	ID3D11Buffer* _constantBuffer;
	ID3D11PixelShader* _pixelShader;

	ID3D11DepthStencilState* _depthStencilSkybox;
	ID3D11RasterizerState* _rasterizerSkybox;

	MeshData _meshData;
	ID3D11Device* _device;
	HWND* _windowHandle;

public:
	Skybox(ID3D11Device* device, HWND* windHand, ID3D11Buffer* constBuff, ID3D11ShaderResourceView* texture);
	~Skybox();

	HRESULT Init();
	HRESULT InitShader();

	void Draw(ID3D11DeviceContext* immediateContext, ConstantBuffer& cbData, D3D11_MAPPED_SUBRESOURCE& mappedSubresource);
	void Update(float DeltaTime);
};

