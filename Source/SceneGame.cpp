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

//a

SceneGame::SceneGame()
{

	stage.scale.x = 0.1f;
	stage.scale.y = 0.1f;
	stage.scale.z = 0.1f;
	// ���f��
	stage.model = std::make_unique<Model>("Data/Model/Stage/map(comp).mdl");

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

