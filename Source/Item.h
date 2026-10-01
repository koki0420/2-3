#pragma once
#include"Object.h"
#include"Interaction.h"
#include <ModelRenderer.h>

//前方宣言
class audio_source_voice;


class Item
{
public:
	Item(DirectX::XMFLOAT3 pos, Interact::InteractionType type);
	~Item();

	void Update(const DirectX::XMFLOAT3& playerPos);

	void Render(RenderContext rc);

	/*static void DrawGUI(){}*/

	DirectX::XMFLOAT3 GetPos() { return item.position; }

	void SetItemPosY(float pos) { item.position.y = pos; }

	void SubItemPosY(float pos) { item.position.y -= pos; }

	bool itemAlive() { return alive_item; }

	const Interact::InteractionType GetItemType() const { return static_cast<Interact::InteractionType>(item_type); } //オーバーロードしているが、のちにこちらのみに移行する

	Object* GetItem() { return &item; }

	audio_source_voice* const &GetSE() { return se; }

	//セッター
	void SetSE(audio_source_voice* SE) { se = SE; }

	void SetAlive(bool alive)
	{
		alive_item = alive;
	}

public:
	struct SE_Data
	{
		Interact::InteractionType itemType; // IDのような使い方
		float Volume{}; //SEのデフォルトのボリューム
	};

	//アイテムの種類数
	constexpr static int ITEM_TYPE_NUM = 4; //アイテムの種類を追加するごとに加算する
	constexpr static SE_Data ItemMasterData[ITEM_TYPE_NUM] = //アイテムごとのパラメータのマスターデータ
	{
		{Interact::InteractionType::Bomb,	2.0f}, //　アイテムの種類, デフォルトのボリューム
		{Interact::InteractionType::Slash,	3.0f},
		{Interact::InteractionType::Smash,	4.0f},
		{Interact::InteractionType::Flash,	3.0f},
	};

private:
	Object item;
	bool alive_item = false;
	Interact::InteractionType item_type;

	audio_source_voice* se = nullptr;
};


