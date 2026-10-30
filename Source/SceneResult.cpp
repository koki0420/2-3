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
	spriteTitele = std::make_unique<Sprite>(Graphics::Instance().GetDevice(), "Data/Sprite/ic_input_key-wide-filled_title_128.png");
	sprite = std::make_unique<Sprite>(Graphics::Instance().GetDevice(), "Data/Sprite/ic_input_key-wide-filled_exit_128.png");
}
void SceneResult::Finalize()
{
}
void SceneResult::Update(float elapsedTime)
{
	while (ShowCursor(TRUE) <= 0) {}
	static bool prev = false;
	bool now = (GetAsyncKeyState(VK_LBUTTON) & 0x8000);
	POINT mousePos;
	GetCursorPos(&mousePos);
	ScreenToClient(Graphics::Instance().GetWindowHandle(), &mousePos);
	// mousePos.x, mousePos.y がウィンドウ内のマウス座標
	int mx = mousePos.x;
	int my = mousePos.y;

	float S_W = Graphics::Instance().GetScreenWidth();
	float S_H = Graphics::Instance().GetScreenHeight();

	S_W / 10, S_H / 1.7f;

	//左クリックの押下情報を記録
	if (my >= 500 && my <= 628&&prev && !now)
	{
		if (mx >= 400 && mx <= 528 )
		{
			
			SceneManager::Instance().ChangeScene(new SceneTitle);

		}
		if (mx >= 700 && mx <= 828)
		{

			PostMessage(Graphics::Instance().GetWindowHandle(), WM_CLOSE, 0, 0);
		}
	}
	prev = now;
}
void SceneResult::Render(float elapsedTime)
{
	ID3D11DeviceContext* dc = Graphics::Instance().GetDeviceContext();
	spriteTitele->Render(dc, 400, 500, 0, 128, 128, 0, 0, 128, 128, 0, 1, 1, 1, 1);
	sprite->Render(dc, 700, 500, 0, 128, 128, 0, 0, 128, 128, 0, 1, 1, 1, 1);

}

