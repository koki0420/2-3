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

}

