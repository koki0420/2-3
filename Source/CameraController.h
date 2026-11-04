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
	void Update()
	{
		// デバッグウインドウ操作中は処理しない
		if (ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow))
		{
			return;
		}

		// IMGUIのマウス入力値を使ってカメラ操作する
		ImGuiIO io = ImGui::GetIO();

		// マウスカーソルの移動量を求める
		float moveX = io.MouseDelta.x * 0.02f;
		float moveY = io.MouseDelta.y * 0.02f;

		//Y軸のカメラ回転を-3.14～3.14に収まるようにする
		if (angleX < -DirectX::XM_PI) { angleY += DirectX::XM_2PI; }
		if (angleX > DirectX::XM_PI) { angleY -= DirectX::XM_2PI; }


		// マウス左ボタン押下中
		if (!(io.MouseDown[ImGuiMouseButton_Right]))
		{
			// Y軸回転
			angleY += moveX * 0.5f;
			if (angleY > DirectX::XM_PI)
			{
				angleY -= DirectX::XM_2PI;
			}
			else if (angleY < -DirectX::XM_PI)
			{
				angleY += DirectX::XM_2PI;
			}
			// X軸回転
			angleX += moveY * 0.5f;
			if (angleX > DirectX::XM_PI)
			{
				angleX -= DirectX::XM_2PI;
			}
			else if (angleX < -DirectX::XM_PI)
			{
				angleX += DirectX::XM_2PI;
			}
		}
		
		//仮
		//カメラ距離固定
		distance = 100.0f;

		//X軸のカメラ回転を制限
		if (angleX > maxAngleX) { angleX = maxAngleX; }
		if (angleX < minAngleX) { angleX = minAngleX; }

		//近づきすぎを防ぐ
		if (distance < 1.0f) { distance = 1.0f; }

		float sx = ::sinf(angleX);
		float cx = ::cosf(angleX);
		float sy = ::sinf(angleY);
		float cy = ::cosf(angleY);

		

		// カメラの方向を算出
		DirectX::XMVECTOR Front = DirectX::XMVectorSet(-cx * sy, -sx, -cx * cy, 0.0f);
		DirectX::XMVECTOR Right = DirectX::XMVectorSet(cy, 0, -sy, 0.0f);
		DirectX::XMVECTOR Up = DirectX::XMVector3Cross(Right, Front);
		// カメラの視点＆注視点を算出
		DirectX::XMVECTOR Focus = DirectX::XMLoadFloat3(&focus);
		DirectX::XMVECTOR Distance = DirectX::XMVectorSet(distance, distance, distance, 0.0f);
		DirectX::XMVECTOR Eye = DirectX::XMVectorSubtract(Focus, DirectX::XMVectorMultiply(Front, Distance));
		// ビュー行列からワールド行列を算出
		DirectX::XMMATRIX View = DirectX::XMMatrixLookAtLH(Eye, Focus, Up);
		DirectX::XMMATRIX World = DirectX::XMMatrixTranspose(View);
		// ワールド行列から方向を算出
		Right = DirectX::XMVectorSet(1, 0, 0, 0);
		Up = DirectX::XMVectorSet(0, 1, 0, 0);
		// 結果を格納
		DirectX::XMStoreFloat3(&eye, Eye);
		DirectX::XMStoreFloat3(&up, Up);
		DirectX::XMStoreFloat3(&right, Right);
		
		}

		//注視点をプレイヤーの位置に設定
		void SetFocus(const DirectX::XMFLOAT3& position) { focus = position; }

	void DrawGUI() {
	
		if (ImGui::CollapsingHeader("camera", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::DragFloat("cAngleX", &angleX, 0.1f);
			ImGui::DragFloat("cAngleY", &angleY, 0.1f);
		}
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
};


// カメラからコントローラーへパラメータを同期する
//void TestCameraController::SyncCameraToController(const Camera& camera)

// コントローラーからカメラへパラメータを同期する
//void TestCameraController::SyncControllerToCamera(Camera& camera)


// 更新処理
//void TestCameraController::Update()

