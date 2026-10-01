#pragma once
#include"Object.h"
#include"Interaction.h"
#include <ModelRenderer.h>

class SoundSource //音源のオブジェクトを管理
{
public:
	enum class ModelType
	{
		Gramophone, //蓄音機
		TV,			//テレビ
	};
public:
	SoundSource(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 angle, const ModelType type);
	~SoundSource() = default;
	void Update(); //設置位置調整用(後で消す)
	void Render(RenderContext rc);
	void DrawGUI();


private:
	Object soundSource;
	Object desk;
	ModelType modelType; //どのモデルを使用するか
	//Interact::InteractionType createItemType; //生成するアイテムのタイプ
public:
	void SetSize(float size) { soundSource.scale = { size, size, size }; };
	void SetPosition(DirectX::XMFLOAT3 pos) { desk.position = { pos.x, 0 , pos.z }; };
	void SubPosition(float pos) { if (soundSource.position.y > 0.83f)soundSource.position.y -= pos; }
	bool SoundPos() { return soundSource.position.y <= 0.83f; }
	
	const ModelType const GetModelType() { return modelType; };
};
