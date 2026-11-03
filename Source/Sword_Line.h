#pragma once
#include <DirectXMath.h>
#include "RenderContext.h"

class Sword_Line
{
public:
	

	// XVˆ—
	void Update(const DirectX::XMFLOAT4X4& weaponTransform);

	// •`‰æˆ—
	void Render(const RenderContext& rc);

private:
	bool  spline = false;

	static constexpr int MAX_POLYGON = 32;

	DirectX::XMFLOAT3 rootHistory[MAX_POLYGON];
	DirectX::XMFLOAT3 tipHistory[MAX_POLYGON];
};
