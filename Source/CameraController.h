#pragma once

#include <imgui.h>
#include "Camera.h"

class CameraController
{
public:
	// カメラからコントローラーへパラメータを同期する
	void SyncCameraToController(const Camera& camera)
	{
		eye = camera.GetEye();
		focus = camera.GetFocus();
		up = camera.GetUp();
		right = camera.GetRight();

		// 視点から注視点までの距離を算出
		DirectX::XMVECTOR Eye = DirectX::XMLoadFloat3(&eye);
		DirectX::XMVECTOR Focus = DirectX::XMLoadFloat3(&focus);
		DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(Focus, Eye);
		DirectX::XMVECTOR Distance = DirectX::XMVector3Length(Vec);
		DirectX::XMStoreFloat(&distance, Distance);

		// 回転角度を算出
		const DirectX::XMFLOAT3& front = camera.GetFront();
		angleX = ::asinf(-front.y);
		if (up.y < 0)
		{
			if (front.y > 0)
			{
				angleX = -DirectX::XM_PI - angleX;
			}
			else
			{
				angleX = DirectX::XM_PI - angleX;
			}
			angleY = ::atan2f(front.x, front.z);
		}
		else
		{
			angleY = ::atan2f(-front.x, -front.z);
		}

	};


	// コントローラーからカメラへパラメータを同期する
	void SyncControllerToCamera(Camera& camera) 
	{
		camera.SetLookAt(eye, focus, up);
	}

	// 更新処理
	void Update(float elapsedTime, float playerYaw)
	{
		// デバッグウインドウ操作中は処理しない
		if (ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow))
		{
			return;
		}
		if (elapsedTime <= 0.0f)return;

		//ゆっくり追いかける
		const float positionRate =
			1.0f - std::exp(-followSpeed * elapsedTime);

		focus.x += (targetFocus.x - focus.x) * positionRate;
		focus.y += (targetFocus.y - focus.y) * positionRate;
		focus.z += (targetFocus.z - focus.z) * positionRate;

		//角度差を抑えて、近い方向へ回転
		const float difference = playerYaw - followYaw;
		const float angleDifference =
			std::atan2(std::sin(difference), std::cos(difference));

		const float rotationRate =
			1.0f - std::exp(-turnSpeed * elapsedTime);

		followYaw += angleDifference * rotationRate;

		// IMGUIのマウス入力値を使ってカメラ操作する
		const ImGuiIO io = ImGui::GetIO();

		// マウス右ボタン押下中
		if (!io.WantCaptureMouse && (io.MouseDown[ImGuiMouseButton_Right]))
		{
			mouseYaw += io.MouseDelta.x * 0.003f;
			mousePitch += io.MouseDelta.y * 0.003f;
		}

		const float yawLimit = DirectX::XMConvertToRadians(25.0f);
		const float pitchLimit = DirectX::XMConvertToRadians(10.0f);

		if (mouseYaw > yawLimit)mouseYaw = yawLimit;
		if (mouseYaw < -yawLimit)mouseYaw = -yawLimit;

		if (mousePitch > pitchLimit)mousePitch = pitchLimit;
		if (mousePitch < -pitchLimit)mousePitch = -pitchLimit;

		const float yaw = followYaw + mouseYaw;
		const float pitch = basePitch + mousePitch;

		//注視点からプレイヤーの背後上方へ離す
		const float horizontalDistance =
			cameraDistance * std::cos(pitch);

		eye.x = focus.x - std::sin(yaw) * horizontalDistance;
		eye.y = focus.y + std::sin(pitch) * cameraDistance;
		eye.z = focus.z - std::cos(yaw) * horizontalDistance;

		up = { 0,1,0 };


	}

		//注視点をプレイヤーの位置に設定
	void SetFocus(const DirectX::XMFLOAT3& position)
	{
		targetFocus = position;
		targetFocus.y += focusHeight;
	}

	void DrawGUI() {
	
		if (ImGui::CollapsingHeader("camera", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::DragFloat("cAngleX", &angleX, 0.1f);
			ImGui::DragFloat("cAngleY", &angleY, 0.1f);
		}
	}

	void Reset(const DirectX::XMFLOAT3& playerPosition,
		float playerYaw)
	{
		SetFocus(playerPosition);
		focus = targetFocus;

		followYaw = playerYaw;
		mouseYaw = 0.0f;
		mousePitch = 0.0f;

		const float horizontalDistance =
			cameraDistance * std::cos(basePitch);

		eye = {
			focus.x - std::sin(followYaw) * horizontalDistance,
			focus.y + std::sin(basePitch) * cameraDistance,
			focus.z - std::cos(followYaw) * horizontalDistance
		};

		up = { 0,1,0 };

	}

private:
	DirectX::XMFLOAT3		eye;//視点
	DirectX::XMFLOAT3		focus;//注視点
	DirectX::XMFLOAT3		up;//上方向
	DirectX::XMFLOAT3		right;//右方向
	float					distance;

	float					angleX;
	float					angleY;

	float maxAngleX = DirectX::XMConvertToRadians(70.0f);
	float minAngleX = DirectX::XMConvertToRadians(10.0f);

	DirectX::XMFLOAT3 targetFocus = { 0,0,0 };
	//プレイヤー背後への追従角度
	float followYaw = 0.0f;

	//マウスで調整する角度
	float mouseYaw = 0.0f;
	float mousePitch = 0.0f;

	//調整用
	float followSpeed = 5.0f;
	float turnSpeed = 3.0f;
	float cameraDistance = 100.0f;//playerからの距離
	float focusHeight = 30.0f;//見る位置の高さ
	float basePitch = DirectX::XMConvertToRadians(25.0f);//見下ろす角度

};


// カメラからコントローラーへパラメータを同期する
//void TestCameraController::SyncCameraToController(const Camera& camera)

// コントローラーからカメラへパラメータを同期する
//void TestCameraController::SyncControllerToCamera(Camera& camera)


// 更新処理
//void TestCameraController::Update()

