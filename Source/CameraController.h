#pragma once

#include <imgui.h>
#include "Camera.h"
#include <Windows.h>
#include <cmath>


class CameraController
{
public:
	// カメラからコントローラーへパラメータを同期する
	void SyncCameraToController(const Camera& camera);
	


	// コントローラーからカメラへパラメータを同期する
	void SyncControllerToCamera(Camera& camera);

	// 更新処理
	void Update(float elapsedTime, HWND hwnd);
	
		//注視点をプレイヤーの位置に設定
	void SetFocus(const DirectX::XMFLOAT3& position);

	void DrawGUI();

	void Reset(const DirectX::XMFLOAT3& playerPosition,
		float playerYaw);

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

