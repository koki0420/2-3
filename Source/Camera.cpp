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


	// ★ 最初だけ向きを逆にしたいならここで 180° 回す ★
	angleY = DirectX::XM_PI*1.5f;   // ← これだけで前後反転

	SetLookAt(
		{ 0, 0, -5 },		// 視点
		{ 0, 0, 0 },		// 注視点
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


void Camera::Update(DirectX::XMFLOAT3 pos,bool player)
{
	//頭
	pos.y += 1.6f;
	

	// --- マウス中央固定 + 相対移動量取得 ---
	HWND hwnd = GetActiveWindow();

	RECT rc;
	GetClientRect(hwnd, &rc);

	POINT center;
	center.x = (rc.right - rc.left) / 2;
	center.y = (rc.bottom - rc.top) / 2;

	// 中央のスクリーン座標
	POINT screenCenter = center;
	ClientToScreen(hwnd, &screenCenter);

	// 現在のマウス位置
	POINT mouse;
	GetCursorPos(&mouse);

	// 中央との差分（FPS カメラの回転量）
	float dx = float(mouse.x - screenCenter.x);
	float dy = float(mouse.y - screenCenter.y);

	
	// 中央に戻す
	
	SetCursorPos(screenCenter.x, screenCenter.y);
	if (player)
	{
		// 感度
		float sensitivity = 0.002f;

		angleY += dx * sensitivity;
		angleX -= dy * sensitivity;

		// ピッチ制限（上下向きすぎ防止）
		constexpr float limit = DirectX::XMConvertToRadians(5.0f);
		if (angleX > limit) angleX = limit;
		if (angleX < -limit) angleX = -limit;
	}
	// forward 再計算
	float sx = sinf(angleX);
	float cx = cosf(angleX);
	float sy = sinf(angleY);
	float cy = cosf(angleY);

	DirectX::XMFLOAT3 forward = {
		cx * sy,
		sx,
		cx * cy
	};

	DirectX::XMFLOAT3 target = {
		pos.x + forward.x,
		pos.y + forward.y,
		pos.z + forward.z
	};

	SetLookAt(pos, target, { 0,1,0 });
}



// パースペクティブ設定
void Camera::SetPerspectiveFov(float fovY, float aspect, float nearZ, float farZ)
{
	// 画角、画面比率、クリップ距離からプロジェクション行列を作成
	DirectX::XMMATRIX Projection = DirectX::XMMatrixPerspectiveFovLH(fovY, aspect, nearZ, farZ);
	DirectX::XMStoreFloat4x4(&projection, Projection);
}


