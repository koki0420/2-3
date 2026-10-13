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

#include "audio.h"
#include "AudioResource.h"




SceneGame::SceneGame()
{

	stage.scale.x = 0.2f;
	stage.scale.y = 0.1f;
	stage.scale.z = 0.2f;
	
	stage.position.y = -20;
	
	stage.model = std::make_unique<Model>("Data/Model/Stage/stage4.mdl");
	
	for (int i = 0;i < 10;i += 1)
	{
		for (int j = 0;j < 10;j += 1)
		{
			enemies.emplace_back(std::make_unique<Enemy>(DirectX::XMFLOAT3{ -120+24.0f*j, 0, -120+24.0f*i }));
		}
	}
	/*enemies.emplace_back(std::make_unique<Enemy>(DirectX::XMFLOAT3{ 0, 0, 10 }));
	enemies.emplace_back(std::make_unique<Enemy>(DirectX::XMFLOAT3{ 10, 0, 10 }));*/

	
	for (size_t i = 0; i < enemies.size(); i++)
	{
		EnemyManager::Instance().Register(std::move(enemies[i]));
	}
}

SceneGame::~SceneGame()
{
	
}

void SceneGame::Initialize()
{

}	

void SceneGame::Finalize()
{
	
}

void SceneGame::Update(float elapsedTime)
{
	camera.Update();
	stage.UpdateTransform();
	EnemyManager::Instance().Update(elapsedTime);

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
	EnemyManager::Instance().Render(rc,modelRenderer);

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
	}
	ImGui::End();
	
}

