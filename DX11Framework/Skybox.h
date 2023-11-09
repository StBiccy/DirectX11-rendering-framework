#pragma once
#include <d3d11_4.h>
#include <d3dcompiler.h>
#include "Structures.h"
#include "GameObject.h"


using namespace DirectX;

class Skybox
{
private:
	XMFLOAT4X4 _World;
	ID3D11ShaderResourceView* _texture;
	ID3D11VertexShader* _vertexShader;
	ID3D11Buffer* _constantBuffer;
	ID3D11PixelShader* _pixelShader;

	ID3D11DepthStencilState* _depthStencilSkybox;
	ID3D11RasterizerState* _rasterizerSkybox;

	GameObject _cube;

	ID3D11Device* _device;
	HWND* _windowHandle;

public:
	Skybox(ID3D11Device* device, HWND* windHand, ID3D11Buffer* constBuff);
	~Skybox();

	HRESULT Init();
	HRESULT InitShader();


	void SetTexture(ID3D11ShaderResourceView* texture) { _texture = texture; }
	void SetWorld(XMFLOAT4X4 world) { _World = world; }

	void Draw(ID3D11DeviceContext* immediateContext, ConstantBuffer* cbData);
	void Update(float DeltaTime);
};

