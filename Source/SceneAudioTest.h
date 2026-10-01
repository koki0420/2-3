#pragma once

#include "Scene.h"
#include "Camera.h"
#include "FreeCameraController.h"

#include"Player.h"
#include <Item.h>

// XAUDIO2
#include "audio.h"
#include <vector>

class SceneAudioTest :public Scene
{

public:
	SceneAudioTest();
	~SceneAudioTest();

public:
	void Initialize()override;
	void Finalize()override;

	void Update(float elapsedTime)override;
	void Render(float elapsedTime)override;

private:
	Camera								camera;
	FreeCameraController				cameraController;
	Object								stage;


	// XAUDIO2
	std::shared_ptr<audio_buffer> audio_buffers[8]; //要素数は、何種類の音源を使うか
	//スマートポインタに変更
	std::unique_ptr<audio_source_voice> bgm[8]{};//要素数は、何個のBGMを使うか
	std::unique_ptr<audio_source_voice> se[8]{}; //要素数は、何個のSEを使うか

	std::unique_ptr <Player> p;
	std::unique_ptr<Item> item[ITEM];

};
#pragma once
