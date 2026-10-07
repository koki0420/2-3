#include "Camera.h"
#include <imgui.h>
#include <wtypes.h>
#include <WinUser.h>

// コンストラクタ
Camera::Camera()
{
	// カメラ設定
	SetPerspectiveFov(
		DirectX::XMConvertToRadians(45),	// 画角
		1280.0f / 720.0f,					// 画面アスペクト比
		0.1f,								// ニアクリップ
		1000.0f								// ファークリップ
	);

	eye = { 0,0,-1 };
	focus = { 0,0,0 };

	// ★ 最初だけ向きを逆にしたいならここで 180° 回す ★
	angleY = DirectX::XM_PI*1.5f;   // ← これだけで前後反転

	SetLookAt(
		{ eye.x, eye.y, eye.z },		// 視点
		{ focus.x, focus.y,focus.z },		// 注視点
		{ 0, 1, 0 }			// 上ベクトル
	);
}

// 指定方向を向く
void Camera::SetLookAt(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT3& focus, const DirectX::XMFLOAT3& up)
{
	// 視点、注視点、上方向からビュー行列を作成
	DirectX::XMVECTOR Eye = DirectX::XMLoadFloat3(&eye);
	DirectX::XMVECTOR Focus = DirectX::XMLoadFloat3(&focus);
	DirectX::XMVECTOR Up = DirectX::XMLoadFloat3(&up);
	DirectX::XMMATRIX View = DirectX::XMMatrixLookAtLH(Eye, Focus, Up);
	DirectX::XMStoreFloat4x4(&view, View);

	// ビューを逆行列化し、ワールド行列に戻す
	DirectX::XMMATRIX World = DirectX::XMMatrixInverse(nullptr, View);
	DirectX::XMFLOAT4X4 world;
	DirectX::XMStoreFloat4x4(&world, World);

	// カメラの方向を取り出す
	this->right.x = world._11;
	this->right.y = world._12;
	this->right.z = world._13;

	this->up.x = world._21;
	this->up.y = world._22;
	this->up.z = world._23;

	this->front.x = world._31;
	this->front.y = world._32;
	this->front.z = world._33;

	// 視点、注視点を保存
	this->eye = eye;
	this->focus = focus;
}


void Camera::Update()
{
	
	float sx = sinf(angleX);
	float cx = cosf(angleX);
	float sy = sinf(angleY);
	float cy = cosf(angleY);

	DirectX::XMFLOAT3 forward = {
		cx * sy,
		sx,
		cx * cy
	};

	focus = {
		eye.x + forward.x,
		eye.y + forward.y,
		eye.z + forward.z
	};

	if (ImGui::Begin("Camera"))
	{
		// カメラ位置
		ImGui::DragFloat3("Eye", &eye.x, 0.1f);

		// カメラ角度（ラジアン）
		ImGui::DragFloat("Pitch (X)", &angleX, 0.01f);
		ImGui::DragFloat("Yaw (Y)", &angleY, 0.01f);

		// 角度を度数法で表示したい場合
		float degX = DirectX::XMConvertToDegrees(angleX);
		float degY = DirectX::XMConvertToDegrees(angleY);
		ImGui::Text("Pitch(deg): %.1f", degX);
		ImGui::Text("Yaw(deg): %.1f", degY);
	}
	ImGui::End();

	SetLookAt(eye, focus, { 0,1,0 });
	

}



// パースペクティブ設定
void Camera::SetPerspectiveFov(float fovY, float aspect, float nearZ, float farZ)
{
	// 画角、画面比率、クリップ距離からプロジェクション行列を作成
	DirectX::XMMATRIX Projection = DirectX::XMMatrixPerspectiveFovLH(fovY, aspect, nearZ, farZ);
	DirectX::XMStoreFloat4x4(&projection, Projection);
}


