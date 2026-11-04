#include "Sword_Line.h"
#include <d3d11.h>
#include "Graphics.h"
#include "texture.h"



Sword_Line::Sword_Line()
{

	ID3D11Device* device =
		Graphics::Instance().GetDevice();

	D3D11_BUFFER_DESC bd{};

	bd.ByteWidth = sizeof(TrailVertex) * MAX_POLYGON * 2;
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	device->CreateBuffer(&bd, nullptr, vertexBuffer.GetAddressOf());

	D3D11_BUFFER_DESC cbDesc{};

	cbDesc.ByteWidth = sizeof(CbTrail);
	cbDesc.Usage =	D3D11_USAGE_DEFAULT;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	device->CreateBuffer(&cbDesc,nullptr,constantBuffer.GetAddressOf());

	//加算ブレンド用
	D3D11_BLEND_DESC blendDesc{};

	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend =D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlend =D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOp =D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha =D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha =D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha =D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask =D3D11_COLOR_WRITE_ENABLE_ALL;

	device->CreateBlendState(&blendDesc,additiveBlendState.GetAddressOf());


	D3D11_INPUT_ELEMENT_DESC desc[]
	{
		{
		  "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0
		},

		{
		  "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0
		},

		{
		  "COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0
		}
	};

	D3D11_TEXTURE2D_DESC textureDesc{};
	create_vs_from_cso(
		device,
		"Data/Shader/Sword_Line_vs.cso",
		vertexShader.GetAddressOf(),
		inputLayout.GetAddressOf(),
		desc,
		_countof(desc));

	create_ps_from_cso(
		device,
		"Data/Shader/Sword_Line_ps.cso",
		pixelShader.GetAddressOf());

	load_texture_from_file(
		device,
		L"Data/Sprite/Sword_Line/Line.png",
		LineTexture.GetAddressOf(),
		&textureDesc);

}

DirectX::XMFLOAT3 Sword_Line::CatmullRom(const DirectX::XMFLOAT3& p0, const DirectX::XMFLOAT3& p1, const DirectX::XMFLOAT3& p2, const DirectX::XMFLOAT3& p3, float t)
{
	float t2 = t * t;
	float t3 = t2 * t;

	DirectX::XMFLOAT3 result;

	result.x =
		0.5f *
		(
			(2.0f * p1.x) +
			(-p0.x + p2.x) * t +
			(2.0f * p0.x - 5.0f * p1.x + 4.0f * p2.x - p3.x) * t2 +
			(-p0.x + 3.0f * p1.x - 3.0f * p2.x + p3.x) * t3
			);

	result.y =
		0.5f *
		(
			(2.0f * p1.y) +
			(-p0.y + p2.y) * t +
			(2.0f * p0.y - 5.0f * p1.y + 4.0f * p2.y - p3.y) * t2 +
			(-p0.y + 3.0f * p1.y - 3.0f * p2.y + p3.y) * t3
			);

	result.z =
		0.5f *
		(
			(2.0f * p1.z) +
			(-p0.z + p2.z) * t +
			(2.0f * p0.z - 5.0f * p1.z + 4.0f * p2.z - p3.z) * t2 +
			(-p0.z + 3.0f * p1.z - 3.0f * p2.z + p3.z) * t3
			);

	return result;
}



void Sword_Line::Update(const DirectX::XMFLOAT4X4& weaponTransform)
{

	// 保存していた頂点座標を１フレーム分ずらす
	{
		for (int j = MAX_POLYGON - 1; j > 0; --j)
		{
			rootHistory[j] = rootHistory[j - 1];
			tipHistory[j] = tipHistory[j - 1];
		}
	}

	//剣ローカル座標
	DirectX::XMVECTOR rootLocal = DirectX::XMVectorSet(0, 0, 0, 1);
	DirectX::XMVECTOR tipLocal = DirectX::XMVectorSet(5, 0, 60.0f, 1);

	//武器行列
	DirectX::XMMATRIX W = DirectX::XMLoadFloat4x4(&weaponTransform);

	//ワールド変換
	DirectX::XMVECTOR rootWorld = DirectX::XMVector3TransformCoord(rootLocal, W);

	DirectX::XMVECTOR tipWorld = DirectX::XMVector3TransformCoord(tipLocal, W);

	DirectX::XMStoreFloat3(&rootHistory[0], rootWorld);
	DirectX::XMStoreFloat3(&tipHistory[0], tipWorld);

	historyCount++;

	historyCount = (std::min)(historyCount, MAX_POLYGON);


	vertices.clear();

	if (historyCount < 4)
	{
		return;
	}


	for (int i = 0; i < historyCount; i++)
	{
		float t =
			static_cast<float>(i) /
			static_cast<float>(
				historyCount - 1);

		float alpha =
			1.0f - t;

		vertices.push_back(
			{
				rootHistory[i],
				{0.0f, t},
				{0.6f,0.05f,0.05f,alpha}
			});

		float brightness =
			alpha * alpha;
		vertices.push_back(
			{
				tipHistory[i],
				{1.0f, t},
				{
					1.0f * brightness,
					0.05f * brightness,
					0.05f * brightness,alpha
				}
			});
	}

	

}


void Sword_Line::Render(ID3D11DeviceContext* dc,const DirectX::XMFLOAT4X4& viewProjection)
{
	
	if (vertices.empty())return;

	CbTrail cb{};

	cb.viewProjection =viewProjection;

	dc->UpdateSubresource(constantBuffer.Get(),0,nullptr,&cb,0,0);

	ID3D11Buffer* cbs[]
	{
		constantBuffer.Get()
	};

	dc->VSSetConstantBuffers(
		0,
		1,
		cbs);

	D3D11_MAPPED_SUBRESOURCE mapped{};

	HRESULT hr =
		dc->Map(
			vertexBuffer.Get(),
			0,
			D3D11_MAP_WRITE_DISCARD,
			0,
			&mapped);

	if (FAILED(hr))return;

	memcpy(mapped.pData,vertices.data(),
		sizeof(TrailVertex) *vertices.size());

	dc->Unmap(vertexBuffer.Get(),0);

	UINT stride =
		sizeof(TrailVertex);

	UINT offset = 0;

	dc->IASetVertexBuffers(
		0,
		1,
		vertexBuffer.GetAddressOf(),
		&stride,
		&offset);

	dc->IASetPrimitiveTopology(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	dc->IASetInputLayout(inputLayout.Get());
	dc->VSSetShader(vertexShader.Get(),nullptr,0);
	dc->PSSetShader(pixelShader.Get(),nullptr,0);

	float blendFactor[4] =
	{
		1,1,1,1
	};

	ID3D11SamplerState* sampler =Graphics::Instance().GetSamplerState();

	dc->PSSetSamplers(0,1,&sampler);
	dc->PSSetShaderResources(0, 1, LineTexture.GetAddressOf());
	dc->OMSetBlendState(additiveBlendState.Get(),blendFactor,0xffffffff);

	dc->Draw(static_cast<UINT>(vertices.size()),0);

	dc->OMSetBlendState(nullptr,blendFactor,0xFFFFFFFF);
}

