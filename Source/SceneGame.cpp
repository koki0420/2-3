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
	p = std::make_unique<Player>();
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
	
	p->Initialize();
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

	EffectManager::Instance().Render(rc.camera->GetView(), rc.camera->GetProjection());

	boss->Render(rc, modelRenderer);

	{
		p->UIRender(elapsedTime, dc);
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

