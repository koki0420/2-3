#pragma once

#include <imgui.h>
#include "Camera.h"
#include <Windows.h>
#include <cmath>


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
	void Update(float elapsedTime,HWND hwnd)
	{

		//HWND hwnd
		const bool gameActive = hwnd != nullptr && GetForegroundWindow() == hwnd;
		const bool tabDown = (GetAsyncKeyState(VK_TAB) & 0x8000) != 0;

		if (gameActive && tabDown && !previousTabDown) { showCursor = !showCursor; };

		previousTabDown = tabDown;

		if (gameActive && !showCursor)
		{
			ImGui::SetMouseCursor(ImGuiMouseCursor_None);
			SetCursor(nullptr);

			RECT rect{};
			if (GetClientRect(hwnd, &rect) && rect.right > 0 && rect.bottom > 0)
			{
				POINT center{ rect.right / 2,rect.bottom / 2 };
				POINT cursor{};

				//ウィンドウ内の中央座標を、画面全体の座標に変換する
				if (ClientToScreen(hwnd, &center) && GetCursorPos(&cursor))
				{
					//非表示に切り替えた最初のフレームは中央に戻すだけ
					if (mouseCentered)
					{
						followYaw += (cursor.x - center.x) * 0.003f;
						mousePitch += (cursor.y - center.y) * 0.003f;
					}
					mouseCentered = SetCursorPos(center.x, center.y) != FALSE;
				}
				else
				{
					mouseCentered = false;
				}
			}
			else
			{
				//tabで表示したときや、別ウィンドウへ移った時
				mouseCentered = false;
			}
		}

		//ゆっくり追いかける
		const float rate =
			1.0f - std::exp(-followSpeed * elapsedTime);

		focus.x += (targetFocus.x - focus.x) * rate;
		focus.y += (targetFocus.y - focus.y) * rate;
		focus.z += (targetFocus.z - focus.z) * rate;

		// IMGUIのマウス入力値を使ってカメラ操作する
		const ImGuiIO io = ImGui::GetIO();

		// カメラの向きはマウスで決める
		if (!io.WantCaptureMouse)
		{
			followYaw += io.MouseDelta.x * 0.003f;
			mousePitch += io.MouseDelta.y * 0.003f;
		}

		//左右は360度回せる。角度の値だけを－πから－πに収める
		followYaw = DirectX::XMScalarModAngle(followYaw);
		const float yaw = followYaw;
		float pitch = basePitch + mousePitch;

		//真上や真下を向かないようにする
		const float minPitch = DirectX::XMConvertToRadians(10.0f);
		const float maxPitch = DirectX::XMConvertToRadians(70.0f);

		if (pitch < minPitch)pitch = minPitch;
		if (pitch > maxPitch)pitch = maxPitch;
		mousePitch = pitch - basePitch;

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

	//ka-soru
	bool showCursor = false;
	bool previousTabDown = false;
	bool mouseCentered = false;

};


// カメラからコントローラーへパラメータを同期する
//void TestCameraController::SyncCameraToController(const Camera& camera)

// コントローラーからカメラへパラメータを同期する
//void TestCameraController::SyncControllerToCamera(Camera& camera)


// 更新処理
//void TestCameraController::Update()

