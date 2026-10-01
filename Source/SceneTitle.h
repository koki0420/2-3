#pragma once

#include "Scene.h"
#include "Graphics.h"
#include<Sprite.h>

class SceneTitle :public Scene
{
public:
	SceneTitle() ;
	~SceneTitle();

public:
	void Initialize()override;
	void Finalize()override;

	void Update(float elapsedTime)override;
	void Render(float elapsedTime)override;
	//bool GetFinish() { return FINISH; }

private:
	std::unique_ptr<Sprite> Title;

	std::unique_ptr<Sprite> GameSprite;
	float game_alpha = 0.4f;
	std::unique_ptr<Sprite> FinishSprite;
	float finish_alpha = 0.4f;

	bool title_game = false;
	bool finish_game = false;
	//bool FINISH = false;

};
