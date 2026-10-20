#pragma once

#include"ModelRenderer.h"
#include"Object.h"

//エネミー
class Enemy
{
public:
	
	Enemy(DirectX::XMFLOAT3 pos);
	~Enemy();
	//更新処理
	void Update(float elapsedTime);
	//描画処理
	void Render(const RenderContext& immediate_context, ModelRenderer* renderer);

	bool Alive(){return alive;}

	void enemy_set(bool alive) { this->alive = alive; }

	DirectX::XMFLOAT3 GetPos() const { return enemy.position; };

	void SetPos(const DirectX::XMFLOAT3 pos)
	{
		enemy.position = pos;
	}


private:
	Object enemy;
	DirectX::XMFLOAT3 arrive_position = { 0,0,0 };
	bool alive=true;
	bool arrive = true;
};