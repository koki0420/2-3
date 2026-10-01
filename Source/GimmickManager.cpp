#include"GimmickManager.h"

void GimmickManager::Update(float elapsedTime)
{
	for (Gimmick* gimmick : gimmicks)
	{
		gimmick->Update(elapsedTime);
	}
}

void GimmickManager::Render(const RenderContext& rc)
{
	for (Gimmick* gimmick : gimmicks)
	{
		gimmick->Render(rc);
	}
}

void GimmickManager::Register(Gimmick* gimmick)
{
	gimmicks.emplace_back(gimmick);
}

void GimmickManager::Unregister(Gimmick* gimmick)
{
	gimmicks.erase(
		std::remove(gimmicks.begin(), gimmicks.end(), gimmick),
		gimmicks.end()
	);
}