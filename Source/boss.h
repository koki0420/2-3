#pragma once

#include"Object.h"

#include"ModelRenderer.h"
#include"Animation.h"

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


private:
	Object boss;
	unique_ptr<Animation> boss_anime;
	Object middle_boss;
	unique_ptr<Animation> middle_boss_anime;
	DirectX::XMFLOAT3 arrive_position = { 0,0,0 };
	bool alive = true;
	bool dead = false;
	bool arrive = true;
	bool dyingStarted = false;
	int boss_count = 0;
};