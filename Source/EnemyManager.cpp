#include"EnemyManager.h"

void EnemyManager::Update(float elapsedTime)
{
	for (auto& enemy : enemies)
	{
		enemy->Update(elapsedTime);
	}
}

void EnemyManager::Render(const RenderContext& immediate_context, ModelRenderer* renderer)
{
	for (auto& enemy : enemies)
	{
		enemy->Render(immediate_context,renderer);
	}
}
