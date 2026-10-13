#pragma once

#include<vector>
#include"Enemy.h"

class EnemyManager
{
private:
	EnemyManager() {};
	~EnemyManager() {};

public:
	static EnemyManager& Instance()
	{
		static EnemyManager instance;
		return instance;
	}

	void Update(float elapsedTime);

	void Render(const RenderContext& immediate_context, ModelRenderer* renderer);

	void Register(std::unique_ptr<Enemy> enemy)
	{
		enemies.emplace_back(std::move(enemy));
	}

private:
	std::vector<std::unique_ptr<Enemy>> enemies;
};