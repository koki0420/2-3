#pragma once


#include "Graphics.h"
#include"Interaction.h"
#include <Sprite.h>
#include <EffectManager.h>
#include"Object.h"
#include "audio.h"
#include "Item.h"

//前方宣言
class audio_source_voice;
class Item;

class Player
{
private:
	Object								player;
	Object								Hand;
	Object								Arm;
	Object Arm_Item[Item::ITEM_TYPE_NUM];

public:
	Player();
	~Player() {};

	void Initialize();

	DirectX::XMFLOAT3  GetPlayerPos() { return { player.transform._41,player.transform._42,player.transform._43 };}

	float GetYaw() { return player.angle.y; }

	float GetPitch() { return player.angle.x; }

	// 更新処理
	void Update(float elapsedTime, Object& S, const Camera& C);
	// 描画処理
	void Render(float elapsedTime, RenderContext rc,bool Pause);


	//カメラを動かすか
	bool GetCamera() { return Switch_camera; }


	//アイテムがあるかどうか
	bool GetAliveitem(int index) { return item_alive[index]; }

	//アイテムの種類
	void SetItemType(Interact::InteractionType type,int index) { item_type[index] = type; }

	void SetItem(Object* item, int index) { I[index] = item; }

	//ギミックがあるかどうか
	bool GetAliveGimmick(int index) { return gimmick_alive[index]; }

	//ギミックの種類
	void SetGimmickType(int type, int index) { gimmick_type[index] = type; }

	void SetGimmick(Object* gimmick, int index) { G[index] = gimmick; }

	void FocusItem(DirectX::XMFLOAT3 rayStart, DirectX::XMFLOAT3 rayEnd, float wallDist);

	void FocusGimick(DirectX::XMFLOAT3 rayStart, DirectX::XMFLOAT3 rayEnd, float wallDist);

	void WallMove(DirectX::XMFLOAT3 rayStart, DirectX::XMFLOAT3 rayEnd, Object& S);

	void ArmMove(DirectX::XMFLOAT3 f, DirectX::XMFLOAT3 r);

	void ArmStart(DirectX::XMFLOAT3 f, DirectX::XMFLOAT3 r);

	// seのindex番目にSEを登録
	void SetSE(audio_source_voice* SE, int index) { se[index] = SE; }

	bool CutRopeItem() { return FallItem; }
	bool CutRopeGimmick() { return FallGimmick; }

	void UpdateArmPlayer() 
	{
		arm_player.x = move_position.x - player.position.x;
		arm_player.y = move_position.y - player.position.y;
		arm_player.z = move_position.z - player.position.z;
		DirectX::XMVECTOR arm_player_vec = DirectX::XMLoadFloat3(&arm_player);
		arm_player_vec = DirectX::XMVector3Normalize(arm_player_vec);
		DirectX::XMStoreFloat3(&arm_player, arm_player_vec);

	}

	audio_source_voice* const& GetSE(int index) { return se[index]; }

	const DirectX::XMFLOAT3 GetItemEffectPos() const { return itemEffectPos; }

private:
	constexpr static float ARM_RANGE_MAX = 0.3f;

	DirectX::XMFLOAT3 front; // 旧f

	DirectX::XMFLOAT3 move_position = { 0,0,0 };
	float Speed = 0;
	bool Switch_camera = false;
	float long_Hand = 0;

	//アイテム
	bool get_item = false;
	bool having_item = false; //アイテムを持っているか
	Interact::InteractionType having_item_type = Interact::InteractionType::Bomb; //持っているアイテムのタイプ
	DirectX::XMFLOAT3 arm_player;

	Interact::InteractionType item_type[ITEM];
	bool item_alive[ITEM];
	Object* I[ITEM];
	std::unique_ptr<Sprite> UI[Item::ITEM_TYPE_NUM];

	int gimmick_type[GIMMICK];
	bool gimmick_alive[GIMMICK];
	Object* G[GIMMICK];
	bool FallItem = false;
	bool FallGimmick = false;
	float i_timer = 0;

	int item_r = 1;
	int item_g = 1;
	int item_b = 1;
	bool use_item=false;
	bool focus_item=false;
	bool focus_gimmick=false;
	std::unique_ptr<Sprite> Target;

	Effect* bomb=nullptr;
	Effect* sword = nullptr;
	Effect* smash = nullptr;
	Effect* flash = nullptr;
	Effect* touch = nullptr;
	Effect* move = nullptr;

	bool handWallHit = false;  
	Effekseer::Handle handle_move = -1;


	audio_source_voice* se[Item::ITEM_TYPE_NUM]{}; //要素数は、何個のSEを使うか
	DirectX::XMFLOAT3 itemEffectPos{}; //アイテムを投げて、エフェクトが発生す位置
};

