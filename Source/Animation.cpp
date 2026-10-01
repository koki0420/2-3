#include <imgui.h>
#include <ImGuizmo.h>
#include "Graphics.h"
#include "Animation.h"

// コンストラクタ
Animation::Animation(Model& M)
{
	//ID3D11Device* device = Graphics::Instance().GetDevice();
	//float screenWidth = Graphics::Instance().GetScreenWidth();
	//float screenHeight = Graphics::Instance().GetScreenHeight();

	//// カメラ設定
	//camera.SetPerspectiveFov(
	//	DirectX::XMConvertToRadians(45),	// 画角
	//	screenWidth / screenHeight,			// 画面アスペクト比
	//	0.1f,								// ニアクリップ
	//	1000.0f								// ファークリップ
	//);
	//camera.SetLookAt(
	//	{ 0, 5, 7 },		// 視点
	//	{ 0, 0, 0 },		// 注視点
	//	{ 0, 1, 0 }			// 上ベクトル
	//);
	//cameraController.SyncCameraToController(camera);


	model = &M;
}


// 更新処理
void Animation::Update(float elapsedTime)
{
	// アニメーション更新処理
	UpdateAnimation(elapsedTime);
}



// アニメーション再生
void Animation::PlayAnimation(int index, bool loop)
{
	animationPlaying = true;
	animationLoop = loop;
	animationIndex = index;
	animationSeconds = 0.0f;
}

void Animation::PlayAnimation(const char* name, bool loop)
{
	int index = 0;
	const std::vector<ModelResource::Animation>& animations = model->GetResource()->GetAnimations();
	for (const ModelResource::Animation& animation : animations)
	{
		if (animation.name == name)
		{
			PlayAnimation(index, loop);
			return;
		}
		++index;
	}
}


// アニメーション更新処理
void Animation::UpdateAnimation(float elapsedTime)
{
	if (animationPlaying)
	{

		std::vector<Model::Node>& nodes = model->GetNodes();

		const std::vector<ModelResource::Animation>& animations = model->GetResource()->GetAnimations();
		const ModelResource::Animation& animation = animations.at(animationIndex);

		//時間経過
		animationSeconds += elapsedTime;

		//再生時間が終端時間をこえたら
		if (animationSeconds >= animation.secondsLength)
		{
			if (animationLoop)
			{
				animationSeconds = 0;
			}
			else
			{
				animationSeconds = animation.secondsLength;
				return;
			}
		}

		float blendRate = 1.0f;
		if (animationSeconds < animationBlendSecondsLength)
		{
			blendRate = animationSeconds / animationBlendSecondsLength;
		}

		//アニメーションデータからキーフレームデータリストを取得
		const std::vector<ModelResource::Keyframe>& keyframes = animation.keyframes;
		int keyCount = static_cast<int>(keyframes.size());
		for (int keyIndex = 0;keyIndex < keyCount - 1;++keyIndex)
		{
			const ModelResource::Keyframe& keyframe0 = keyframes.at(keyIndex);
			const ModelResource::Keyframe& keyframe1 = keyframes.at(keyIndex + 1);
			if (animationSeconds >= keyframe0.seconds && animationSeconds < keyframe1.seconds)
			{
				float rate = (animationSeconds - keyframe0.seconds) /
					(keyframe1.seconds - keyframe0.seconds);
				int nodeCount = static_cast<int>(nodes.size());
				for (int nodeIndex = 0;nodeIndex < nodeCount;++nodeIndex)
				{
					//キーフレームデータ取得
					const ModelResource::NodeKeyData& key0 = keyframe0.nodeKeys.at(nodeIndex);
					const ModelResource::NodeKeyData& key1 = keyframe1.nodeKeys.at(nodeIndex);

					//ノード取得
					Model::Node& node = nodes[nodeIndex];

					if (blendRate < 1.0f)
					{
						//前のキーフレームと次のキーフレームの姿勢を補完
						DirectX::XMVECTOR S0 = DirectX::XMLoadFloat3(&node.scale);
						DirectX::XMVECTOR S1 = DirectX::XMVectorLerp(DirectX::XMLoadFloat3(&key0.scale), DirectX::XMLoadFloat3(&key1.scale), rate);

						DirectX::XMVECTOR R0 = DirectX::XMLoadFloat4(&node.rotate);
						DirectX::XMVECTOR R1 = DirectX::XMQuaternionSlerp(XMLoadFloat4(&key0.rotate), XMLoadFloat4(&key1.rotate), rate);

						DirectX::XMVECTOR T0 = DirectX::XMLoadFloat3(&node.translate);
						DirectX::XMVECTOR T1 = DirectX::XMVectorLerp(XMLoadFloat3(&key0.translate), XMLoadFloat3(&key1.translate), rate);

						DirectX::XMVECTOR S = DirectX::XMVectorLerp(S0, S1, blendRate);
						DirectX::XMVECTOR R = DirectX::XMQuaternionSlerp(R0, R1, blendRate);;
						DirectX::XMVECTOR T = DirectX::XMVectorLerp(T0, T1, blendRate);;

						DirectX::XMStoreFloat3(&node.scale, S);
						DirectX::XMStoreFloat4(&node.rotate, R);
						DirectX::XMStoreFloat3(&node.translate, T);
					}
					else
					{
						//前のキーフレームと次のキーフレームの姿勢を補完
						DirectX::XMVECTOR S0 = DirectX::XMLoadFloat3(&key0.scale);
						DirectX::XMVECTOR S1 = DirectX::XMLoadFloat3(&key1.scale);

						DirectX::XMVECTOR R0 = DirectX::XMLoadFloat4(&key0.rotate);
						DirectX::XMVECTOR R1 = DirectX::XMLoadFloat4(&key1.rotate);

						DirectX::XMVECTOR T0 = DirectX::XMLoadFloat3(&key0.translate);
						DirectX::XMVECTOR T1 = DirectX::XMLoadFloat3(&key1.translate);

						DirectX::XMVECTOR S = DirectX::XMVectorLerp(S0, S1, rate);
						DirectX::XMVECTOR R = DirectX::XMQuaternionSlerp(R0, R1, rate);;
						DirectX::XMVECTOR T = DirectX::XMVectorLerp(T0, T1, rate);;

						DirectX::XMStoreFloat3(&node.scale, S);
						DirectX::XMStoreFloat4(&node.rotate, R);
						DirectX::XMStoreFloat3(&node.translate, T);
					}
				}
				break;
			}
		}
	}
	model->UpdateTransform();
}

