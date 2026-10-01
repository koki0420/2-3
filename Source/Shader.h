#pragma once

#include "RenderContext.h"
#include "Model.h"

#include <d3d11.h>


#include "Misc.h"

#include <memory>
using namespace std;


class Shader
{
public:
	Shader() {}
	virtual ~Shader() {}

	// 開始処理
	virtual void Begin(const RenderContext& rc) = 0;

	// 更新処理
	virtual void Update(const RenderContext& rc, const ModelResource::Material& material) = 0;

	// 終了処理
	virtual void End(const RenderContext& rc) = 0;
};



HRESULT create_vs_from_cso(ID3D11Device* device, const char* cso_name, ID3D11VertexShader** vertex_shader, ID3D11InputLayout** input_layout, D3D11_INPUT_ELEMENT_DESC* input_element_desc, UINT num_elements);


HRESULT create_ps_from_cso(ID3D11Device* device, const char* cso_name, ID3D11PixelShader** pixel_shader);
