#pragma once

#include "Scene.h"
#include "Graphics.h"
#include"Sprite.h"


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
};
