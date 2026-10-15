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
	//エネミーの行動
	enemy.position.z += (float)(rand()%10)/10;
	if (enemy.position.z >= 260)enemy.position.z = -260;

	if(alive)enemy.UpdateTransform();
}

void Enemy::Render(const RenderContext& immediate_context, ModelRenderer* renderer)
{
	if(alive)renderer->Render(immediate_context, enemy.transform, enemy.model.get(), ShaderId::Lambert);
}
