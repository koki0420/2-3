#include"enemy.h"
#include <ctime>
#include <imgui.h>

Enemy::Enemy(DirectX::XMFLOAT3 pos)
{
	enemy.position = pos;

	enemy.scale = { 10,10,10 };
	enemy.position.y = -25;
	enemy.model = std::make_unique<Model>("Data/Model/Enemy/enemy_normal.mdl");

	enemy_anime= std::make_unique<Animation>(*enemy.model);
	idle_anime = true;
	walk_anime = true;
	attack_anime = true;
	alive = true;
	timer = 0;
	nextTimer = (float)(rand() % 10 + 1);
}

Enemy::~Enemy()
{

}

void Enemy::Update(float elapsedTime)
{
	timer += elapsedTime;
	float direction = DirectX::XMVectorGetX
	(DirectX::XMVector3Length
	(DirectX::XMVectorSubtract
	({0,0,-1}, DirectX::XMLoadFloat3(&enemy.position))));

	if (walk_anime)
	{
		enemy_anime->PlayAnimation("Walking", true);
		walk_anime = false;
	}

	if (direction <= 50)
	{
		arrive_position = enemy.position;
		if (attack_anime)
		{
			enemy_anime->PlayAnimation("Punch", false);
			attack_anime = false;
		}
		else
		{
			if (idle_anime)
			{
				enemy_anime->PlayAnimation("Idle", true);
				idle_anime = false;
			}

			timer += elapsedTime;

			if (timer >= nextTimer)
			{
				attack_anime = true;

				timer = 0.0f;
				nextTimer = (float)(rand() % 10 + 1);
			}
			if (!(enemy_anime->IsPlaying()))idle_anime = true;
		}
		
	}
	else if (direction <= 100)
	{
		arrive_position = { 0,-25,0 };
		
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

	

	if (arrive_position.x == enemy.position.x&&
		arrive_position.z == enemy.position.z)
	{
		dx = 0- enemy.position.x;   // 0 - pos.x
		dz = 0- enemy.position.z;   // 0 - pos.z

		
	}
	// Y軸回転（Yaw）
	enemy.angle.y = atan2f(dx, dz);

	if (alive)
	{
		enemy.UpdateTransform();
		enemy_anime->Update(elapsedTime);
	}

}

void Enemy::Render(const RenderContext& immediate_context, ModelRenderer* renderer)
{
	if(alive)renderer->Render(immediate_context, enemy.transform, enemy.model.get(), ShaderId::Lambert);
}
