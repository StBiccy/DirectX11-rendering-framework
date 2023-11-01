#pragma once
#include <d3d11_1.h>
#include "OBJLoader.h"
class GameObject
{
private:
	ID3D11ShaderResourceView* _texture;
	ID3D11ShaderResourceView* _specMap;
	ID3D11ShaderResourceView* _normMap;
	bool _hasTexture = false;
	bool _hasSpecMap = false;
	bool _hasNormMap = false;
	MeshData _meshData;
	DirectX::XMFLOAT4X4 _world;

public:
	GameObject();

	void SetTexture(ID3D11ShaderResourceView* in) { _texture = in; _hasTexture = true; }
	void SetSpecularMap(ID3D11ShaderResourceView* in) { _texture = in; _hasSpecMap = true; }
	void SetNormalMap(ID3D11ShaderResourceView* in) { _texture = in; _hasNormMap = true; }
	void SetMeshData(MeshData in) { _meshData = in; }

	ID3D11ShaderResourceView** GetShaderResource() { return &_texture; }
	MeshData* GetMeshData() { return &_meshData; }
	void Draw(ID3D11DeviceContext* immediateContext, ConstantBuffer* cbData)
	{
		cbData->hasTex = _hasTexture;
		cbData->hasSpecMap = _hasSpecMap;
		cbData->hasNormMap = _hasNormMap;
		immediateContext->PSSetShaderResources(0, 1, &_texture);
		immediateContext->PSSetShaderResources(1, 1, &_specMap);
		immediateContext->PSSetShaderResources(2, 1, &_normMap);

		immediateContext->IASetVertexBuffers(0, 1, &_meshData.VertexBuffer, &_meshData.VBStride, &_meshData.VBOffset);
		immediateContext->IASetIndexBuffer(_meshData.IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	}
};

