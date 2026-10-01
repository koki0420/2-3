#pragma once
#include <DirectXMath.h>
#include<Effekseer.h>
#include<EffekseerRendererDX11.h>
#include "Effect.h"
#include <vector>
#include <memory>
#include <deque>
#include <unordered_map>

struct PlayingEffect
{
	Effekseer::Handle handle;
	Effect* effect;
	DirectX::XMFLOAT3 pos;
	float timer;
};




//エフェクトマネージャー
class EffectManager
{
private:
	EffectManager() {}
	~EffectManager() {}

public:
	//唯一のインスタンス取得
	static EffectManager& Instance()
	{
		static EffectManager instance;
		return instance;
	}

	//初期化
	void Initialize();

	//終了化
	void Finalize();

	//更新処理
	void Update(float elapsedTime);

	//描画処理
	void Render(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection);

	//Effekseerマネージャーの取得
	Effekseer::ManagerRef GetEffekseerManager() { return effekseerManager; }

	//敵の攻撃のエフェクト用
	static Effect* GetThunderEffect();

	Effekseer::Handle PlayEffect(Effect* effect, const DirectX::XMFLOAT3& pos, float scale = 1.0f);

	void StopEffect(Effekseer::Handle h);
	Effect* CreateEffect(const char* path);

private:
	Effekseer::ManagerRef          effekseerManager;
	EffekseerRenderer::RendererRef effekseerRenderer;
	std::vector<PlayingEffect> playingEffects;
	std::deque<std::unique_ptr<Effect>> effects;

	std::unordered_map<std::string, Effect*> effectCache;
};