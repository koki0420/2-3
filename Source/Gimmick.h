#pragma once

#include"Object.h"
#include"Interaction.h"
#include <ModelRenderer.h>
#include"Animation.h"

class Gimmick
{
public:
	static enum GimmickType
	{
		BreakableWall,		//壊せる壁						//InteractionType::Bombが対応する
		KnockedDownBridge,	//倒して架ける橋				//Slashが対応
		DroppedBridge,		//落として架ける橋				//Slashが対応
		MovableContainer,	//動かせるコンテナ				//Sｍashが対応
		StringedUpRope,		//音源を吊り上げているロープ	//Slashが対応
	};

public:
	Gimmick(DirectX::XMFLOAT3 pos, GimmickType type);
	~Gimmick();

	void Update(float elapsedTime);

	void Render(RenderContext rc);

	DirectX::XMFLOAT3 GetPos() { return gimmick.position; }

	bool gimmickAlive() { return alive_gimmick; }

	const Interact::InteractionType GetTriggerItemType() const { return triggerItemType; }

	const GimmickType GetGimmickType() const { return gimmickType; }

	Object* GetGimmick() { return &gimmick; }

	void SetAlive(bool alive)
	{
		this->alive = alive;
	}

	void SetFallGimmick(bool gimmick)
	{
		FallGimmick = gimmick;
	}

private:
	Object gimmick;
	bool alive_gimmick = false;
	bool alive = false;
	bool animation_flag = false;
	bool FallGimmick = false;
	bool FallGimmickFlag = false;
	Interact::InteractionType triggerItemType; //対応するアイテムの種類
	GimmickType gimmickType; //ギミック自体の種類
	std::unique_ptr<Animation> animation;
};
