#include <imgui.h>
#include <ImGuizmo.h>
#include "Player.h"
#include <Collision.h>
#include <filesystem>

constexpr float POS = 0.0f;
constexpr float  Head = 1.0f;

Player::Player()
{
	spriteGauge = std::make_unique<Sprite>(Graphics::Instance().GetDevice(), "Data/Sprite/gauge_bar_neon-green_01_ff4444_128.png");
	spriteHP = std::make_unique<Sprite>(Graphics::Instance().GetDevice(), "Data/Sprite/gauge_bar_blue-gold_01_44cc44_128.png");
	spriteWP = std::make_unique<Sprite>(Graphics::Instance().GetDevice(), "Data/Sprite/gauge_bar_blue-gold_01_ff4444_128.png");

	spriteW[0] = std::make_unique<Sprite>(Graphics::Instance().GetDevice(), "Data/Sprite/sword_transparent.png");
	spriteW[1] = std::make_unique<Sprite>(Graphics::Instance().GetDevice(), "Data/Sprite/scythe_transparent.png");
	spriteW[2] = std::make_unique<Sprite>(Graphics::Instance().GetDevice(), "Data/Sprite/glove_transparent.png");

}

void Player::Initialize()
{
	
}

// 更新処理
void Player::Update(float elapsedTime, Object& S, const Camera& C)
{
}

// 描画処理
void Player::Render(float elapsedTime, RenderContext rc,bool Pause)
{	
}

void Player::UIRender(float elapsedTime, ID3D11DeviceContext* dc)
{
	

	spriteGauge->Render(dc, 50, 50, 0, 480, 32, 0, 48, 128, 32, 0, 1, 1, 1, 1);
	spriteHP->Render(dc, 50, 50, 0, 480, 32, 0, 48, 128, 32, 0, 1, 1, 1, 1);
	spriteW[wt]->Render(dc, 10, 50, 0, 56, 56, 0, 0, 134, 134, 0, 1, 1, 1, 1);
	spriteGauge->Render(dc, 50, 82, 0, 280, 28, 0, 48, 128, 32, 0, 1, 1, 1, 1);
	spriteWP->Render(dc, 50, 82, 0, 280, 28, 0, 48, 128, 32, 0, 1, 1, 1, 1);
		//


	spriteGauge->Render(dc, 80, 500, 0, 280, 28, 0, 48, 128, 32, 0, 1, 1, 1, 1);
	spriteWP->Render(dc, 80, 500, 0, 280, 28, 0, 48, 128, 32, 0, 1, 1, 1, 1);
	spriteW[wt]->Render(dc, 60, 500, 0, 28, 28, 0, 0, 134, 134, 0, 1, 1, 1, 1);

	spriteGauge->Render(dc, 80, 532, 0, 280, 28, 0, 48, 128, 32, 0, 1, 1, 1, 1);
	spriteWP->Render(dc, 80, 532, 0, 280, 28, 0, 48, 128, 32, 0, 1, 1, 1, 1);
	spriteW[(1+ wt)%3]->Render(dc, 60, 532, 0, 28, 28, 0, 0, 134, 134, 0, 1, 1, 1, 1);

	spriteGauge->Render(dc, 80, 564, 0, 280, 28, 0, 48, 128, 32, 0, 1, 1, 1, 1);
	spriteWP->Render(dc, 80, 564, 0, 280, 28, 0, 48, 128, 32, 0, 1, 1, 1, 1);
	spriteW[(2 + wt)%3]->Render(dc, 60, 564, 0, 28, 28, 0, 0, 134, 134, 0, 1, 1, 1, 1);
}
