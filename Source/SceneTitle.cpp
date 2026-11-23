#pragma once

#include"Framework.h"
#include "SceneTitle.h"
#include "SceneGame.h"
#include "Graphics.h"
#include"Sprite.h"
#include <SceneManager.h>
#include<SceneLoading.h>



SceneTitle::SceneTitle()
{
	
}

SceneTitle::~SceneTitle()
{

}
void SceneTitle::Initialize()
{
	spriteGame = std::make_unique<Sprite>(Graphics::Instance().GetDevice(), "Data/Sprite/ic_input_capsule_start_128.png");
	sprite = std::make_unique<Sprite>(Graphics::Instance().GetDevice(), "Data/Sprite/ic_input_key-wide-filled_exit_128.png");
	title= std::make_unique<Sprite>(Graphics::Instance().GetDevice(), "Data/Sprite/title.png");
}
void SceneTitle::Finalize()
{
}
void SceneTitle::Update(float elapsedTime)
{
	while (ShowCursor(TRUE) <= 0) {}
	POINT mousePos;
	GetCursorPos(&mousePos); // 画面全体の座標（スクリーン座標）

	// ゲームウィンドウ内の座標に変換
	ScreenToClient(Graphics::Instance().GetWindowHandle(), &mousePos);
	// mousePos.x, mousePos.y がウィンドウ内のマウス座標
	int mx = mousePos.x;
	int my = mousePos.y;

	float S_W = Graphics::Instance().GetScreenWidth();
	float S_H = Graphics::Instance().GetScreenHeight();

	S_W / 10, S_H / 1.7f;

	float spriteWidth = 256 * S_W / 1920;
	float spriteHeight = 256 * S_H / 1080;

	float posX = S_W - spriteWidth - 1100 * S_W/ 1920;
	float posX_t = S_W - spriteWidth - 900 * S_H / 1920;
	float posY = S_H - spriteHeight - 70;
	
		
	//左クリックの押下情報を記録
	static bool prev = false;
	bool now = (GetAsyncKeyState(VK_LBUTTON) & 0x8000);

	if ((my >= posY && my <= posY+spriteHeight) && !now)
	{
		if (mx >= posX && mx <= posX+spriteWidth && prev)
		{
			
			SceneManager::Instance().ChangeScene(new SceneLoading(new SceneGame));
			
		}
		if (mx >= posX_t && mx <= posX_t+spriteWidth && prev)
		{
			
			PostMessage(Graphics::Instance().GetWindowHandle(), WM_CLOSE, 0, 0);
		}

	}
	prev = now;
}
void SceneTitle::Render(float elapsedTime)
{
	ID3D11DeviceContext* dc = Graphics::Instance().GetDeviceContext();
	RenderState* renderState = Graphics::Instance().GetRenderState();
	ModelRenderer* modelRenderer = Graphics::Instance().GetModelRenderer();

	RenderContext rc;
	rc.deviceContext = dc;
	rc.renderState = renderState;

	// レンダーステート設定
	dc->OMSetDepthStencilState(rc.renderState->GetDepthStencilState(DepthState::TestAndWrite), 0);
	// ブレンドステート設定
	dc->OMSetBlendState(rc.renderState->GetBlendState(BlendState::Transparency), nullptr, 0xFFFFFFFF);
	
	float screenWidth = static_cast<float>(Graphics::Instance().GetScreenWidth());
	float screenHeight = static_cast<float>(Graphics::Instance().GetScreenHeight());
	// 2Dスプライト描画
	{
		float spriteWidth = 1920 * screenWidth / 1920;
		float spriteHeight = 1080 * screenHeight / 1080;
		title->Render(dc, 0, 0, 0, spriteWidth, spriteHeight, 0, 1, 1, 1, 1);
	}

	{
		float spriteWidth = 256 * screenWidth / 1920;
		float spriteHeight = 256 * screenHeight / 1080;
		float positionX = screenWidth - spriteWidth ;
		float positionY = screenHeight - spriteHeight ;

		spriteGame->Render(dc, positionX-1100* screenWidth / 1920, positionY-70, 0, spriteWidth, spriteHeight,  0, 1, 1, 1, 1);
		sprite->Render(dc,  positionX-900* screenHeight / 1920,  positionY-70, 0, spriteWidth, spriteHeight, 0, 1, 1, 1, 1);
	}	
}



