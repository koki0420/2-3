#include "Sword_Line.h"



void Sword_Line::Update(const DirectX::XMFLOAT4X4& weaponTransform)
{

	// 保存していた頂点座標を１フレーム分ずらす
	{
		for (int i = 0; i < 2; ++i)
		{
			for (int j = MAX_POLYGON - 1; j > 0; --j)
			{
				if (i == 0)
				{
					rootHistory[j] = rootHistory[j - 1];
				}
				else
				{
					tipHistory[j] = tipHistory[j - 1];
				}
			}
		}
	}

	//剣ローカル座標
	DirectX::XMVECTOR rootLocal = DirectX::XMVectorSet(0, 0, 0, 1);
	DirectX::XMVECTOR tipLocal = DirectX::XMVectorSet(0, 0, 2.0f, 1);

	//武器行列
	DirectX::XMMATRIX W = DirectX::XMLoadFloat4x4(&weaponTransform);

	//ワールド変換
	DirectX::XMVECTOR rootWorld = DirectX::XMVector3TransformCoord(rootLocal, W);

	DirectX::XMVECTOR tipWorld = DirectX::XMVector3TransformCoord(tipLocal, W);

	DirectX::XMStoreFloat3(&rootHistory[0], rootWorld);
	DirectX::XMStoreFloat3(&tipHistory[0], tipWorld);

	
}


void Sword_Line::Render(const RenderContext& rc)
{
}