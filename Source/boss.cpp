#include"boss.h"
#include <ctime>
#include <imgui.h>

#define Y 25

Boss::Boss(DirectX::XMFLOAT3 pos)
{
	arrive_position = { 0,-15,0 };

	boss.position = pos;
	boss.position.y = -Y;
    boss.scale = { 0.4f,0.4f,0.4f };
    boss.model = std::make_unique<Model>("Data/Model/Enemy/enemy_boss.mdl");

	weapon.position = { 0,15,0 };
	weapon.scale = { 0.5f,0.5f,0.5f };
	weapon.model = std::make_unique<Model>("Data/Model/Weapon/Sword.mdl");

	middle_boss.position = pos;
	middle_boss.position.y = -15;
	middle_boss.scale = { 0.5f,0.5f,0.5f };
	middle_boss.model = std::make_unique<Model>("Data/Model/Enemy/enemy_midle.mdl");

	boss_count = 0;

	boss_anime= std::make_unique<Animation>(*boss.model);
	middle_boss_anime = std::make_unique<Animation>(*middle_boss.model);

	alive = false;

	//-3, 43,0.495

	midle_effect = EffectManager::Instance().CreateEffect("Data/Effect/midle_enemy_smash.efkefc");
	boss_jump_effect = EffectManager::Instance().CreateEffect("Data/Effect/enemy_lastboss_sowrd.efkefc");
	boss_slash_effect = EffectManager::Instance().CreateEffect("Data/Effect/enemy_lastboss_slash.efkefc");

	jump_effect_play = true;
	boss_jump = true;
	boss_slash = true;
}

Boss::~Boss()
{

}

void Boss::Update(float elapsedTime)
{
	timer += elapsedTime;

	timer += elapsedTime;
	float middle_boss_direction = DirectX::XMVectorGetX
	(DirectX::XMVector3Length
	(DirectX::XMVectorSubtract
	(DirectX::XMLoadFloat3(&Target), DirectX::XMLoadFloat3(&middle_boss.position))));

	float boss_direction = DirectX::XMVectorGetX
	(DirectX::XMVector3Length
	(DirectX::XMVectorSubtract
	(DirectX::XMLoadFloat3(&Target), DirectX::XMLoadFloat3(&boss.position))));


	{
		float dx = Target.x - middle_boss.position.x;   // 0 - pos.x
		float dz = Target.z - middle_boss.position.z;


		// Y軸回転（Yaw）
		{
			//enemy.angle.y = atan2f(dx, dz);
			float targetAngle = atan2f(dx, dz);

			float dif = targetAngle - middle_boss.angle.y;

			while (dif > DirectX::XM_PI)  dif -= DirectX::XM_2PI;
			while (dif < -DirectX::XM_PI) dif += DirectX::XM_2PI;

			float turnSpeed = 3.0f * elapsedTime; // rad/s

			if (fabsf(dif) < turnSpeed)
			{
				middle_boss.angle.y = targetAngle;
			}
			else
			{
				middle_boss.angle.y += (dif > 0.0f) ? turnSpeed : -turnSpeed;
			}
		}
	}

	{
		float dx = Target.x - boss.position.x;   // 0 - pos.x
		float dz = Target.z - boss.position.z;


		// Y軸回転（Yaw）
		{
			//enemy.angle.y = atan2f(dx, dz);
			float targetAngle = atan2f(dx, dz); 

			float dif = targetAngle - boss.angle.y;

			while (dif > DirectX::XM_PI)  dif -= DirectX::XM_2PI;
			while (dif < -DirectX::XM_PI) dif += DirectX::XM_2PI;

			float turnSpeed = 3.0f * elapsedTime; // rad/s

			if (fabsf(dif) < turnSpeed)
			{
				boss.angle.y = targetAngle;
			}
			else
			{
				boss.angle.y += (dif > 0.0f) ? turnSpeed : -turnSpeed;
			}
		}
	}

	if(!(dead))
	{
		dyingStarted = false;
		if (middle_boss_direction <= 100)
		{
			bool anime = true;
			if (middle_boss_anime->IsPlaying("Jump"))anime = false;
			if (anime)
			{
				//中ボス
				middle_boss_anime->PlayAnimation("Jump", false);
				jump_effect_play = false;
			}
		}
		else
		{
			if (!(middle_boss_anime->IsPlaying()))
			{
				middle_boss_anime->PlayAnimation("Roaring", false);
				attack_anime = true;
				idle_anime = true;;
			}
		}

		if (boss_direction<=100)
		{//ボス
			{

				if (attack_anime)
				{

					int i = rand() % 2;
					if (i == 0)
					{
						boss_anime->PlayAnimation("Attack", false);
						boss_slash = false;
					}


					if (i == 1)
					{
						boss_anime->PlayAnimation("Jump_Attack", false);
						boss_jump = false;
					}

					attack_anime = false;
				}
				else
				{
					if (idle_anime)
					{
						boss_anime->PlayAnimation("Idle", true);
						idle_anime = false;
					}

					if ((int)timer % (5 * (rand() % 6 + 1)) == 0 && !(boss_anime->IsPlaying("Attack")) && !(boss_anime->IsPlaying("Jump_Attack")))
					{
						attack_anime = true;
					}
					if (!(boss_anime->IsPlaying()))idle_anime = true;
				}

			}
		}
		else 
		{
			if (walk_anime)
			{
				boss_anime->PlayAnimation("Walk", false);
				walk_anime = false;
			}
			if (!(boss_anime->IsPlaying()))walk_anime = true;
			if (boss_anime->IsPlaying("Walk"))
			{
				DirectX::XMVECTOR dis = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&Target), DirectX::XMLoadFloat3(&boss.position)));
				DirectX::XMVECTOR new_pos = DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&boss.position), DirectX::XMVectorScale(dis, 0.5f));
				DirectX::XMStoreFloat3(&boss.position, new_pos);
			}
			
		}
	}
	else
	{
		if (!(middle_boss_anime->IsPlaying("Death"))|| !(boss_anime->IsPlaying("Death")))
		{
			if (dyingStarted)
			{
				dead = false;
				if (middle_boss_anime->GetCurrentFrame() >= 250)
				{
					alive = false;
				}
				if (boss_anime->GetCurrentFrame() >= 100)
				{
					alive = false;
				}
			}
		}

		if (boss_count >= 2)
		{
		
			if (!(dyingStarted))
			{
				boss_anime->PlayAnimation("Death", false);
				dyingStarted = true;
			}
		}
		else
		{
			if (!(dyingStarted))
			{
				middle_boss_anime->PlayAnimation("Death", false);
				dyingStarted = true;
			}
		}
	}


	

	
	if (boss_count >= 2&& dead)clear=true;
	if (alive)
	{
		
		if (boss_count < 2)
		{
			middle_boss.UpdateTransform();
			middle_boss_anime->Update(elapsedTime);
		}
		else
		{
			boss.UpdateTransform();
			boss_anime->Update(elapsedTime);

			const char* rightHandName = "RightHand";
			DirectX::XMMATRIX S = DirectX::XMMatrixScaling(weapon.scale.x, weapon.scale.y, weapon.scale.z);
			DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(weapon.angle.x, weapon.angle.y, weapon.angle.z);
			DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(weapon.position.x, weapon.position.y, weapon.position.z);
			DirectX::XMStoreFloat4x4(&weapon.transform, S* R* T);

			for (const Model::Node& node : boss.model->GetNodes())
			{
				if (strcmp(node.name, rightHandName) == 0)
				{
					DirectX::XMMATRIX w_trans = DirectX::XMLoadFloat4x4(&weapon.transform);
					DirectX::XMMATRIX g_trans = DirectX::XMLoadFloat4x4(&node.globalTransform);
					DirectX::XMMATRIX p_trans = DirectX::XMLoadFloat4x4(&boss.transform);
					w_trans = (w_trans * g_trans) * p_trans;
					DirectX::XMStoreFloat4x4(&weapon.transform, w_trans);


					break;
				}
			}
		}
	}

	//エフェクト
	

	if (middle_boss_anime->GetCurrentFrame()>=70 && !jump_effect_play)
	{
		
		midle_effect->Play(middle_boss.position, 7.0f);
		jump_effect_play = true;
	}

	if (boss_anime->GetCurrentFrame() >= 50 && !boss_jump)
	{
		boss_jump_effect->Play({boss.position.x,boss.position.y,boss.position.z-60.0f}, 6.0f);
		boss_jump = true;
	}
	
	if (boss_anime->GetCurrentFrame() >= 20 && !boss_slash)
	{
		boss_slash_effect->Play({ boss.position.x,boss.position.y + 30,boss.position.z-30 }, 15.0f);
		boss_slash = true;
	}

}

void Boss::Render(const RenderContext& immediate_context, ModelRenderer* renderer)
{
	if (alive)
	{
		if (boss_count < 2)renderer->Render(immediate_context, middle_boss.transform, middle_boss.model.get(), ShaderId::Lambert);
		else
		{
			renderer->Render(immediate_context, boss.transform, boss.model.get(), ShaderId::Lambert);
			renderer->Render(immediate_context, weapon.transform, weapon.model.get(), ShaderId::Lambert);
		}
	}
}