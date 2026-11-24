#include <algorithm>
#include "Misc.h"
#include "GpuResourceUtils.h"
#include "ModelRenderer.h"
#include "BasicShader.h"
#include "LambertShader.h"

// コンストラクタ
ModelRenderer::ModelRenderer(ID3D11Device* device)
{
	// シーン用定数バッファ
	GpuResourceUtils::CreateConstantBuffer(
		device,
		sizeof(CbScene),
		sceneConstantBuffer.GetAddressOf());

	// スケルトン用定数バッファ
	GpuResourceUtils::CreateConstantBuffer(
		device,
		sizeof(CbSkeleton),
		skeletonConstantBuffer.GetAddressOf());

	// シェーダー生成
	shaders[static_cast<int>(ShaderId::Basic)] = std::make_unique<BasicShader>(device);
	shaders[static_cast<int>(ShaderId::Lambert)] = std::make_unique<LambertShader>(device);

	//シェーダー読み込み
	GpuResourceUtils::LoadVertexShader(device, "Data/Shader/MotionBlurVelocity_vs.cso",
						ModelResource::InputElementDescs.data(), static_cast<UINT>
						(ModelResource::InputElementDescs.size()), velocityInputLayout.GetAddressOf(), velocityVertexShader.GetAddressOf());

	GpuResourceUtils::LoadPixelShader(device,"Data/Shader/MotionBlurVelocity_ps.cso",
											velocityPixelShader.GetAddressOf());

	//定数バッファ生成
	GpuResourceUtils::CreateConstantBuffer(device,sizeof(CbMotionBlur),
												motionBlurConstantBuffer.GetAddressOf());

	GpuResourceUtils::CreateConstantBuffer(device,sizeof(CbPreviousSkeleton),
										previousSkeletonConstantBuffer.GetAddressOf());
}

// 描画実行
void ModelRenderer::Render(const RenderContext& rc, const DirectX::XMFLOAT4X4& worldTransform, const Model* model, ShaderId shaderId)
{
	ID3D11DeviceContext* dc = rc.deviceContext;

	// シーン用定数バッファ更新
	{
		static LightManager defaultLightManager;
		const LightManager* lightManager = rc.lightManager ? rc.lightManager : &defaultLightManager;

		CbScene cbScene{};
		DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&rc.camera->GetView());
		DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&rc.camera->GetProjection());
		DirectX::XMStoreFloat4x4(&cbScene.viewProjection, V * P);
		const DirectionalLight& directionalLight = lightManager->GetDirectionalLight();
		cbScene.lightDirection.x = directionalLight.direction.x;
		cbScene.lightDirection.y = directionalLight.direction.y;
		cbScene.lightDirection.z = directionalLight.direction.z;
		cbScene.lightColor.x = directionalLight.color.x;
		cbScene.lightColor.y = directionalLight.color.y;
		cbScene.lightColor.z = directionalLight.color.z;
		const DirectX::XMFLOAT3& eye = rc.camera->GetEye();
		cbScene.cameraPosition.x = eye.x;
		cbScene.cameraPosition.y = eye.y;
		cbScene.cameraPosition.z = eye.z;
		dc->UpdateSubresource(sceneConstantBuffer.Get(), 0, 0, &cbScene, 0, 0);
	}

	// 定数バッファ設定
	ID3D11Buffer* vsConstantBuffers[] =
	{
		sceneConstantBuffer.Get(),
		skeletonConstantBuffer.Get(),
	};
	dc->VSSetConstantBuffers(0, _countof(vsConstantBuffers), vsConstantBuffers);

	ID3D11Buffer* psConstantBuffers[] =
	{
		sceneConstantBuffer.Get(),
	};
	dc->PSSetConstantBuffers(0, _countof(psConstantBuffers), psConstantBuffers);

	// サンプラステート設定
	ID3D11SamplerState* samplerStates[] =
	{
		rc.renderState->GetSamplerState(SamplerState::LinearWrap)
	};
	dc->PSSetSamplers(0, _countof(samplerStates), samplerStates);

	// レンダーステート設定
	dc->OMSetDepthStencilState(rc.renderState->GetDepthStencilState(DepthState::TestAndWrite), 0);
	dc->RSSetState(rc.renderState->GetRasterizerState(RasterizerState::SolidCullBack));

	// ブレンドステート設定
	dc->OMSetBlendState(rc.renderState->GetBlendState(BlendState::Transparency), nullptr, 0xFFFFFFFF);

	// 描画処理
	Shader* shader = shaders[static_cast<int>(shaderId)].get();
	shader->Begin(rc);

	DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&worldTransform);

	const ModelResource* resource = model->GetResource();
	for (const ModelResource::Mesh& mesh : resource->GetMeshes())
	{
		// 頂点バッファ設定
		UINT stride = sizeof(ModelResource::Vertex);
		UINT offset = 0;
		dc->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
		dc->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// スケルトン用定数バッファ更新
		CbSkeleton cbSkeleton{};
		if (mesh.nodeIndices.size() > 0)
		{
			for (size_t i = 0; i < mesh.nodeIndices.size(); ++i)
			{
				int nodeIndex = mesh.nodeIndices.at(i);
				const Model::Node& node = model->GetNodes().at(nodeIndex);
				DirectX::XMMATRIX GlobalTransform = DirectX::XMLoadFloat4x4(&node.globalTransform);
				DirectX::XMMATRIX OffsetTransform = DirectX::XMLoadFloat4x4(&mesh.offsetTransforms.at(i));
				DirectX::XMMATRIX BoneTransform = OffsetTransform * GlobalTransform * WorldTransform;
				DirectX::XMStoreFloat4x4(&cbSkeleton.boneTransforms[i], BoneTransform);
			}
		}
		else
		{
			const Model::Node& node = model->GetNodes().at(mesh.nodeIndex);
			DirectX::XMMATRIX GlobalTransform = DirectX::XMLoadFloat4x4(&node.globalTransform);
			DirectX::XMMATRIX BoneTransform = GlobalTransform * WorldTransform;
			DirectX::XMStoreFloat4x4(&cbSkeleton.boneTransforms[0], BoneTransform);
		}
		dc->UpdateSubresource(skeletonConstantBuffer.Get(), 0, 0, &cbSkeleton, 0, 0);

		


		// 描画
		for (const ModelResource::Subset& subset : mesh.subsets)
		{
			shader->Update(rc, *subset.material);

			dc->DrawIndexed(subset.indexCount, subset.startIndex, 0);
		}
	}

	shader->End(rc);

	// 定数バッファ設定解除
	for (ID3D11Buffer*& vsConstantBuffer : vsConstantBuffers) { vsConstantBuffer = nullptr; }
	for (ID3D11Buffer*& psConstantBuffer : psConstantBuffers) { psConstantBuffer = nullptr; }
	dc->VSSetConstantBuffers(0, _countof(vsConstantBuffers), vsConstantBuffers);
	dc->PSSetConstantBuffers(0, _countof(psConstantBuffers), psConstantBuffers);

	// サンプラステート設定解除
	for (ID3D11SamplerState*& samplerState : samplerStates) { samplerState = nullptr; }
	dc->PSSetSamplers(0, _countof(samplerStates), samplerStates);
}

void ModelRenderer::RenderVelocity(const RenderContext& rc, const DirectX::XMFLOAT4X4& worldTransform, const Model* model,
	const DirectX::XMFLOAT4X4& previousViewProjection)
{

	if (!hasPreviousSkeleton)return;
	if (model == nullptr||rc.camera==nullptr)return;

	ID3D11DeviceContext* dc = rc.deviceContext;

	if (dc == nullptr)return;

	//シェーダー設定
	dc->IASetInputLayout(velocityInputLayout.Get());

	dc->VSSetShader(velocityVertexShader.Get(), nullptr, 0);
	dc->PSSetShader(velocityPixelShader.Get(), nullptr, 0);

	//現在・前フレームのViewProjection
	CbMotionBlur cbMotionBlur{};

	DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&rc.camera->GetView());
	DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&rc.camera->GetProjection());

	DirectX::XMStoreFloat4x4(&cbMotionBlur.currentViewProjection, V * P);

	cbMotionBlur.previousViewProjection = previousViewProjection;

	dc->UpdateSubresource(motionBlurConstantBuffer.Get(), 0, nullptr,
														&cbMotionBlur, 0, 0);

	ID3D11Buffer* motionBlurCB = motionBlurConstantBuffer.Get();

	dc->VSSetConstantBuffers(0, 1, &motionBlurCB);

	


	DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&worldTransform);



	const ModelResource* resource = model->GetResource();

	if (resource == nullptr)return;

	const auto& meshes = resource->GetMeshes();

	if (previousMeshSkeletons.size() != meshes.size())return;


	//描画
	for (size_t meshIndex = 0; meshIndex < meshes.size(); ++meshIndex)
	{
		const ModelResource::Mesh& mesh = meshes[meshIndex];

		// 前フレームのMesh別ボーン行列を取得
		const auto& previousBones = previousMeshSkeletons[meshIndex].boneTransforms;

		// ボーン数チェック
		const size_t currentBoneCount = mesh.nodeIndices.empty()? 1: mesh.nodeIndices.size();

		if (previousBones.size() != currentBoneCount || currentBoneCount > 256)
		{
			return;
		}

		// 前フレーム用定数バッファ
		CbPreviousSkeleton cbPrevious{};

		for (size_t i = 0; i < previousBones.size(); ++i)
		{
			cbPrevious.boneTransforms[i] = previousBones[i];
		}

		dc->UpdateSubresource(previousSkeletonConstantBuffer.Get(),0,nullptr,&cbPrevious,0,0);

		ID3D11Buffer* previousCB =previousSkeletonConstantBuffer.Get();

		dc->VSSetConstantBuffers(
			4,1,
			&previousCB);



		// 頂点バッファ
		UINT stride = sizeof(ModelResource::Vertex);
		UINT offset = 0;

		dc->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);

		//インデックスバッファ
		dc->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//現在のボーン行列
		CbSkeleton cbSkeleton{};

		if (!mesh.nodeIndices.empty())
		{
			if (mesh.nodeIndices.size() > 256 ||mesh.offsetTransforms.size() < mesh.nodeIndices.size())
			{
				continue;
			}

			for (size_t i = 0; i < mesh.nodeIndices.size();++i)
			{
				int nodeIndex = mesh.nodeIndices.at(i);


				if (nodeIndex < 0 || static_cast<size_t>(nodeIndex) >= model->GetNodes().size())
				{
					continue;
				}

				const Model::Node& node = model->GetNodes().at(nodeIndex);


				DirectX::XMMATRIX globalTransform = DirectX::XMLoadFloat4x4(&node.globalTransform);
				DirectX::XMMATRIX offsetTransform = DirectX::XMLoadFloat4x4(&mesh.offsetTransforms[i]);
				DirectX::XMMATRIX boneTransform = offsetTransform * globalTransform * WorldTransform;

				DirectX::XMStoreFloat4x4(&cbSkeleton.boneTransforms[i],boneTransform);
			
			}
		}
		else
		{
			const Model::Node& node = model->GetNodes().at(mesh.nodeIndex);

			DirectX::XMMATRIX globalTransform = DirectX::XMLoadFloat4x4(&node.globalTransform);
			DirectX::XMMATRIX boneTransform = globalTransform * DirectX::XMLoadFloat4x4(&worldTransform);

			DirectX::XMStoreFloat4x4(&cbSkeleton.boneTransforms[0],boneTransform);
		}

		//現在のボーン行列をb1に設定
		dc->UpdateSubresource(skeletonConstantBuffer.Get(), 0, nullptr, &cbSkeleton, 0, 0);

		ID3D11Buffer* skeletonCB = skeletonConstantBuffer.Get();

		dc->VSSetConstantBuffers(1, 1, &skeletonCB);


		// 前フレームのボーン行列をb4へ設定
		// ※Meshごとの履歴との対応付けが必要

		// 描画
		for (const ModelResource::Subset& subset : mesh.subsets)
		{
			dc->DrawIndexed(
				subset.indexCount,
				subset.startIndex,
				0);
		}

	}
}

void ModelRenderer::SavePreviousBoneTransforms(const DirectX::XMFLOAT4X4& worldTransform, const Model* model)
{
	if (model == nullptr)return;

	const ModelResource* resource = model->GetResource();

	if (resource == nullptr)return;

	const auto& meshes = resource->GetMeshes();
	const auto& nodes = model->GetNodes();

	previousMeshSkeletons.resize(meshes.size());

	DirectX::XMMATRIX world = DirectX::XMLoadFloat4x4(&worldTransform);

	for (size_t mesh_Index = 0; mesh_Index < meshes.size(); ++mesh_Index)
	{
		const ModelResource::Mesh& mesh = meshes[mesh_Index];

		PreviousMeshSkeleton& history = previousMeshSkeletons[mesh_Index];

		size_t boneCount = mesh.nodeIndices.empty() ? 1 : mesh.nodeIndices.size();
		if (boneCount > 256)return;

		history.boneTransforms.resize(boneCount);

		if (!mesh.nodeIndices.empty())
		{
			if (mesh.offsetTransforms.size() < boneCount)return;

			for (size_t i = 0; i < boneCount; ++i)
			{
				int nodeIndex = mesh.nodeIndices[i];

				if (nodeIndex < 0 || static_cast<size_t>(nodeIndex) >= nodes.size())return;

				DirectX::XMMATRIX global = DirectX::XMLoadFloat4x4(&nodes[nodeIndex].globalTransform);
				DirectX::XMMATRIX offset = DirectX::XMLoadFloat4x4(&mesh.offsetTransforms[i]);
				DirectX::XMMATRIX bone = offset * global * world;

				DirectX::XMStoreFloat4x4(&history.boneTransforms[i], bone);
			}
		}
		else
		{
			if (mesh.nodeIndex < 0 || static_cast<size_t>(mesh.nodeIndex) >= nodes.size())return;

			DirectX::XMMATRIX global = DirectX::XMLoadFloat4x4(&nodes[mesh.nodeIndex].globalTransform);
			DirectX::XMMATRIX bone = global * world;

			DirectX::XMStoreFloat4x4(&history.boneTransforms[0],bone);
		
		}
	}
	hasPreviousSkeleton = true;
}

void ModelRenderer::ResetPreviousBoneTransforms()
{
	previousMeshSkeletons.clear();
	hasPreviousSkeleton = false;
}


