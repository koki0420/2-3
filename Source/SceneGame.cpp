#pragma once

#include "SceneGame.h"
#include <imgui.h>
#include <ImGuizmo.h>
#include <DirectXCollision.h>
#include <algorithm>

#include "Graphics.h"
#include "Collision.h"
#include <SceneManager.h>
#include"SceneTitle.h"
#include"SceneResult.h"
#include"SceneLoading.h"

#include <EffectManager.h>

#include "audio.h"
#include "AudioResource.h"




SceneGame::SceneGame()
{
	
}

SceneGame::~SceneGame()
{

	

}

void SceneGame::Initialize()
{
	stage.model = std::make_unique<Model>("Data/Model/Stage/stage4.mdl");

	enemy_num = 0;
	
	stage.scale.x = 0.2f;
	stage.scale.y = 0.2f;
	stage.scale.z = 0.2f;

	stage.position.y = -20;

	boss=std::make_unique<Boss>(DirectX::XMFLOAT3{ 0, 0, 100 });

	for (int i = 0;i < 10;i += 1)
	{
		for (int j = 0;j < 10;j += 1)
		{
			enemies.emplace_back(std::make_unique<Enemy>(DirectX::XMFLOAT3{ -260 + 52.0f * j, 0, -260 + 52.0f * i }));
		}
	}
	for (size_t i = 0; i < enemies.size(); i++)
	{
		EnemyManager::Instance().Register(std::move(enemies[i]));
		enemy_num += 1;
	}
	spriteGauge = std::make_unique<Sprite>(Graphics::Instance().GetDevice(), "Data/Sprite/gauge_bar_neon-green_01_ff4444_128.png");
	spriteHP = std::make_unique<Sprite>(Graphics::Instance().GetDevice(), "Data/Sprite/gauge_bar_blue-gold_01_ff4444_128.png");

	spriteW1 = std::make_unique<Sprite>(Graphics::Instance().GetDevice(), "Data/Sprite/sword_transparent.png");
	spriteW2 = std::make_unique<Sprite>(Graphics::Instance().GetDevice(), "Data/Sprite/scythe_transparent .png");
	spriteW3 = std::make_unique<Sprite>(Graphics::Instance().GetDevice(), "Data/Sprite/glove_transparent .png");
}	

void SceneGame::Finalize()
{
	EnemyManager::Instance().Clear();
}

void SceneGame::Update(float elapsedTime)
{
	if (Boss_Dead)
	{
		boss->Boss_Dead(Boss_Dead);
	

		if (!(boss->Boss_Alive()))
		{
			Respawn();
			boss->Boss_Chage();
			Boss_Dead = false;
		}
	}
	{
		if (enemy_num > 0)boss->Boss_Set(false);
		else
		{
			boss->Boss_Set(true);
			boss->Update(elapsedTime);
		}
	}

	camera.Update();
	stage.UpdateTransform();


	EnemyManager::Instance().Update(elapsedTime);

	EffectManager::Instance().Update(elapsedTime);

}

void SceneGame::Render(float elapsedTime)
{
	ID3D11DeviceContext* dc = Graphics::Instance().GetDeviceContext();
	RenderState* renderState = Graphics::Instance().GetRenderState();
	ModelRenderer* modelRenderer = Graphics::Instance().GetModelRenderer();

	ShapeRenderer* shapeRenderer = Graphics::Instance().GetShapeRenderer();

	RenderContext rc;
	rc.deviceContext = dc;

	rc.renderState = renderState;
	rc.camera = &camera;

	modelRenderer->Render(rc, stage.transform, stage.model.get(), ShaderId::Lambert);
	EnemyManager::Instance().Render(rc, modelRenderer);

	EffectManager::Instance().Render(rc.camera->GetView(), rc.camera->GetView());

	boss->Render(rc, modelRenderer);
	
	{
		spriteGauge->Render(dc, 50, 50, 0, 480, 32, 0, 48, 128, 32, 0, 1, 1, 1, 1);
		spriteHP->Render(dc, 50, 50, 0, 480, 32, 0, 48, 128, 32, 0, 1, 1, 1, 1);
		//


		spriteGauge->Render(dc, 80, 500, 0, 280, 28, 0, 48, 128, 32, 0, 1, 1, 1, 1);
		spriteHP->Render(dc, 80, 500, 0, 280, 28, 0, 48, 128, 32, 0, 1, 1, 1, 1);
		spriteW1->Render(dc, 60, 500, 0, 28, 28, 0, 0, 134, 134, 0, 1, 1, 1, 1);

		spriteGauge->Render(dc, 80, 532, 0, 280, 28, 0, 48, 128, 32, 0, 1, 1, 1, 1);
		spriteHP->Render(dc, 80, 532, 0, 280, 28, 0, 48, 128, 32, 0, 1, 1, 1, 1);
		spriteW2->Render(dc, 60, 532, 0, 28, 28, 0, 0, 134, 134, 0, 1, 1, 1, 1);

		spriteGauge->Render(dc, 80, 564, 0, 280, 28, 0, 48, 128, 32, 0, 1, 1, 1, 1);
		spriteHP->Render(dc, 80, 564, 0, 280, 28, 0, 48, 128, 32, 0, 1, 1, 1, 1);
		spriteW3->Render(dc, 60, 564, 0, 28, 28, 0, 0, 134, 134, 0, 1, 1, 1, 1);

	}

}

void SceneGame::DrawGUI()
{
	ImVec2 displaySize = ImGui::GetIO().DisplaySize;
	ImVec2 pos = ImGui::GetMainViewport()->GetWorkPos();
	float width = 210;
	float height = 460;
	ImGui::SetNextWindowPos(ImVec2(pos.x + 10, pos.y + 10), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiCond_Once);

	if (ImGui::Begin("SceneGame"))
	{
		if (ImGui::Button("enemy"))
		{
			if (enemy_num > 0)
			{
				EnemyManager::Instance().Enemy_Set(enemy_num_debug,false);
				enemy_num -= 1;
				enemy_num_debug += 1;
			}
		}
		if (ImGui::Button("all_enemy"))
		{
			if (enemy_num > 0)
			{
				for (size_t i = 0;i < enemies.size();i += 1)
				{
					EnemyManager::Instance().Enemy_Set(i, false);
					enemy_num -= 1;
				}
			}
		}
		if (ImGui::Button("boss"))
		{
			if (boss->Boss_Alive()&&enemy_num<=0)
			{
				Boss_Dead = true;
			}
		}
	}
	ImGui::End();
}

void SceneGame::Respawn()
{
	for (size_t i = 0; i < enemies.size(); i++)
	{
		EnemyManager::Instance().Enemy_Set(i,true);
		enemy_num += 1;
	}

}

