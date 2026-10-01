#include <imgui.h>
#include <ImGuizmo.h>
#include <DirectXCollision.h>
#include "Graphics.h"
#include "Collision.h"
#include "Scene/ProjectScreenScene.h"

// コンストラクタ
ProjectScreenScene::ProjectScreenScene()
{
	ID3D11Device* device = Graphics::Instance().GetDevice();
	float screenWidth = Graphics::Instance().GetScreenWidth();
	float screenHeight = Graphics::Instance().GetScreenHeight();

	// カメラ設定
	camera.SetPerspectiveFov(
		DirectX::XMConvertToRadians(45),	// 画角
		screenWidth / screenHeight,			// 画面アスペクト比
		0.1f,								// ニアクリップ
		1000.0f								// ファークリップ
	);
	camera.SetLookAt(
		{ 0, 30, 30 },		// 視点
		{ 0, 0, 0 },		// 注視点
		{ 0, 1, 0 }			// 上ベクトル
	);
	cameraController.SyncCameraToController(camera);

	sprite = std::make_unique<Sprite>(device);
	stage.model = std::make_unique<Model>("Data/Model/Stage/ExampleStage.mdl");
}

// 更新処理
void ProjectScreenScene::Update(float elapsedTime)
{
	// カメラ更新処理
	cameraController.Update();
	cameraController.SyncControllerToCamera(camera);

	// マウス左クリックした
	if (GetAsyncKeyState(VK_LBUTTON) & 0x01)
	{
		// スクリーンサイズ取得
		float screenWidth = Graphics::Instance().GetScreenWidth();
		float screenHeight = Graphics::Instance().GetScreenHeight();

		// マウスカーソル位置の取得
		POINT cursor;
		::GetCursorPos(&cursor);
		::ScreenToClient(Graphics::Instance().GetWindowHandle(), &cursor);


		//各行列を取得
		DirectX::XMMATRIX View=DirectX::XMLoadFloat4x4(&camera.GetView());
		DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&camera.GetProjection());
		DirectX::XMMATRIX World = DirectX::XMMatrixIdentity();

		//スクリーン座標の設定
		DirectX::XMVECTOR ScreenPosition, WorldPosition;
		DirectX::XMFLOAT3 screenPosition;
		screenPosition.x = static_cast<float>(cursor.x);
		screenPosition.y = static_cast<float>(cursor.y);

		
		screenPosition.z = 0.0f;
		ScreenPosition = DirectX::XMLoadFloat3(&screenPosition);

		WorldPosition = DirectX::XMVector3Unproject(
			ScreenPosition,
			0, 0, screenWidth, screenHeight,
			0.0f, 1.0f,
			Projection, View, World);
		DirectX::XMFLOAT3 rayStart;
		DirectX::XMStoreFloat3(&rayStart, WorldPosition);


		screenPosition.z = 1.0f;
		ScreenPosition = DirectX::XMLoadFloat3(&screenPosition);

		DirectX::XMStoreFloat3(&screenPosition, ScreenPosition);
		WorldPosition = DirectX::XMVector3Unproject(
			ScreenPosition,
			0, 0, screenWidth , screenHeight,
			0.0f, 1.0f,
			Projection, View, World);
		DirectX::XMFLOAT3 rayEnd;
		DirectX::XMStoreFloat3(&rayEnd, WorldPosition);

		DirectX::XMFLOAT3 hitPosition, hitNormal;

		if (Collision::RayCast(rayStart, rayEnd, stage.transform, stage.model.get(), hitPosition, hitNormal))
		{
			//キャラクターを配置
			Object& obj = objs.emplace_back();
			obj.model = std::make_unique<Model>("Data/Model/Mr.Incredible/Mr.Incredible.mdl");
			obj.scale = { 0.01f,0.01f,0.01f };
			obj.position = hitPosition;
		}

	}


	// ステージ行列更新処理
	{
		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(stage.scale.x, stage.scale.y, stage.scale.z);
		DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(stage.angle.x, stage.angle.y, stage.angle.z);
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(stage.position.x, stage.position.y, stage.position.z);
		DirectX::XMStoreFloat4x4(&stage.transform, S * R * T);
	}

	// オブジェクト行列更新処理
	for (Object& obj : objs)
	{
		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(obj.scale.x, obj.scale.y, obj.scale.z);
		DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(obj.angle.x, obj.angle.y, obj.angle.z);
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(obj.position.x, obj.position.y, obj.position.z);
		DirectX::XMStoreFloat4x4(&obj.transform, S * R * T);
	}
}

// 描画処理
void ProjectScreenScene::Render(float elapsedTime)
{
	ID3D11DeviceContext* dc = Graphics::Instance().GetDeviceContext();
	RenderState* renderState = Graphics::Instance().GetRenderState();
	ModelRenderer* modelRenderer = Graphics::Instance().GetModelRenderer();

	// モデル描画
	RenderContext rc;
	rc.deviceContext = dc;
	rc.renderState = renderState;
	rc.camera = &camera;
	modelRenderer->Render(rc, stage.transform, stage.model.get(), ShaderId::Lambert);

	// スクリーンサイズ取得
	float screenWidth = Graphics::Instance().GetScreenWidth();
	float screenHeight = Graphics::Instance().GetScreenHeight();

	DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&camera.GetView());
	DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&camera.GetProjection());
	DirectX::XMMATRIX World = DirectX::XMMatrixIdentity();

	for (const Object& obj : objs)
	{
		modelRenderer->Render(rc, obj.transform, obj.model.get(), ShaderId::Lambert);

				// サンプラーステート
		ID3D11SamplerState* samplerStates[] =
		{
			renderState->GetSamplerState(SamplerState::LinearWrap)
		};
		dc->PSSetSamplers(0, _countof(samplerStates), samplerStates);


		//頭上のワールド行列
		DirectX::XMFLOAT3 position = obj.position;
		position.y += 2.0f;

		DirectX::XMVECTOR ScreenPosition;
		DirectX::XMVECTOR WorldPos = DirectX::XMLoadFloat3(&position);
		ScreenPosition = DirectX::XMVector3Project(
			WorldPos,
			0.0f, 0.0f,
			screenWidth, screenHeight,
			0.0f, 1.0f,
			Projection,
			View,
			World);

		//スクリーン
		DirectX::XMFLOAT2 screenPosition;
		DirectX::XMStoreFloat2(&screenPosition, ScreenPosition);

		//ゲージ描画
		const float gaugeWidth = 30.0f;
		const float gaugeHeight = 5.0f;

		sprite->Render(
			dc,
			screenPosition.x - gaugeWidth * 0.5f, screenPosition.y - gaugeHeight * 0.5f, 0.0f,
			gaugeWidth, gaugeHeight, 0, 1, 0, 0, 1);
	}
}

// GUI描画処理
void ProjectScreenScene::DrawGUI()
{
	ImVec2 pos = ImGui::GetMainViewport()->GetWorkPos();

	ImGui::SetNextWindowPos(ImVec2(pos.x + 10, pos.y + 10), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(300, 180), ImGuiCond_Once);

	if (ImGui::Begin(u8"スクリーン座標変換", nullptr, ImGuiWindowFlags_NoNavInputs))
	{
		ImGui::Text(u8"クリック：キャラ配置");
	}
	ImGui::End();
}
