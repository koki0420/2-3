#pragma once
#include "Model.h"

// constexprに置換するかも
constexpr int ITEM = 7; //全体の数(種類数ではない)
constexpr int GIMMICK = 7;
constexpr int SOUND_SOURCE_NUM = 7;

struct Object
{
	bool					onGround = false;
	DirectX::XMFLOAT3		velocity = { 0, 0, 0 };
	DirectX::XMFLOAT3		position = { 0, 0, 0 };
	DirectX::XMFLOAT3		angle = { 0, 0, 0 };
	DirectX::XMFLOAT3		scale = { 1, 1, 1 };
	DirectX::XMFLOAT4X4		transform = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
	std::unique_ptr<Model>	model;

	void UpdateTransform()
	{
		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
		DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
		DirectX::XMMATRIX WorldTransform = S * R * T;
		DirectX::XMStoreFloat4x4(&transform, WorldTransform);
	}
	DirectX::XMMATRIX LookRotation(const DirectX::XMFLOAT3& forward)
	{
		using namespace DirectX;

		XMVECTOR f = XMVector3Normalize(XMLoadFloat3(&forward));
		XMVECTOR up = XMVectorSet(0, 1, 0, 0);

		// forward と up が平行に近い場合の対策
		if (fabsf(XMVectorGetX(XMVector3Dot(f, up))) > 0.999f)
		{
			up = XMVectorSet(0, 0, 1, 0);
		}

		XMVECTOR r = XMVector3Normalize(XMVector3Cross(up, f)); // 右方向
		XMVECTOR u = XMVector3Cross(f, r);                      // 正しい上方向

		XMMATRIX m;
		m.r[0] = r; // X軸
		m.r[1] = u; // Y軸
		m.r[2] = f; // Z軸
		m.r[3] = XMVectorSet(0, 0, 0, 1);

		return m;
	}
	void SetRotationFromMatrix(const DirectX::XMMATRIX& m)
	{
		using namespace DirectX;

		// 行列 → Euler（Pitch, Yaw, Roll）
		float pitch, yaw, roll;

		// Z軸が forward の LookRotation なのでこの取り方で OK
		yaw = atan2f(m.r[2].m128_f32[0], m.r[2].m128_f32[2]);
		pitch = -asinf(m.r[2].m128_f32[1]);

		angle = { pitch, yaw, 0 };
	}

};

