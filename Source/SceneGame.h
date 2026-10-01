#pragma once

#include "Scene.h"
#include"Object.h"
#include "Camera.h"
#include "FreeCameraController.h"

#include"Framework.h"
#include"Player.h"
#include <EffectManager.h>

// XAUDIO2
//#include "audio.h"
//#include <vector> //未使用のため削除

//前方宣言

class audio_source_voice;
class audio_buffer;

class SceneGame :public Scene
{
public:
	SceneGame() ;
	~SceneGame() ;

public:
	void Initialize()override;
	void Finalize()override;
	
	void Update(float elapsedTime)override;
	void Render(float elapsedTime)override;

	void DrawGUI() override;



private:
	
	Camera								camera;
	FreeCameraController				cameraController;
	Object								stage;



	//GUI用------------------------------------------
	int itemNum{};
	float playerDirectionX{};
	float playerDirectionZ{};
	float positionVecX{};
	float positionVecZ{};
	float resultCrossGUI{};
	float resultDotGUI{};
	//使用中の変数
	float coefficientVolumeGUI = 1.0f; //デフォルトの音量(全体)(係数)
	float gramoSizeGUI = 0.22f;	//音源拡縮用変数
	float TVSizeGUI = 0.25f;		//音源拡縮用変数
	DirectX::XMFLOAT3 soundSourcesPos[SOUND_SOURCE_NUM] = //音源配置用変数
	{
		{-17.2f,	4.6f,		8.0f},		//紐切り
		{-14.5f,	0.0f,		-17.5f},	//横穴１
		{-14.5f,	0.0f,		-20.0f},	//横穴２
		{15.5f,		0.0f,		-16.7f},	//コンテナ前１　
		{15.5f,		0.0f,		-14.0f},	//コンテナ前２
		{15.5f,		0.0f,		25.7f},		//爆破前１
		{6.5f,		0.0f,		25.7f},		//爆破前２
	};
	

	//SE パラメータ
	float bombVolume  = 1.0f;
	float slashVolume = 3.0f;
	float smashVolume = 4.0f;
	//-----------------------------------------------------

	// AudioSourceへ移動
	//std::shared_ptr<audio_buffer> audio_buffers[8]; //要素数は、何種類の音源を使うか
	//スマートポインタに変更
	//std::unique_ptr<audio_source_voice> bgm[1]{};//要素数は、何個のBGMを使うか
	//std::unique_ptr<audio_source_voice> sePlayer[Item::ITEM_TYPE_NUM]{}; //要素数は、何個のSEを使うか	//プレイヤーにもたせるSE
	//std::unique_ptr<audio_source_voice> seItem[ITEM]{}; //アイテムにもたせるSE
	// seSystemの添字はAudioResourceNumber::「欲しい番号」- Item::ITEM_TYPE_NUMで取得できる //システム音
	//std::unique_ptr<audio_source_voice> seSystem[AudioResourceNumber::CountNum - Item::ITEM_TYPE_NUM]{};
	
	//const float bombVolumeDefault = 5.0f;
	

	std::unique_ptr <Player> p;
	std::unique_ptr<Sprite> pauseSprite;

	Object   Light[LIGHT_MAX];

	std::unique_ptr<Sprite> GameSprite;
	float game_alpha = 0.4f;
	bool resumeFlag = false; //ポーズ中の遷移フラグ //ゲームに戻る
	bool isResumeSelected = false; // 1F前にUIの範囲内に入っていたか

	std::unique_ptr<Sprite> TitleSprite;
	float title_alpha = 0.4f;
	bool toTitleFlag = false;
	bool isToTitleSelected = false; // 1F前にUIの範囲内に入っていたか

	std::unique_ptr<Sprite> ReTrySprite;
	float  ReTry_alpha = 0.4f;
	bool retryFlag = false;
	bool isRetrySelected = false; // 1F前にUIの範囲内に入っていたか

	bool pause_flag = false;
	bool pause = false;

	//ゴールの位置
	DirectX::XMFLOAT3 goalPos = { 0,0,0 };

	Effect* goalEffect = nullptr;
	Effekseer::Handle goalEffectHandle = -1;


	bool goalReached = false;
	float goalTimer = 0.0f;

};
