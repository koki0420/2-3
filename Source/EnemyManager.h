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

	bool Enemy_Alive(int i) { return enemies[i]->Alive(); }

	void Enemy_Set(int i, bool Alive);
	

	void Clear() { enemies.clear(); }

private:
	std::vector<std::unique_ptr<Enemy>> enemies;
	int enemy_respawn_w = 0;
	int enemy_respawn_h = 0;
	//敵の数（シーンゲームとは使い方別）
	int enemy_num=0;
};