#include "SoundSource.h"
#include <Graphics.h>
#include <imgui.h>

SoundSource::SoundSource(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 angle, const ModelType type) : modelType(type)
{
	soundSource.position = pos;
	desk.position = pos;
	desk.position.y = 0 + 1.53f;
	soundSource.angle = desk.angle = angle;
	desk.model = std::make_unique<Model>("Data/Model/object/desk.mdl");
	
	//modelType = type;
	switch (modelType)
	{
	default:
		/*fallthrough*/
	case ModelType::Gramophone:
		soundSource.scale = { 0.22f, 0.22f, 0.22f };
		desk.scale = { 0.25f, 0.22f, 0.25f };
		soundSource.model = std::make_unique<Model>("Data/Model/object/gramophong.mdl");
		break;
	case ModelType::TV:
		soundSource.position.y += 0.75f;
		soundSource.scale = { 0.2, 0.2f, 0.2f };
		desk.scale = { 0.30, 0.22, 0.30 };
		soundSource.model = std::make_unique<Model>("Data/Model/object/TV(comp).mdl");
		break;
	}
}

void SoundSource::Update()
{
	desk.UpdateTransform();
	soundSource.UpdateTransform();
}

void SoundSource::Render(RenderContext rc)
{
	ID3D11DeviceContext* dc = Graphics::Instance().GetDeviceContext();
	ModelRenderer* modelRenderer = Graphics::Instance().GetModelRenderer();

	modelRenderer->Render(rc, soundSource.transform, soundSource.model.get(), ShaderId::Lambert);
	modelRenderer->Render(rc, desk.transform, desk.model.get(), ShaderId::Lambert);
}

void SoundSource::DrawGUI()
{
#if DEBUG

#endif
}
