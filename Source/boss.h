#pragma once

#include"Object.h"

#include"ModelRenderer.h"
#include"Animation.h"

#include <Effect.h>
#include <EffectManager.h>

#include "Animation.h"

class Boss
{
public:
	Boss(DirectX::XMFLOAT3 pos);
	~Boss();

	void Update(float elapsedTime);

	void Render(const RenderContext& immediate_context, ModelRenderer* renderer);

	bool Boss_Alive() { return alive; }

	void Boss_Set(bool alive) 
	{
		
		this->alive=alive; 
	}

	void Boss_Dead(bool dead)
	{
		this->dead = dead;
	}

	void Boss_Chage() 
	{
		boss_count+=1;
		this->dead = false;
	}

	bool Clear() { return clear; }

private:
	//ボス
	Object boss;
	unique_ptr<Animation> boss_anime;

	//中ボス
	Object middle_boss;
	unique_ptr<Animation> middle_boss_anime;

	//AIぽい用の位置情報
	DirectX::XMFLOAT3 arrive_position = { 0,0,0 };

	//たどり着いたかどうか
	bool alive = true;

	//死んだか
	bool dead = false;

	//生きてるか
	bool arrive = true;

	//死ぬときのアニメーション用
	bool dyingStarted = false;

	//中ボス。ボスを分けるため
	int boss_count = 0;

	//クリア
	bool clear = false;


	//エフェクト
	Effect* midle_effect=nullptr;

	//ジャンプ用
	bool jump_effect_play;

	Effekseer::Handle Effect_handle = -1;
};