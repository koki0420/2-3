#pragma once

#include"Object.h"

#include"ModelRenderer.h"

class Boss
{
public:
	Boss();
	~Boss();

	void Update(float elapsedTime);

	void Render(const RenderContext& immediate_context, ModelRenderer* renderer);

private:
	Object boss;
};