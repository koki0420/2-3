#pragma once

#include <DirectXMath.h>
#include "Model.h"

// コリジョン
class Collision
{

public:
	struct SphereCastResult
	{
		DirectX::XMVECTOR	position = { 0, 0, 0 };	// レイとポリゴンの交点
		DirectX::XMVECTOR	normal = { 0, 0, 0 };	// 衝突したポリゴンの法線ベクトル
		float				distance = 0.0f; 		// レイの始点から交点までの距離
		DirectX::XMVECTOR	verts[3];
		int					materialIndex = -1; 	// 衝突したポリゴンのマテリアル番号
	};
	// レイキャスト
	static bool RayCast(
		const DirectX::XMFLOAT3& start,
		const DirectX::XMFLOAT3& end,
		const DirectX::XMFLOAT4X4& worldTransform,
		const Model* model,
		DirectX::XMFLOAT3& hitPosition,
		DirectX::XMFLOAT3& hitNormal);

	static bool sphereVssphere(
		const DirectX::XMFLOAT3& pos,
		float radius,
		const DirectX::XMFLOAT3& _pos,
		float _radius
	);

	// レイVs球
	static bool IntersectRayVsSphere(
		const DirectX::XMFLOAT3& start,
		const DirectX::XMFLOAT3& end,
		const DirectX::XMFLOAT3& spherePos,
		const float radius,
		SphereCastResult* result = {});

};
