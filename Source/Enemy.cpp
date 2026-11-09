#include"enemy.h"
#include <ctime>
#include <imgui.h>

Enemy::Enemy(DirectX::XMFLOAT3 pos)
{
	enemy.position = pos;


	enemy.scale = { 12,12,12 };
	enemy.position.y = -25;
	enemy.model = std::make_unique<Model>("Data/Model/Enemy/enemy_normal.mdl");

	enemy_anime = std::make_unique<Animation>(*enemy.model);
	idle_anime = true;
	walk_anime = true;
	attack_anime = true;
	death_anime = true;

	alive = true;
	timer = 0;
	nextTimer = (float)(rand() % 10 + 1);

	renderer = false;

	Target = { 0.0f,0.0f,0.0f };
}

Enemy::~Enemy()
{

}

void Enemy::Update(float elapsedTime)
{
	if (alive)
	{

		timer += elapsedTime;
		float direction = DirectX::XMVectorGetX
		(DirectX::XMVector3Length
		(DirectX::XMVectorSubtract
		(DirectX::XMLoadFloat3(&Target), DirectX::XMLoadFloat3(&enemy.position))));
		
	
		if(direction<=200)
		{
			renderer = true;
			if (direction <= 100)
			{
				if (direction <= 30)
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
				else
				{
					arrive_position = Target;
					arrive_position.y = -25;
				}

			}
			else
			{
				if (arrive)
				{
					arrive_position.x = enemy.position.x + (float)(rand() % 100 - 50) * 10;
					arrive_position.y = enemy.position.y;
					arrive_position.z = enemy.position.z + (float)(rand() % 100 - 50) * 10;
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
		}
		else
		{
			renderer = false;
			return;
		}

		if (walk_anime)
		{
			enemy_anime->PlayAnimation("Walking", true);
			walk_anime = false;
		}
		if (!(enemy_anime->IsPlaying()))walk_anime = true;


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



		if (arrive_position.x == enemy.position.x &&
			arrive_position.z == enemy.position.z)
		{
			dx = Target.x - enemy.position.x;   // 0 - pos.x
			dz = Target.z - enemy.position.z;   // 0 - pos.z


		}
		// Y軸回転（Yaw）
		//enemy.angle.y = atan2f(dx, dz);
		float targetAngle = atan2f(dx, dz);

		float dif = targetAngle - enemy.angle.y;

		while (dif > DirectX::XM_PI)  dif -= DirectX::XM_2PI;
		while (dif < -DirectX::XM_PI) dif += DirectX::XM_2PI;

		float turnSpeed = 3.0f * elapsedTime; // rad/s

		if (fabsf(dif) < turnSpeed)
		{
			enemy.angle.y = targetAngle;
		}
		else
		{
			enemy.angle.y += (dif > 0.0f) ? turnSpeed : -turnSpeed;
		}

		enemy.UpdateTransform();
		enemy_anime->Update(elapsedTime);
	}
	if (!(death_anime) && !(enemy_anime->IsPlaying("Death")))
	{
		alive = false;
		death_anime = true;
	}
	if (enemy_anime->IsPlaying("Death"))
	{
		enemy.position.y -= 0.05f;
	}
	else
	{
		enemy.position.y = -25;
	}

}

void Enemy::Render(const RenderContext& immediate_context, ModelRenderer* renderer)
{
	if (alive&&this->renderer)renderer->Render(immediate_context, enemy.transform, enemy.model.get(), ShaderId::Lambert);
	//renderer->Render(immediate_context, enemy.transform, enemy.model.get(), ShaderId::Lambert);
}

void Enemy::enemy_set(bool alive)
{
	if (!(alive))
	{
		if (death_anime)
		{
			enemy_anime->PlayAnimation("Death", false);
			death_anime = false;
		}
	}
	else this->alive = true;
}