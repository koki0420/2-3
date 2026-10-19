#pragma once

#include"Object.h"

#include"ModelRenderer.h"

class Boss
{
public:
	Boss(DirectX::XMFLOAT3 pos);
	~Boss();

	void Update(float elapsedTime);

	void Render(const RenderContext& immediate_context, ModelRenderer* renderer);

private:
	Object boss;
	DirectX::XMFLOAT3 arrive_position = { 0,0,0 };
	bool alive = true;
	bool arrive = true;
};