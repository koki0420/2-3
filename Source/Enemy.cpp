#include"enemy.h"
#include <ctime>
#include <imgui.h>
#include <algorithm>


Enemy::Enemy(DirectX::XMFLOAT3 pos)
{
	enemy.position = pos;

	enemy.scale = { 12,12,12 };
	enemy.position.y = -25;
	enemy.model = std::make_unique<Model>("Data/Model/Enemy/enemy_normal.mdl");


	for (int i = 0;i < 3;i += 1)
	{
		Blood[i].scale = { 0.005f,0.005f,0.005f };
		Blood[i].position = pos;
		Blood[i].model = std::make_unique<Model>("Data/Model/Enemy/Blood.mdl");
		Blood_Get[i] = false;
		Blood_Display[i] = true;
		Blood_timer[i] = 0;
	}

	enemy_anime = std::make_unique<Animation>(*enemy.model);
	idle_anime = true;
	walk_anime = true;
	attack_anime = true;
	death_anime = true;


	alive = true;
	timer = 0;
	nextTimer = (float)(rand() % 10 + 1);
		

	Target = { 0.0f,0.0f,0.0f };
}

Enemy::~Enemy()
{
}

void Enemy::Update(float elapsedTime)
{
	//エネミーが生きていたら
	if (alive)
	{
		//エネミーが表示されていたら
		if (display)
		{
			for (int i = 0;i < 3;i += 1)
			{
				Blood_Get[i] = false;
				Blood[i].position = enemy.position;
				Blood[i].position.y = -5;
				Blood[i].UpdateTransform();
			}
			timer += elapsedTime;
			float direction = DirectX::XMVectorGetX
			(DirectX::XMVector3Length
			(DirectX::XMVectorSubtract
			(DirectX::XMLoadFloat3(&Target), DirectX::XMLoadFloat3(&enemy.position))));

			float dx = arrive_position.x - enemy.position.x;   // 0 - pos.x
			float dz = arrive_position.z - enemy.position.z;   // 0 - pos.z
			if (direction <= 400)
			{
				if (direction <= 100)
				{
					dx = Target.x - enemy.position.x;   // 0 - pos.x
					dz = Target.z - enemy.position.z;   // 0 - pos.z
					if (direction <= 30)
					{
						arrive_position = Target;
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

			if (walk_anime)
			{
				enemy_anime->PlayAnimation("Walking", false);
				walk_anime = false;
			}
			if (!(enemy_anime->IsPlaying()))walk_anime = true;


			if (arrive_position.x >= 250)arrive_position.x = 250;
			if (arrive_position.x <= -250)arrive_position.x = -250;
			if (arrive_position.z >= 250)arrive_position.z = 250;
			if (arrive_position.z <= -250)arrive_position.z = -250;


			//エネミーの行動
			{
				DirectX::XMVECTOR diff =
					DirectX::XMVectorSubtract
					(DirectX::XMLoadFloat3(&arrive_position), DirectX::XMLoadFloat3(&enemy.position));

				diff = DirectX::XMVector3Normalize(diff);


				DirectX::XMVECTOR new_pos = DirectX::XMVectorAdd
				(DirectX::XMLoadFloat3(&enemy.position),
					DirectX::XMVectorScale(diff, 0.1f));
				DirectX::XMStoreFloat3(&enemy.position, new_pos);
				if (knockback)
				{
					DirectX::XMVECTOR new_pos = DirectX::XMVectorAdd
					(DirectX::XMLoadFloat3(&enemy.position),
						DirectX::XMVectorScale(diff, -knockback_length));
					DirectX::XMStoreFloat3(&enemy.position, new_pos);
					knockback_length -= 0.1f;
					if (knockback_length <= 0)knockback = false;
				}
				else
				{
					knockback_length = 1.0f;
				}
			}


			DirectX::XMVECTOR e_pos = DirectX::XMLoadFloat3(&enemy.position);
			DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&Target);


			DirectX::XMVECTOR diff = DirectX::XMVectorSubtract(pos, e_pos);

			float distSq =
				DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(diff));

			constexpr float radius = 15.0f;
			constexpr float radiusSq = radius * radius;

			if (distSq <= radiusSq)
			{
				float direction = sqrtf(distSq);
				// 正規化（方向ベクトル）
				DirectX::XMVECTOR dir = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(pos, e_pos));

				// めり込み量
				float overlap = (radius - direction) * 0.5f;

				// 押し返し
				DirectX::XMVECTOR offset = DirectX::XMVectorScale(dir, overlap);

				e_pos = DirectX::XMVectorSubtract(e_pos, offset);

				// 書き戻し
				DirectX::XMFLOAT3 newPos1;
				DirectX::XMStoreFloat3(&newPos1, e_pos);

				enemy.position = newPos1;

			}


			// Y軸回転（Yaw）
			{
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
			}
			enemy.UpdateTransform();
			enemy_anime->Update(elapsedTime);
		}
	}
	//ドロップ血液の処理
	else
	{
		if (Blood[0].position.y > -15)
		{
			Blood[0].position.x += 0.3f;
			Blood[0].position.y -= 0.5f;
		}
		if (Blood[1].position.y > -15)
		{
			Blood[1].position.x -= 0.3f;
			Blood[1].position.y -= 0.5f;
		}
		if (Blood[2].position.y > -15)
		{
			Blood[2].position.x -= 0.3f;
			Blood[2].position.z -= 0.3f;
			Blood[2].position.y -= 0.5f;
		}
		
		
		for (int i = 0;i < 3;i += 1)
		{
			Blood[i].UpdateTransform();
			if (!(Blood_Get[i])&&Blood_Display[i])
			{
				DirectX::XMVECTOR Blood_Target = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&Target), DirectX::XMLoadFloat3(&Blood[i].position));
				if (Blood[i].position.y <= -15)
				{
					float t = std::clamp(Blood_timer[i] / 1.0f, 0.0f, 1.0f);

					DirectX::XMVECTOR Blood_Target_dir = DirectX::XMVector3Normalize(Blood_Target);	
					DirectX::XMStoreFloat3(&Blood[i].position,DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&Blood[i].position), DirectX::XMVectorScale(Blood_Target_dir, t* t* t* t)));
					Blood_timer[i] += elapsedTime;

				}
				if (DirectX::XMVectorGetX(DirectX::XMVector3Length(Blood_Target)) <= 1.0f)
				{
					Blood_Display[i] = false;
					Blood_Get[i] = true;
					Blood_timer[i] = 0;
				}
			}
			
		}
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
	if (alive)
	{
		if (display)renderer->Render(immediate_context, enemy.transform, enemy.model.get(), ShaderId::Lambert);
	}
	else
	{
		for (int i = 0;i < 3;i += 1)
		{
			if(Blood_Display[i])renderer->Render(immediate_context, Blood[i].transform, Blood[i].model.get(), ShaderId::Lambert);
		}
	}
	//renderer->Render(immediate_context, enemy.transform, enemy.model.get(), ShaderId::Lambert);
}

	void Enemy::enemy_set(bool alive)
	{
		if (!(alive))
		{
			if (death_anime)
			{
				for (int i = 0;i < 3;i += 1)
				{
					Blood[i].position = enemy.position;
					Blood[i].position.y = -15;
					Blood_Display[i] = true;
					Blood_Get[i] = false;
					Blood_timer[i] = 0;
				}
				enemy_anime->PlayAnimation("Death", false);
				death_anime = false;
			}
		}
		else this->alive = true;
	}