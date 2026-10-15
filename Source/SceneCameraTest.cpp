#include "SceneCameraTest.h"
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

//

SceneCameraTest::SceneCameraTest()
{

	stage.scale.x = 1;
	stage.scale.y = 1;
	stage.scale.z = 1;

	stage.model = std::make_unique<Model>("Data/Model/Stage/stage4.mdl");

	cameraController.SyncCameraToController(camera);
}

SceneCameraTest::~SceneCameraTest()
{

}

void SceneCameraTest::Initialize()
{

}

void SceneCameraTest::Finalize()
{

}

void SceneCameraTest::Update(float elapsedTime)
{
	

	cameraController.Update();
	cameraController.SyncControllerToCamera(camera);

	camera.SetLookAt(camera.GetEye(), player.player.position, camera.GetUp());

	player.Update(elapsedTime,stage,camera);
}

void SceneCameraTest::Render(float elapsedTime)
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
	player.Render(elapsedTime, rc, false);

}

void SceneCameraTest::DrawGUI()
{
	ImVec2 displaySize = ImGui::GetIO().DisplaySize;
	ImVec2 pos = ImGui::GetMainViewport()->GetWorkPos();
	float width = 210;
	float height = 460;
	ImGui::SetNextWindowPos(ImVec2(pos.x + 10, pos.y + 10), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiCond_Once);

	if (ImGui::Begin("SceneCameraTest"))
	{
		ImGui::DragFloat3("pPosition", &player.player.position.x, 0.1f);
		ImGui::DragFloat3("pScale", &player.player.scale.x, 0.1f);
	}
	ImGui::End();

}

