#pragma once
#include <DirectXMath.h>
#include "RenderContext.h"
#include <vector>
#include <algorithm>

class Sword_Line
{
public:
	struct TrailVertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 texcoord;
		DirectX::XMFLOAT4 color;
	};

	struct CbTrail
	{
		DirectX::XMFLOAT4X4 viewProjection;
	};


	//コンストラクタ
	Sword_Line();

	// 更新処理
	void Update(const DirectX::XMFLOAT4X4& weaponTransform);

	// 描画処理
	void Render(ID3D11DeviceContext* dc, const DirectX::XMFLOAT4X4& viewProjection);

	DirectX::XMFLOAT3 CatmullRom(
		const DirectX::XMFLOAT3& p0, const DirectX::XMFLOAT3&
		p1, const DirectX::XMFLOAT3& p2, const DirectX::XMFLOAT3& p3, float t);

private:
	bool  spline = false;

	static constexpr int MAX_POLYGON = 32;

	DirectX::XMFLOAT3 rootHistory[MAX_POLYGON];
	DirectX::XMFLOAT3 tipHistory[MAX_POLYGON];

	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;


	std::vector<TrailVertex> vertices;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;

	//加算ブレンド用
	Microsoft::WRL::ComPtr<ID3D11BlendState> additiveBlendState;

	//テクスチャ用
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> LineTexture;

	int historyCount = 0;
	
};

