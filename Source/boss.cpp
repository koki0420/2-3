#include"boss.h"
#include <ctime>
#include <imgui.h>

Boss::Boss(DirectX::XMFLOAT3 pos)
{
	arrive_position = { 0,-15,0 };

	boss.position = pos;
	boss.position.y = -15;
    boss.scale = { 0.1f,0.1f,0.1f };
    boss.model = std::make_unique<Model>("Data/Model/Enemy/low_last_boss.mdl");

	middle_boss.position = pos;
	middle_boss.position.y = -15;
	middle_boss.scale = { 0.5f,0.5f,0.5f };
	middle_boss.model = std::make_unique<Model>("Data/Model/Enemy/enemy_midle.mdl");

	boss_count = 0;

	alive = false;
}

Boss::~Boss()
{

}

void Boss::Update(float elapsedTime)
{

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
	}

	{
		float direction = DirectX::XMVectorGetX
		(DirectX::XMVector3Length
		(DirectX::XMVectorSubtract
		({ 0,0,-1 }, DirectX::XMLoadFloat3(&middle_boss.position))));


		float dx = arrive_position.x - middle_boss.position.x;   // 0 - pos.x
		float dz = arrive_position.z - middle_boss.position.z;   // 0 - pos.z

		// Y軸回転（Yaw）
		middle_boss.angle.y = atan2f(dx, dz);
	}

	if (boss_count >= 3)boss_count = 0;
	if (alive)
	{
		if (boss_count < 2)middle_boss.UpdateTransform();
		else boss.UpdateTransform();
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