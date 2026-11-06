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
		if (!(enemies[i]->Alive()))continue;
		for (int j = i + 1 ; j < enemies.size(); j += 1)
		{
			if (!(enemies[j]->Alive()))continue;

			XMVECTOR e_pos1 = DirectX::XMLoadFloat3(&enemies[i]->GetPos());
			XMVECTOR e_pos2 = DirectX::XMLoadFloat3(&enemies[j]->GetPos());


			XMVECTOR diff = XMVectorSubtract(e_pos2, e_pos1);

			float distSq =
				XMVectorGetX(XMVector3LengthSq(diff));

			constexpr float radius = 10.0f;
			constexpr float radiusSq = radius * radius;

			if (distSq <= radiusSq)
			{
				float direction = sqrtf(distSq);
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

void EnemyManager::Enemy_Set(int i, bool Alive)
{
	enemies[i]->enemy_set(Alive);
	if(Alive)
	{
		//25
		enemy_respawn_w = i % 10;
		//4
		enemy_respawn_h = i / 10;
	

		enemies[i]->SetPos({
			-260 + (520/9.0f) * enemy_respawn_w,
			-25,
			-260 + (520/9.0f) * enemy_respawn_h
			});

		
	}
}