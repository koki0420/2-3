#pragma once

#include "SceneResult.h"
#include "SceneTitle.h"
#include <Graphics.h>
#include <SceneManager.h>
#include<SceneLoading.h>


SceneResult::SceneResult()
{

}

SceneResult::~SceneResult()
{

}
void SceneResult::Initialize()
{
}
void SceneResult::Finalize()
{
}
void SceneResult::Update(float elapsedTime)
{
	while (ShowCursor(TRUE) <= 0) {}
	static bool prev = false;
	bool now = (GetAsyncKeyState(VK_LBUTTON) & 0x8000);
	if (!(now) && prev)
	{
		SceneManager::Instance().ChangeScene(new SceneTitle);
	}
	prev = now;
}
void SceneResult::Render(float elapsedTime)
{

}

