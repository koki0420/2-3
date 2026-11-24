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
	Click = std::make_unique< audio_source_voice>(AudioResource::audio_buffers[0]);
}
void SceneTitle::Finalize()
{
	Click->stop();
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

	//左クリックの押下情報を記録
	static bool prev = false;
	bool now = (GetAsyncKeyState(VK_LBUTTON) & 0x8000);
	if ((my >= 500 && my <= 628) && !now)
	{
		if (mx >= 400 && mx <= 528 && prev)
		{
			Click->play();
			SceneManager::Instance().ChangeScene(new SceneLoading(new SceneGame));
			
		}
		if (mx >= 700 && mx <= 828 && prev)
		{
			Click->play();
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
	
	title->Render(dc, 0, 0, 0, 1280, 720, 0, 1, 1, 1, 1);
	spriteGame->Render(dc, 400, 500, 0, 128, 128, 0, 0, 128 , 128, 0, 1, 1, 1, 1);
	sprite->Render(dc, 700, 500, 0, 128, 128, 0, 0, 128 , 128, 0, 1, 1, 1, 1);
	
}

