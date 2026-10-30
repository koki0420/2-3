#include"boss.h"
#include <ctime>
#include <imgui.h>

Boss::Boss(DirectX::XMFLOAT3 pos)
{
	arrive_position = { 0,-15,0 };

	boss.position = pos;
	boss.position.y = -15;
    boss.scale = { 0.4f,0.4f,0.4f };
    boss.model = std::make_unique<Model>("Data/Model/Enemy/enemy_boss.mdl");

	weapon.position = { 0,15,0 };
	weapon.scale = { 0.5f,0.5f,0.5f };
	weapon.model = std::make_unique<Model>("Data/Model/Weapon/Sword.mdl");

	middle_boss.position = pos;
	middle_boss.position.y = -15;
	middle_boss.scale = { 0.5f,0.5f,0.5f };
	middle_boss.model = std::make_unique<Model>("Data/Model/Enemy/enemy_midle.mdl");

	boss_count = 2;

	boss_anime= std::make_unique<Animation>(*boss.model);
	middle_boss_anime = std::make_unique<Animation>(*middle_boss.model);

	alive = false;

	//-3, 43,0.495

	midle_effect = EffectManager::Instance().CreateEffect("Data/Effect/midle_enemy_smash.efkefc");
	boss_jump_effect = EffectManager::Instance().CreateEffect("Data/Effect/enemy_lastboss_sowrd.efkefc");

	jump_effect_play = true;
	boss_jump = true;

}

Boss::~Boss()
{

}

void Boss::Update(float elapsedTime)
{
	timer += elapsedTime;
	
	
	if (dead)
	{
		if (!dyingStarted)
		{
			middle_boss_anime->PlayAnimation("Death", false);
			boss_anime->PlayAnimation("Death", false);
			dyingStarted = true;
		}
		if (!boss_anime->IsPlaying() && boss_count >= 2)
		{
			alive = false;
			dyingStarted = false;
		}

		if (!middle_boss_anime->IsPlaying()&&boss_count<2)
		{
			alive = false;
			dyingStarted = false;
		}

		
	}
	else
	{
		//中ボス
		{
			float direction = DirectX::XMVectorGetX
			(DirectX::XMVector3Length
			(DirectX::XMVectorSubtract
			({ 0,0,-1 }, DirectX::XMLoadFloat3(&middle_boss.position))));


			float dx = arrive_position.x - middle_boss.position.x;   // 0 - pos.x
			float dz = arrive_position.z - middle_boss.position.z;   // 0 - pos.z

			// Y軸回転（Yaw）
			middle_boss.angle.y = atan2f(dx, dz);

			if (!(middle_boss_anime->IsPlaying()))
			{
				int i = rand() % 2;
				if (i == 0)middle_boss_anime->PlayAnimation("Roaring", false);
				if (i == 1)
				{
					middle_boss_anime->PlayAnimation("Jump", false);
					jump_effect_play = false;
				}

			}
		}

		//ボス
		{
			float direction = DirectX::XMVectorGetX
			(DirectX::XMVector3Length
			(DirectX::XMVectorSubtract
			({ 0,0,-1 }, DirectX::XMLoadFloat3(&boss.position))));


			float dx = arrive_position.x - boss.position.x;   // 0 - pos.x
			float dz = arrive_position.z - boss.position.z;   // 0 - pos.z

			// Y軸回転（Yaw）
			boss.angle.y = atan2f(dx, dz);


			if (attack_anime)
			{
				
				int i = rand() % 2;
				if (i == 0)boss_anime->PlayAnimation("Attack", false);
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

	
	if (boss_count >= 3)clear=true;
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