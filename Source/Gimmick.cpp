#include"Gimmick.h"
#include <Graphics.h>
#include <imgui.h>


Gimmick::Gimmick(DirectX::XMFLOAT3 pos, GimmickType type)
{
	gimmick.position = pos;
	gimmick.scale = { 0.1f,0.1f,0.1f };
	alive_gimmick = true;
	gimmickType = type;

	animation_flag = false;
	FallGimmick = false;
	FallGimmickFlag = false;

	switch (gimmickType)
	{
	default:
		/*fallthrough*/
	case GimmickType::BreakableWall:
		gimmick.model = std::make_unique<Model>("Data/Model/object/break_wall.mdl");
		triggerItemType = Interact::InteractionType::Bomb;
		break;
		
	case GimmickType::KnockedDownBridge:
		gimmick.model = std::make_unique<Model>("Data/Model/object/Brige.mdl");
		triggerItemType = Interact::InteractionType::Slash;
		break;
		
	case GimmickType::DroppedBridge:
		gimmick.model = std::make_unique<Model>("Data/Model/object/FallBridge.mdl");
		triggerItemType = Interact::InteractionType::Slash;
		break;
		
	case GimmickType::MovableContainer:
		gimmick.model = std::make_unique<Model>("Data/Model/object/container.mdl");
		gimmick.scale.x = 0.2f;
		triggerItemType = Interact::InteractionType::Smash;
		break;
		
	case GimmickType::StringedUpRope:
		gimmick.model = std::make_unique<Model>("Data/Model/object/rope.mdl");
		triggerItemType = Interact::InteractionType::Slash;
		break;
	}
	animation = std::make_unique<Animation>(*gimmick.model);	
}

Gimmick::~Gimmick(){}

void Gimmick::Update(float elapsedTime)
{

	if (!(alive))
	{
		switch (gimmickType)
		{
		default:
			/*fallthrough*/
		case GimmickType::BreakableWall:
			alive_gimmick = false;
			break;

		case GimmickType::KnockedDownBridge:
			if (!animation->IsPlaying())
			{
				animation->PlayAnimation("brige(comp)", false);
			}
			break;

		case GimmickType::DroppedBridge:
			if (FallGimmickFlag)
			{
				if (gimmick.position.y <= -0.5f)gimmick.position.y = -0.5f;
				else
				{
					gimmick.position.y -= 0.05f;
				}
			}
			break;

		case GimmickType::MovableContainer:
			if (gimmick.position.z >= -5)gimmick.position.z = -5;
			else gimmick.position.z += 0.2f;
			break;

		case GimmickType::StringedUpRope:
			alive_gimmick = false;
			
			break;
		}
	}
	if (FallGimmick)
	{
		FallGimmickFlag = true;
	}

	animation->Update(elapsedTime);
	gimmick.UpdateTransform();
}

void Gimmick::Render(RenderContext rc)
{
	ID3D11DeviceContext* dc = Graphics::Instance().GetDeviceContext();
	ModelRenderer* modelRenderer = Graphics::Instance().GetModelRenderer();
	ShapeRenderer* shaperenderer = Graphics::Instance().GetShapeRenderer();
	if (alive_gimmick)
	{
		modelRenderer->Render(rc, gimmick.transform, gimmick.model.get(), ShaderId::Lambert);
	}
}

