#pragma once
#include"Item.h"
#include<vector>

class ItemManager
{
private:
	ItemManager() {};
	~ItemManager() {};
public:
	static ItemManager& instance()
	{
		static ItemManager instance;
		return instance;
	}	

	void Update(const DirectX::XMFLOAT3& playerPos);

	void Render(const RenderContext& rc);
	
	void Register(Item* item);

	void Unregister(Item* item);

private:
	std::vector<Item*> items;
};