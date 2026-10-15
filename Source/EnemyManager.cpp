#include"EnemyManager.h"
using namespace DirectX;

void EnemyManager::Update(float elapsedTime)
{
	for (auto& enemy : enemies)
	{
		enemy->Update(elapsedTime);
	}

	for (int i = 0;i < enemies.size();i += 1)
	{
		for (int j = i + 1 ; j < enemies.size(); j += 1)
		{
			XMVECTOR e_pos1 = DirectX::XMLoadFloat3(&enemies[i]->GetPos());
			XMVECTOR e_pos2 = DirectX::XMLoadFloat3(&enemies[j]->GetPos());

			float direction =
				XMVectorGetX(XMVector3Length
				(XMVectorSubtract(e_pos2, e_pos1)));

			float radius = 10;
			if (direction <= radius)
			{
				// 正規化（方向ベクトル）
				DirectX::XMVECTOR dir = DirectX::XMVector3Normalize(XMVectorSubtract(e_pos2, e_pos1));

				// めり込み量
				float overlap = (radius - direction) * 0.5f;

				// 押し返し
				DirectX::XMVECTOR offset = DirectX::XMVectorScale(dir, overlap);

				e_pos1 = DirectX::XMVectorSubtract(e_pos1, offset);
				e_pos2 = DirectX::XMVectorAdd(e_pos2, offset);

				// 書き戻し
				DirectX::XMFLOAT3 newPos1, newPos2;
				DirectX::XMStoreFloat3(&newPos1, e_pos1);
				DirectX::XMStoreFloat3(&newPos2, e_pos2);

				enemies[i]->SetPos(newPos1);
				enemies[j]->SetPos(newPos2);
			}

		}
	}
}

void EnemyManager::Render(const RenderContext& immediate_context, ModelRenderer* renderer)
{
	for (auto& enemy : enemies)
	{
		enemy->Render(immediate_context,renderer);
	}
}
