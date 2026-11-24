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
	result = std::make_unique<Sprite>(Graphics::Instance().GetDevice(), "Data/Sprite/result.png");

	Click = std::make_unique< audio_source_voice>(AudioResource::audio_buffers[0]);
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

	float spriteWidth = 256 * S_W / 1920;
	float spriteHeight = 256 * S_H / 1080;

	float posX = S_W - spriteWidth - 1100 * S_W / 1920;
	float posX_t = S_W - spriteWidth - 900 * S_H / 1920;
	float posY = S_H - spriteHeight - 70;


	if ((my >= posY && my <= posY + spriteHeight) && !now)
	{
		if (mx >= posX && mx <= posX + spriteWidth && prev)
		{
			Click->play();
			SceneManager::Instance().ChangeScene(new SceneTitle);

		}
		if (mx >= posX_t && mx <= posX_t + spriteWidth && prev)
		{
			Click->play();
			PostMessage(Graphics::Instance().GetWindowHandle(), WM_CLOSE, 0, 0);
		}

	}
	prev = now;
}
void SceneResult::Render(float elapsedTime)
{
	float screenWidth = static_cast<float>(Graphics::Instance().GetScreenWidth());
	float screenHeight = static_cast<float>(Graphics::Instance().GetScreenHeight());
	ID3D11DeviceContext* dc = Graphics::Instance().GetDeviceContext();
	{
		float spriteWidth = 1920 * screenWidth / 1920;
		float spriteHeight = 1080 * screenHeight / 1080;
		result->Render(dc, 0, 0, 0, spriteWidth, spriteHeight, 0, 1, 1, 1, 1);
	}
	{
		float spriteWidth = 256 * screenWidth / 1920;
		float spriteHeight = 256 * screenHeight / 1080;
		float positionX = screenWidth - spriteWidth;
		float positionY = screenHeight - spriteHeight;

		spriteTitele->Render(dc, positionX - 1100 * screenWidth / 1920, positionY - 70, 0, spriteWidth, spriteHeight, 0, 1, 1, 1, 1);
		sprite->Render(dc, positionX - 900 * screenHeight / 1920, positionY - 70, 0, spriteWidth, spriteHeight, 0, 1, 1, 1, 1);
	}

}

