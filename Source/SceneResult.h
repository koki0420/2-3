#pragma once

#include "Scene.h"
#include "Graphics.h"
#include"Sprite.h"
#include"audio.h"
#include"AudioResource.h"

class SceneResult :public Scene
{
public:
	SceneResult();
	~SceneResult();

public:
	void Initialize()override;
	void Finalize()override;

	void Update(float elapsedTime);
	void Render(float elapsedTime);
private:
	std::unique_ptr<Sprite> spriteTitele = nullptr;
	std::unique_ptr<Sprite> sprite = nullptr;
	std::unique_ptr<Sprite> result = nullptr;
	std::unique_ptr<audio_source_voice> Click = nullptr;
};
