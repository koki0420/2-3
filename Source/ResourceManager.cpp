#include <filesystem>
#include <imgui.h>
#include "Graphics.h"
#include "ResourceManager.h"

// ƒ‚ƒfƒ‹ƒŠƒ\[ƒX“Ç‚İ‚İ
std::shared_ptr<ModelResource> ResourceManager::LoadModelResource(const char* filename)
{
    // •¶š—ñƒL[‰»
    std::string key = filename;

    // ‡@ Šù‚É“Ç‚İ‚Ü‚ê‚Ä‚¢‚é‚©H
    auto it = models.find(key);
    if (it != models.end()) {
        // ¨ “Ç‚İ‚İÏ‚İ‚ğ•Ô‚·
        std::shared_ptr<ModelResource> existing = it->second.lock();
        if (existing) return existing;
    }

    // ‡A V‹Kì¬
    auto resource = std::make_shared<ModelResource>();

    // ‡B “Ç‚İ‚İ
    resource->Load(Graphics::Instance().GetDevice(), filename);

    // ‡C ŠÇ——p map ‚É“o˜^
    models[key] = resource;

    // ‡D •Ô‚·
    return resource;
}

// ƒfƒoƒbƒOGUI•`‰æ
void ResourceManager::DrawDebugGUI()
{
	if (ImGui::CollapsingHeader("Resource", ImGuiTreeNodeFlags_DefaultOpen))
	{
		for (auto it = models.begin(); it != models.end(); ++it)
		{
			std::filesystem::path filepath(it->first);

			int use_count = it->second.use_count();
			ImGui::Text("use_count = %5d : %s", use_count, filepath.filename().u8string().c_str());
		}
	}
}

