#pragma once

#include "Scene.h"
#include "Camera.h"
#include "FreeCameraController.h"

#include"Player.h"
#include <Item.h>

// XAUDIO2
#include "audio.h"
#include <vector>

#include "Graphics.h"
#include <Sprite.h>




class PlayerAudioVer
{
private:
	Object								player;
	//Object								Hand;
	//Object								Arm;

public:
	PlayerAudioVer();
	~PlayerAudioVer() {};

	DirectX::XMFLOAT3  GetPlayerPos() { return { player.transform._41,player.transform._42,player.transform._43 }; }

	const float GetYaw() { return player.angle.y; }
	//float GetPitch() { return player.angle.x; }

	// 更新処理
	void Update(float elapsedTime, Object& S, const Camera& C);
	// 描画処理
	void Render(float elapsedTime, RenderContext rc);

	//アイテムを持っているか
	bool GetItem() { return having_item; }

	//カメラを動かすか
	bool GetCamera() { return Switch_camera; }

	//アイテムの種類
	void SetItemType(int type) { item_type = type; }

private:
	DirectX::XMFLOAT3 move_position = { 0,0,0 };
	float Speed = 0;
	bool Switch_camera = false;
	float long_Hand = 0;

	//アイテム
	bool get_item = false;
	bool having_item = false;
	int item_type = 0;;
	int item_color = 0;
	bool use_item = false;
	//std::unique_ptr<Sprite> Target;

};



class SceneAudioWASD :public Scene
{

public:
	SceneAudioWASD();
	~SceneAudioWASD();

public:
	void Initialize()override;
	void Finalize()override;

	void Update(float elapsedTime)override;
	void Render(float elapsedTime)override;

	// GUI描画
	void DrawGUI() override;

private:
	Camera								camera;
	FreeCameraController				cameraController;
	Object								stage;

	//GUI用
	float playerDirectionX{};
	float playerDirectionZ{};
	float positionVecX{};
	float positionVecZ{};
	float resultCrossGUI{};
	float resultDotGUI{};
	//float panParam{};

	// XAUDIO2
	std::shared_ptr<audio_buffer> audio_buffers[8]; //要素数は、何種類の音源を使うか
	//スマートポインタに変更
	std::unique_ptr<audio_source_voice> bgm[8]{};//要素数は、何個のBGMを使うか
	std::unique_ptr<audio_source_voice> se[8]{}; //要素数は、何個のSEを使うか

	const float hearMaxDist = 20.0f;

	const float bombVolumeDefault = 5.0f;

	std::unique_ptr <PlayerAudioVer> p;
	std::unique_ptr<Item> item;

};
#pragma once
