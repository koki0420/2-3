#include"boss.h"
#include <ctime>
#include <imgui.h>

Boss::Boss(DirectX::XMFLOAT3 pos)
{
	srand((unsigned int)time(NULL));
	boss.position = pos;
	boss.position.y = -15;
	arrive_position = { 0,-15,0 };
    boss.scale = { 0.1f,0.1f,0.1f };
    boss.model = std::make_unique<Model>("Data/Model/Enemy/last_boss.mdl");

	alive = true;
}

Boss::~Boss()
{

}

void Boss::Update(float elapsedTime)
{

	float direction = DirectX::XMVectorGetX
	(DirectX::XMVector3Length
	(DirectX::XMVectorSubtract
	({ 0,0,-1 }, DirectX::XMLoadFloat3(&boss.position))));


	float dx = arrive_position.x - boss.position.x;   // 0 - pos.x
	float dz = arrive_position.z - boss.position.z;   // 0 - pos.z

	// YŽ²‰ñ“]iYawj
	boss.angle.y = atan2f(dx, dz);

	if (alive)boss.UpdateTransform();
}

void Boss::Render(const RenderContext& immediate_context, ModelRenderer* renderer)
{
	if (alive)renderer->Render(immediate_context,boss.transform, boss.model.get(), ShaderId::Lambert);
}