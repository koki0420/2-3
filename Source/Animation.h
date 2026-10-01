#pragma once

#include <memory>
#include "Scene.h"
#include "Camera.h"
#include "FreeCameraController.h"
#include "Model.h"

class Animation 
{
private:

public:

	Animation(Model& M);
	~Animation() = default;


	// 更新処理
	void Update(float elapsedTime);
	bool IsPlaying() const
	{
		return animationPlaying;
	}


	// アニメーション再生
	void PlayAnimation(int index, bool loop);
	void PlayAnimation(const char* name, bool loop);
private:
	// アニメーション更新処理
	void UpdateAnimation(float elapsedTime);



private:
	//Camera								camera;
	//FreeCameraController				cameraController;

	Model*			model;

	int									animationIndex = -1;
	float								animationSeconds = 0.0f;
	bool								animationLoop = false;
	bool								animationPlaying = false;
	float								animationBlendSecondsLength = 0.2f;

};
