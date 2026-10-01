#include"ItemManager.h"

void ItemManager::Update(const DirectX::XMFLOAT3& playerPos)
{
	for (Item* item : items)
	{
		if (!item) continue;
		item->Update(playerPos);
	}
}

void ItemManager::Render(const RenderContext& rc)
{
	for (Item* item : items)
	{
		item->Render(rc);
	}
}

void ItemManager::Register(Item* item)
{
	items.emplace_back(item);
}

void ItemManager::Unregister(Item* item)
{
	items.erase(
		std::remove(items.begin(), items.end(), item),
		items.end()
	);
}