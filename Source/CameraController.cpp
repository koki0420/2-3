#include"CameraController.h"


// カメラからコントローラーへパラメータを同期する
void CameraController::SyncCameraToController(const Camera& camera)
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
void CameraController::SyncControllerToCamera(Camera& camera)
{
	camera.SetLookAt(eye, focus, up);
}

// 更新処理
void CameraController::Update(float elapsedTime, HWND hwnd)
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

	if (Blood)
	{
		Blood_time -= 1.0f*elapsedTime;

		float shake = Blood_time * 0.5f;

		eye.x += cosf(Blood_time * 20.0f) * shake;
		eye.y += sinf(Blood_time * 20.0f) * shake;
		
		
	}

}
//注視点をプレイヤーの位置に設定
void CameraController::SetFocus(const DirectX::XMFLOAT3& position)
{
	targetFocus = position;
	targetFocus.y += focusHeight;
}

void CameraController::DrawGUI() {

	if (ImGui::CollapsingHeader("camera", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::DragFloat("cAngleX", &angleX, 0.1f);
		ImGui::DragFloat("cAngleY", &angleY, 0.1f);
	}
}

void CameraController::Reset(const DirectX::XMFLOAT3& playerPosition,
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
