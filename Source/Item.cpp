#include"Item.h"
#include <Graphics.h>
#include <imgui.h>

Item::Item(DirectX::XMFLOAT3 pos, Interact::InteractionType type)
{
	item.position = pos;
	item.scale = { 0.05, 0.05, 0.05 };
	alive_item = true;
	item_type = type;
	switch (type)
	{
	default:
		/*faallthrough*/
	case Interact::InteractionType::Bomb:
		item.model = std::make_unique<Model>("Data/Model/onomatopoeia/boom.mdl");
		break;
	case Interact::InteractionType::Slash:
		item.model = std::make_unique<Model>("Data/Model/onomatopoeia/Slash.mdl");
		break;
	case Interact::InteractionType::Smash:
		item.model = std::make_unique<Model>("Data/Model/onomatopoeia/Smash.mdl");
		break;
	case Interact::InteractionType::Flash:
		item.model = std::make_unique<Model>("Data/Model/onomatopoeia/Flash.mdl");
		break;
	}
}

Item::~Item(){}

void Item::Update(const DirectX::XMFLOAT3& playerPos)
{
	if (!alive_item) return;
	// プレイヤーへの方向ベクトル
	DirectX::XMFLOAT3 dir;
	dir.x = playerPos.x - item.position.x;
	dir.y = playerPos.y - item.position.y;
	dir.z = playerPos.z - item.position.z;

	// 正規化
	DirectX::XMVECTOR v = DirectX::XMLoadFloat3(&dir);
	v = DirectX::XMVector3Normalize(v);
	DirectX::XMStoreFloat3(&dir, v);

	// Y軸回転（水平面でプレイヤー方向を向く）
	float angleY = atan2(dir.x, dir.z);
	item.angle.y = angleY;
	item.UpdateTransform();
}

void Item::Render(RenderContext rc)
{
	ID3D11DeviceContext* dc = Graphics::Instance().GetDeviceContext();
	ModelRenderer* modelRenderer = Graphics::Instance().GetModelRenderer();
	ShapeRenderer* shaperenderer = Graphics::Instance().GetShapeRenderer();
	if (alive_item)
	{
		shaperenderer->DrawSphere(item.position, 0.5f, { 1,1,1,1 });
		modelRenderer->Render(rc, item.transform, item.model.get(), ShaderId::Lambert);
	}
}