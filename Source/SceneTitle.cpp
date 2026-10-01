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
	Title = std::make_unique<Sprite>(Graphics::Instance().GetDevice(), "Data/Sprite/taitoru.png");
	GameSprite = std::make_unique<Sprite>(Graphics::Instance().GetDevice(),"Data/Sprite/GAMESTART.png");
	FinishSprite = std::make_unique<Sprite>(Graphics::Instance().GetDevice(),"Data/Sprite/Finish_the_game.png");
	title_game = false;
}

SceneTitle::~SceneTitle()
{

}
void SceneTitle::Initialize()
{
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

	//左クリックの押下情報を記録
	static bool prev = false;
	bool now = (GetAsyncKeyState(VK_LBUTTON) & 0x8000);

	//範囲内なら
	if (mx >= S_W / 10 && mx <= S_W / 10 + 0.4f * S_W&&
		my >= S_H / 1.7f && my <= S_H / 1.7f + 0.3f * S_H)
	{
		game_alpha = 1.0f;
		if (now)
		{
			title_game = true;			
		}
	}
	else 
	{
		title_game = false;
		game_alpha = 0.4f;
	}

	if (title_game)
	{
		if (!(now) && prev)
		{
			SceneManager::Instance().ChangeScene(new SceneLoading(new SceneGame));
		}
	}

	if (mx >= S_W / 1.9f && mx <= S_W / 1.9f + 0.4f * S_W &&
		my >= S_H / 1.7f && my <= S_H / 1.7f + 0.3f * S_H)
	{

		finish_alpha = 1.0f;
		if (now)
		{
			finish_game = true;
		}
	}
	else 
	{
		finish_game = false;
		finish_alpha = 0.4f;
	}
	if (finish_game)
	{
		if (!(now) && prev)
		{
			PostQuitMessage(0);
		}
	}

	//１フレーム前の情報を保持
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

	float S_W = Graphics::Instance().GetScreenWidth();
	float S_H = Graphics::Instance().GetScreenHeight();
	
	Title->Render(dc, 0, 0, 0, S_W, S_H, 0, 1, 1, 1, 1);
	GameSprite->Render  (dc, S_W / 10,    S_H /1.7f, 0, 0.4f * S_W, 0.4f * S_H, 1, 1, 1, 0, game_alpha);
	FinishSprite->Render(dc, S_W / 1.9f, S_H /1.7f, 0, 0.4f * S_W, 0.4f * S_H, 1, 1, 1, 0, finish_alpha);

}

