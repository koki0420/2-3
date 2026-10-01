#pragma once
#include"Gimmick.h"
#include<vector>

class GimmickManager
{
private:
	GimmickManager() {};
	~GimmickManager() {};
public:
	static GimmickManager& instance()
	{
		static GimmickManager instance;
		return instance;
	}

	void Update(float elapsedTime);
	void Render(const RenderContext& rc);

	void Register(Gimmick* gimmick);
	void Unregister(Gimmick* gimmick);

private:
	std::vector<Gimmick*> gimmicks;
};