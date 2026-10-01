#pragma once

#include "SceneAudioWASD.h"
#include"Interaction.h"
#include <imgui.h>
#include <ImGuizmo.h>
#include <DirectXCollision.h>
#include <algorithm>

#include "Graphics.h"
#include "Collision.h"


constexpr float POS = 0.0f;

constexpr float HEAD = 1.0f;
// コンストラクタ
PlayerAudioVer::PlayerAudioVer()
{
	player.onGround = false;
	player.position = { 0, POS, 0 };
	player.scale = { 0.01f, 0.01f, 0.01f };
	player.model = nullptr;

}


// 更新処理
void PlayerAudioVer::Update(float elapsedTime, Object& S, const Camera& C)
{
	HWND hwnd = GetActiveWindow();
	RECT rc;
	GetClientRect(hwnd, &rc);
	POINT center;
	center.x = (rc.right - rc.left) / 2;
	center.y = (rc.bottom - rc.top) / 2;

	//スクリーン座標の設定
	DirectX::XMVECTOR ScreenPosition, WorldPosition;
	DirectX::XMFLOAT3 screenPosition;
	screenPosition.x = static_cast<float>(center.x);
	screenPosition.y = static_cast<float>(center.y);

	float screenWidth = Graphics::Instance().GetScreenWidth();
	float screenHeight = Graphics::Instance().GetScreenHeight();
	//各行列を取得
	DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&C.GetView());
	DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&C.GetProjection());
	DirectX::XMMATRIX World = DirectX::XMMatrixIdentity();

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
		0, 0, screenWidth, screenHeight,
		0.0f, 1.0f,
		Projection, View, World);
	DirectX::XMFLOAT3 rayEnd;
	DirectX::XMStoreFloat3(&rayEnd, WorldPosition);

	// プレイヤー移動処理
	// 入力処理
	float axisX = 0.0f;
	float axisY = 0.0f;
	if (GetAsyncKeyState('W') & 0x8000) axisY += 1.0f;
	if (GetAsyncKeyState('S') & 0x8000) axisY -= 1.0f;
	if (GetAsyncKeyState('D') & 0x8000) axisX += 1.0f;
	if (GetAsyncKeyState('A') & 0x8000) axisX -= 1.0f;
	//上下移動
	if (GetAsyncKeyState('Q') & 0x8000) player.position.y += 1.5f * elapsedTime;
	if (GetAsyncKeyState('E') & 0x8000) player.position.y -= 1.5f * elapsedTime;

	DirectX::XMFLOAT3 right;
	right.x = cosf(GetYaw());
	right.y = 0.0f;
	right.z = -sinf(GetYaw());

	// カメラの方向
	const DirectX::XMFLOAT3& cameraFront = C.GetFront();
	const DirectX::XMFLOAT3& camemraRight = C.GetRight();
	float cameraFrontLengthXZ = sqrtf(cameraFront.x * cameraFront.x + cameraFront.z * cameraFront.z);
	float cameraRightLengthXZ = sqrtf(camemraRight.x * camemraRight.x + camemraRight.z * camemraRight.z);
	float cameraFrontX = cameraFront.x / cameraFrontLengthXZ;
	float cameraFrontZ = cameraFront.z / cameraFrontLengthXZ;
	float cameraRightX = camemraRight.x / cameraRightLengthXZ;
	float cameraRightZ = camemraRight.z / cameraRightLengthXZ;

	// 移動ベクトル
	const float speed = 5.0f * elapsedTime;
	float vecX = cameraFrontX * axisY + cameraRightX * axisX;
	float vecZ = cameraFrontZ * axisY + cameraRightZ * axisX;
	float moveX = vecX * speed;
	float moveZ = vecZ * speed;
	float moveLength = sqrtf(moveX * moveX + moveZ * moveZ);

	player.position.x += moveX;
	player.position.z += moveZ;

	// 進行方向を向くようにする
	DirectX::XMFLOAT3 f = cameraFront;
	float lenXZ = sqrtf(f.x * f.x + f.z * f.z);
	if (lenXZ > 0.0001f) {
		f.x /= lenXZ;
		f.z /= lenXZ;
	}
	player.angle.y = atan2f(f.x, f.z);

	
	if (Collision::sphereVssphere(player.position, 0.1f,
		{ move_position.x, 0, move_position.z }, 0.1f))Switch_camera = true;

	player.UpdateTransform();
}

// 描画処理
void PlayerAudioVer::Render(float elapsedTime, RenderContext rc)
{
	ID3D11DeviceContext* dc = Graphics::Instance().GetDeviceContext();
	ModelRenderer* modelRenderer = Graphics::Instance().GetModelRenderer();
	ShapeRenderer* shaperenderer = Graphics::Instance().GetShapeRenderer();

	shaperenderer->Render(dc, rc.camera->GetView(), rc.camera->GetProjection());
}

//コンストラクタ
SceneAudioWASD::SceneAudioWASD()
{	
}

SceneAudioWASD::~SceneAudioWASD()
{
}

void SceneAudioWASD::Initialize()
{
	p = std::make_unique<PlayerAudioVer>();
	item = std::make_unique<Item>(DirectX::XMFLOAT3(0, 0, 3), Interact::InteractionType::Bomb);


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
		{ 10, 5,3 },	// 視点
		{ 13, 5, 16 },	// 注視点
		{ 0, 1, 0 }		// 上ベクトル
	);



	cameraController.SyncCameraToController(camera);

	// モデル
	stage.model = std::make_unique<Model>("Data/Model/Greybox/Greybox.mdl");

	audio_buffers[0] = std::make_shared<audio_buffer>(L".\\Data\\Sounds\\SoundEffects\\Bomb.wav");

	se[0] = std::make_unique <audio_source_voice>(audio_buffers[0]);
	item->SetSE(se[0].get());
}

void SceneAudioWASD::Finalize()
{
}


void SceneAudioWASD::Update(float elapsedTime)
{
	// プレイヤー更新
	p->Update(elapsedTime, stage, camera);
	//item->Update(); //存在フラグをオンにしてアップデート
	if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
	{
		ShowCursor(TRUE);
	}
	else
	{
		camera.Update(p->GetPlayerPos(), p->GetCamera());
	}
	/*if (GetAsyncKeyState('1') & 1)
	{
	}*/
	//アイテムから常に音を再生する
	item->GetSE()->play(0);

	//パンの比率を計算
	//プレイヤーと音源の位置関係を割り出す
	float posVecX = item->GetPos().x - p->GetPlayerPos().x;
	float posVecZ = item->GetPos().z - p->GetPlayerPos().z;
	//正規化
	float playerToItemLengthXZ = sqrtf(posVecX * posVecX + posVecZ * posVecZ);
	posVecX /= playerToItemLengthXZ;
	posVecZ /= playerToItemLengthXZ;
	positionVecX = posVecX; // GUI
	positionVecZ = posVecZ;

	//プレイヤーの向き
	float playerDirX = sinf(p->GetYaw());
	float playerDirZ = cosf(p->GetYaw());
	playerDirectionX = playerDirX; // GUI
	playerDirectionZ = playerDirZ;
	// 2Dの外積で左右判定
	float resultCross = playerDirX * posVecZ - playerDirZ * posVecX;
	// 内積で、プレイヤーの向きとプレイヤーから音源までの角度を算出
	//float resultDot = playerDirX * posVecX + playerDirZ * posVecZ;
	//GUI用変数に格納
	resultCrossGUI = resultCross;
	//resultDotGUI = resultDot;
	// resultCrossを左-1.0～右1.0の範囲に調整してからパンを変更
	item->GetSE()->pan(-resultCross);
	//item->GetSE()->pan(panParam);

	//音量の距離減衰
	float seVolume = std::clamp(1.0f - playerToItemLengthXZ / hearMaxDist, 0.0f, 1.0f );
	item->GetSE()->volume(seVolume * bombVolumeDefault); //デフォルトの音量はSEごとに変更


	if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
	{
		ShowCursor(TRUE);
	}
	else
	{
		camera.Update(p->GetPlayerPos(), p->GetCamera());
	}
}

void SceneAudioWASD::Render(float elapsedTime)
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

	p->Render(elapsedTime, rc);
	item->Render(rc);
}

// GUI描画
void SceneAudioWASD::DrawGUI()
{
	ImVec2 displaySize = ImGui::GetIO().DisplaySize;
	ImVec2 pos = ImGui::GetMainViewport()->GetWorkPos();
	float width = 210;
	float height = 460;
	ImGui::SetNextWindowPos(ImVec2(pos.x + 10, pos.y + 10), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiCond_Once);

	if (ImGui::Begin("SceneAudioWASD"))
	{
		ImGui::InputFloat("pPosVecX", &positionVecX);
		ImGui::InputFloat("pPosVecZ", &positionVecZ);
		ImGui::InputFloat("pDirX", &playerDirectionX);
		ImGui::InputFloat("pDirZ", &playerDirectionZ);
		ImGui::InputFloat("cross", &resultCrossGUI);
		ImGui::InputFloat("dot", &resultDotGUI);
		//ImGui::DragFloat("pan", &panParam, 0.01f, -1.0f, 1.0f);
	}
	ImGui::End();
}

