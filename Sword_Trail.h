#pragma once
#include <DirectXMath.h>


class Sword_Trail
{
public:
	Sword_Trail();
	~Sword_Trail();

	// XVˆ—
	void Update(const DirectX::XMFLOAT4X4& weaponTransform);

	// •`‰æˆ—
	void Render(float elapsedTime) ;

private:
	bool  spline = false;
};
