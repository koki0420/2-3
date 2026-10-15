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

	void Dead() { alive = false; }

private:
	Object enemy;
	bool alive;
};