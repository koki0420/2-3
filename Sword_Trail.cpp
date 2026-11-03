#include "Sword_Trail.h"

Sword_Trail::Sword_Trail()
{
}

Sword_Trail::~Sword_Trail()
{
}

void Sword_Trail::Update(const DirectX::XMFLOAT4X4& weaponTransform)
{
	// 保存していた頂点座標を１フレーム分ずらす
	{
		for (int i = 0; i < 2; ++i)
		{
			for (int j = MAX_POLYGON - 1; j > 0; --j)
			{
				trailPositions[i][j] = trailPositions[i][j - 1];
			}
		}
	}

	// 剣の根本と先端の座標を取得し、最新の頂点座標を保存
	{
		//剣の根本と先端までのオフセット
		DirectX::XMVECTOR RootOffset = DirectX::XMVectorSet(0, 0, 0.3f, 0);
		DirectX::XMVECTOR TipOffset = DirectX::XMVectorSet(0, 0, 2.3f, 0);

		DirectX::XMMATRIX W = DirectX::XMLoadFloat4x4(&handNode.worldTransform);
		DirectX::XMVECTOR V = DirectX::XMVector3Normalize(W.r[0]);
		DirectX::XMVECTOR Root = DirectX::XMVector3TransformCoord(RootOffset, W);
		DirectX::XMVECTOR Tip = DirectX::XMVector3TransformCoord(TipOffset, W);
		DirectX::XMStoreFloat3(&trailPositions[0][0], Root);
		DirectX::XMStoreFloat3(&trailPositions[1][0], Tip);

	}


}


void Sword_Trail::Render(float elapsedTime)
{
}