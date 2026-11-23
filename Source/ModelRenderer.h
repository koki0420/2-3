#pragma once

#include <memory>
#include <vector>
#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include "Model.h"
#include "Shader.h"

enum class ShaderId
{
	Basic,
	Lambert,

	EnumCount
};

class ModelRenderer
{
public:
	ModelRenderer(ID3D11Device* device);
	~ModelRenderer() {}

	// 描画実行
	void Render(const RenderContext& rc, const DirectX::XMFLOAT4X4& worldTransform, const Model* model, ShaderId shaderId);


	// モーションブラー用速度描画
	void RenderVelocity(const RenderContext& rc,const DirectX::XMFLOAT4X4& worldTransform,
		const Model* model,const DirectX::XMFLOAT4X4& previousViewProjection);

	void SavePreviousBoneTransforms(const DirectX::XMFLOAT4X4& worldTransform,const Model* model);

	void ResetPreviousBoneTransforms();

private:
	struct CbScene
	{
		DirectX::XMFLOAT4X4		viewProjection;
		DirectX::XMFLOAT4		lightDirection;
		DirectX::XMFLOAT4		lightColor;
		DirectX::XMFLOAT4		cameraPosition;
	};

	struct CbSkeleton
	{
		DirectX::XMFLOAT4X4		boneTransforms[256];
	};

	std::unique_ptr<Shader>					shaders[static_cast<int>(ShaderId::EnumCount)];

	Microsoft::WRL::ComPtr<ID3D11Buffer>	sceneConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>	skeletonConstantBuffer;

	//モーションブラー
	struct CbPreviousSkeleton
	{
		DirectX::XMFLOAT4X4 boneTransforms[256];
	};

	
	struct CbMotionBlur
	{
		DirectX::XMFLOAT4X4 currentViewProjection;
		DirectX::XMFLOAT4X4 previousViewProjection;
	};

	struct PreviousMeshSkeleton
	{
		std::vector<DirectX::XMFLOAT4X4> boneTransforms;
	};
	std::vector<PreviousMeshSkeleton> previousMeshSkeletons;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> velocityVertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>  velocityPixelShader;
	Microsoft::WRL::ComPtr<ID3D11Buffer> motionBlurConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> previousSkeletonConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> velocityInputLayout;

	bool hasPreviousSkeleton = false;
};
