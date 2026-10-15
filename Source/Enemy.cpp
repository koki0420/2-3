#include"enemy.h"

Enemy::Enemy(DirectX::XMFLOAT3 pos)
{
	enemy.position = pos;
	enemy.scale = { 0.1f,0.1f,0.1f };
	enemy.position.y = -10;
	enemy.model = std::make_unique<Model>("Data/Model/Mr.Incredible/Mr.Incredible.mdl");
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
		//エネミーの行動
		DirectX::XMVECTOR diff =
			DirectX::XMVectorSubtract({ 0,0,-1 }, DirectX::XMLoadFloat3(&enemy.position));

		diff = DirectX::XMVector3Normalize(diff);

		DirectX::XMVECTOR new_pos = DirectX::XMVectorAdd
		(DirectX::XMLoadFloat3(&enemy.position),
			DirectX::XMVectorScale(diff, 0.1f));
		DirectX::XMStoreFloat3(&enemy.position, new_pos);
		enemy.position.y = -10;

		
	}
	float dx = -enemy.position.x;   // 0 - pos.x
	float dz = -enemy.position.z;   // 0 - pos.z

	// Y軸回転（Yaw）
	enemy.angle.y = atan2f(dx, dz);
	if(alive)enemy.UpdateTransform();
}

void Enemy::Render(const RenderContext& immediate_context, ModelRenderer* renderer)
{
	if(alive)renderer->Render(immediate_context, enemy.transform, enemy.model.get(), ShaderId::Lambert);
}
