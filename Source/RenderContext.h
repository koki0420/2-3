#pragma once

#include "Camera.h"
#include "RenderState.h"
#include "Light.h"

struct RenderContext
{
	ID3D11DeviceContext*	deviceContext;
	const RenderState*		renderState;
	const Camera*			camera;
	const LightManager*		lightManager = nullptr;

	DirectX::XMFLOAT4X4 viewProjection;

	bool high_light = false;
	DirectX::XMFLOAT4 highlightColor =	{1,1,1,1};
};
