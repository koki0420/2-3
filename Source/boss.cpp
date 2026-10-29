#include"boss.h"
#include <ctime>
#include <imgui.h>

Boss::Boss(DirectX::XMFLOAT3 pos)
{
	arrive_position = { 0,-15,0 };

	boss.position = pos;
	boss.position.y = -15;
    boss.scale = { 0.1f,0.1f,0.1f };
    boss.model = std::make_unique<Model>("Data/Model/Enemy/enemy_boss.mdl");

	middle_boss.position = pos;
	middle_boss.position.y = -15;
	middle_boss.scale = { 0.5f,0.5f,0.5f };
	middle_boss.model = std::make_unique<Model>("Data/Model/Enemy/enemy_midle.mdl");

	boss_count = 0;

	boss_anime= std::make_unique<Animation>(*boss.model);
	middle_boss_anime = std::make_unique<Animation>(*middle_boss.model);

	alive = false;

	midle_effect = EffectManager::Instance().CreateEffect("Data/Effect/midle_enemy_smash.efkefc");

	jump_effect_play = true;

}

Boss::~Boss()
{

}

void Boss::Update(float elapsedTime)
{

	
	if (dead)
	{
		if (!dyingStarted)
		{
			middle_boss_anime->PlayAnimation("Dying", false);
			//boss_anime->PlayAnimation("Dying", false);
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
			if (!(boss_anime->IsPlaying()))
			{
				//boss_anime->PlayAnimation("Roaring", true);
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
		}
	}

	//エフェクト
	

	if (middle_boss_anime->GetCurrentFrame()>=70 && !jump_effect_play)
	{
		
		midle_effect->Play(middle_boss.position, 7.0f);
		jump_effect_play = true;
	}

	
}

void Boss::Render(const RenderContext& immediate_context, ModelRenderer* renderer)
{
	if (alive)
	{
		if (boss_count < 2)renderer->Render(immediate_context, middle_boss.transform, middle_boss.model.get(), ShaderId::Lambert);
		else renderer->Render(immediate_context, boss.transform, boss.model.get(), ShaderId::Lambert);
	}
}