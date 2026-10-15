#include <imgui.h>
#include <ImGuizmo.h>
#include "Player.h"
#include <Collision.h>
#include <filesystem>

constexpr float POS = 0.0f;
constexpr float  Head = 1.0f;

Player::Player()
{
	Initialize();
}

void Player::Initialize()
{
	player.position = { 13, 0, 16 };
	player.scale = { 1.0f, 1.0f, 1.0f };
	player.model = std::make_unique<Model>("Data/Model/Mr.Incredible/Mr.Incredible.mdl");
}

// 更新処理
void Player::Update(float elapsedTime, Object& S, const Camera& C)
{
	// プレイヤー移動処理
	{
		// 入力処理
		float axisX = 0.0f;
		float axisY = 0.0f;
		if (GetAsyncKeyState('W') & 0x8000) axisY += 1.0f;
		if (GetAsyncKeyState('S') & 0x8000) axisY -= 1.0f;
		if (GetAsyncKeyState('D') & 0x8000) axisX += 1.0f;
		if (GetAsyncKeyState('A') & 0x8000) axisX -= 1.0f;

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
		const float speed = 100.0f * elapsedTime;
		float vecX = cameraFrontX * axisY + cameraRightX * axisX;
		float vecZ = cameraFrontZ * axisY + cameraRightZ * axisX;
		float moveX = vecX * speed;
		float moveZ = vecZ * speed;
		float moveLength = sqrtf(moveX * moveX + moveZ * moveZ);

		
		player.position.x += moveX;
		player.position.z += moveZ;

		// 移動しているときだけ向きを更新する
		if (moveLength > 0.0f)
		{
			float vx = moveX / moveLength;
			float vz = moveZ / moveLength;
			player.angle.y = atan2f(vx, vz);
		}

	}

	player.UpdateTransform();
	//// プレイヤー行列更新処理
	//{
	//	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(player.scale.x, player.scale.y, player.scale.z);
	//	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(player.angle.x, player.angle.y, player.angle.z);
	//	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(player.position.x, player.position.y, player.position.z);
	//	DirectX::XMStoreFloat4x4(&player.transform, S * R * T);
	//}


}

// 描画処理
void Player::Render(float elapsedTime, RenderContext rc,bool Pause)
{
	/*ID3D11DeviceContext* dc = Graphics::Instance().GetDeviceContext();
	RenderState* renderState = Graphics::Instance().GetRenderState();*/
	ModelRenderer* modelRenderer = Graphics::Instance().GetModelRenderer();

	modelRenderer->Render(rc, player.transform, player.model.get(), ShaderId::Lambert);
}
