#include <imgui.h>
#include "FreeCameraController.h"
#include <wtypes.h>

// カメラからコントローラーへパラメータを同期する
void FreeCameraController::SyncCameraToController(const Camera& camera)
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

}

// コントローラーからカメラへパラメータを同期する
void FreeCameraController::SyncControllerToCamera(Camera& camera)
{
	camera.SetLookAt(eye, focus, up);
}


void FreeCameraController::Update()
{
	// デバッグウインドウ操作中は処理しない
	if (ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow))
	{
		return;
	}

	// FPS モード：カーソル非表示 + ウィンドウ内に閉じ込める
	{
		ShowCursor(FALSE);

		HWND hwnd = GetActiveWindow();
		RECT rect;
		GetClientRect(hwnd, &rect);

		POINT lt = { rect.left, rect.top };
		POINT rb = { rect.right, rect.bottom };

		ClientToScreen(hwnd, &lt);
		ClientToScreen(hwnd, &rb);

		RECT clip = { lt.x, lt.y, rb.x, rb.y };
		ClipCursor(&clip);
	}

	// ImGui のマウス相対移動量を使う（FPS 方式）
	ImGuiIO& io = ImGui::GetIO();
	float moveX = io.MouseDelta.x * 0.02f;
	float moveY = io.MouseDelta.y * 0.02f;

	// Y軸回転
	angleY += moveX * 0.5f;
	if (angleY > DirectX::XM_PI) angleY -= DirectX::XM_2PI;
	else if (angleY < -DirectX::XM_PI) angleY += DirectX::XM_2PI;

	// X軸回転
	angleX += moveY * 0.5f;
	if (angleX > DirectX::XM_PI) angleX -= DirectX::XM_2PI;
	else if (angleX < -DirectX::XM_PI) angleX += DirectX::XM_2PI;

	float sx = ::sinf(angleX);
	float cx = ::cosf(angleX);
	float sy = ::sinf(angleY);
	float cy = ::cosf(angleY);

	DirectX::XMVECTOR Front = DirectX::XMVectorSet(-cx * sy, -sx, -cx * cy, 0.0f);
	DirectX::XMVECTOR Right = DirectX::XMVectorSet(cy, 0, -sy, 0.0f);
	DirectX::XMVECTOR Up = DirectX::XMVector3Cross(Right, Front);

	DirectX::XMVECTOR Focus = DirectX::XMLoadFloat3(&focus);
	DirectX::XMVECTOR Distance = DirectX::XMVectorSet(distance, distance, distance, 0.0f);
	DirectX::XMVECTOR Eye = DirectX::XMVectorSubtract(Focus, DirectX::XMVectorMultiply(Front, Distance));

	DirectX::XMMATRIX View = DirectX::XMMatrixLookAtLH(Eye, Focus, Up);
	DirectX::XMMATRIX World = DirectX::XMMatrixTranspose(View);

	Right = DirectX::XMVector3TransformNormal(DirectX::XMVectorSet(1, 0, 0, 0), World);
	Up = DirectX::XMVector3TransformNormal(DirectX::XMVectorSet(0, 1, 0, 0), World);

	DirectX::XMStoreFloat3(&eye, Eye);
	DirectX::XMStoreFloat3(&up, Up);
	DirectX::XMStoreFloat3(&right, Right);
}


