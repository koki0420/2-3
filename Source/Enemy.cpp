#include"enemy.h"
#include <ctime>
#include <imgui.h>

Enemy::Enemy(DirectX::XMFLOAT3 pos)
{
	srand((unsigned int)time(NULL));
	enemy.position = pos;

	enemy.scale = { 0.04f,0.04f,0.04f };
	enemy.position.y = -10;
	//enemy.model = std::make_unique<Model>("Data/Model/Enemy/enemy_normal.mdl");
	enemy.model = std::make_unique<Model>("Data/Model/Enemy/enemy_3.mdl");

	alive = true;
}

Enemy::~Enemy()
{

}

void Enemy::Update(float elapsedTime)
{

	float direction = DirectX::XMVectorGetX
	(DirectX::XMVector3Length
	(DirectX::XMVectorSubtract
	({0,0,-1}, DirectX::XMLoadFloat3(&enemy.position))));

	if (direction <= 100)
	{
		arrive_position = { 0,-10,0 };
		
	}
	else
	{
		if (arrive)
		{
			arrive_position.x = enemy.position.x+(float)(rand() % 100-50) * 10;
			arrive_position.y = enemy.position.y;
			arrive_position.z = enemy.position.z+(float)(rand() % 100-50) * 10;
			//
			
			arrive = false;
		}
		else
		{
	
			float arrive_direction = DirectX::XMVectorGetX
			(DirectX::XMVector3Length
			(DirectX::XMVectorSubtract
			(DirectX::XMLoadFloat3(&arrive_position), DirectX::XMLoadFloat3(&enemy.position))));
			if (arrive_direction <= 50)
			{
				arrive = true;
			}

		}
	}

	if (arrive_position.x >= 250)arrive_position.x = 250;
	if (arrive_position.x <= -250)arrive_position.x = -250;
	if (arrive_position.z >= 250)arrive_position.z = 250;
	if (arrive_position.z <= -250)arrive_position.z = -250;

	//エネミーの行動
	DirectX::XMVECTOR diff =
		DirectX::XMVectorSubtract
		(DirectX::XMLoadFloat3(&arrive_position), DirectX::XMLoadFloat3(&enemy.position));

	diff = DirectX::XMVector3Normalize(diff);

	DirectX::XMVECTOR new_pos = DirectX::XMVectorAdd
	(DirectX::XMLoadFloat3(&enemy.position),
		DirectX::XMVectorScale(diff, 0.1f));
	DirectX::XMStoreFloat3(&enemy.position, new_pos);

	float dx = arrive_position.x - enemy.position.x;   // 0 - pos.x
	float dz = arrive_position.z - enemy.position.z;   // 0 - pos.z

	// Y軸回転（Yaw）
	enemy.angle.y = atan2f(dx, dz);
	if (alive)
	{
		enemy.UpdateTransform();

	}

}

void Enemy::Render(const RenderContext& immediate_context, ModelRenderer* renderer)
{
	if(alive)renderer->Render(immediate_context, enemy.transform, enemy.model.get(), ShaderId::Lambert);
}
