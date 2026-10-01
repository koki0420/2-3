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

private:


};
