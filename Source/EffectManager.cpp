#include "Graphics.h"
#include "EffectManager.h"

#include <windows.h>
#include "player.h"


//初期化
void EffectManager::Initialize()
{
	Graphics& graphics = Graphics::Instance();

	//Effekseerレンダラ生成
	effekseerRenderer = EffekseerRendererDX11::Renderer::Create(graphics.GetDevice(), graphics.GetDeviceContext(), 2048);
	
	//Effekseerマネージャー生成
	effekseerManager = Effekseer::Manager::Create(1024);

	//Effekseerレンダラの各種設定（特別なカスタマイズをいない場合は定型的に以下の設定でOK）
	effekseerManager->SetSpriteRenderer(effekseerRenderer->CreateSpriteRenderer());
	effekseerManager->SetRibbonRenderer(effekseerRenderer->CreateRibbonRenderer());
	effekseerManager->SetRingRenderer(effekseerRenderer->CreateRingRenderer());
	effekseerManager->SetTrackRenderer(effekseerRenderer->CreateTrackRenderer());
	effekseerManager->SetModelRenderer(effekseerRenderer->CreateModelRenderer());
	//Effekseer内でのローダーの設定（特別なカスタマイズをいない場合は定型的に以下の設定でOK）
	effekseerManager->SetTextureLoader(effekseerRenderer->CreateTextureLoader());
	effekseerManager->SetModelLoader(effekseerRenderer->CreateModelLoader());
	effekseerManager->SetMaterialLoader(effekseerRenderer->CreateMaterialLoader());

	//Effekseerを左手座標系で計算する
	effekseerManager->SetCoordinateSystem(Effekseer::CoordinateSystem::LH);


}

//終了化
void EffectManager::Finalize()
{
	//EffeksserManagerなどはスマートポインタによって破棄されるので何もしない

	playingEffects.clear();
	effects.clear();

	effekseerManager.Reset();
	effekseerRenderer.Reset();

}

//更新処理
void EffectManager::Update(float elapsedTime)
{
	//エフェクト更新処理（引数にはフレームの経過時間を渡す）
	effekseerManager->Update(elapsedTime*60.0f);

	//DirectX::XMFLOAT3 playerPos = 



	static int frame = 0;
	frame++;

	if (frame % 4 == 0)
	{
		playingEffects.erase(
			std::remove_if(
				playingEffects.begin(),
				playingEffects.end(),
				[&](const PlayingEffect& e)
				{
					return e.handle < 0 ||
						!effekseerManager->Exists(e.handle);
				}),
			playingEffects.end()
		);
	}


	//for (auto& e : playingEffects)
	//{
	//	float dx = e.pos.x - playerPos.x;
	//	float dy = e.pos.y - playerPos.y;
	//	float dz = e.pos.z - playerPos.z;

	//	float distSq = dx * dx + dy * dy + dz * dz;

	//	// 100より遠いなら停止
	//	if (distSq > 100.0f * 100.0f)
	//	{
	//		effekseerManager->StopEffect(e.handle);
	//	}
	//}

	


}


void EffectManager::StopEffect(Effekseer::Handle h)
{

	if (h < 0) return;

	effekseerManager->StopEffect(h);

	playingEffects.erase(
		std::remove_if(
			playingEffects.begin(),
			playingEffects.end(),
			[&](const PlayingEffect& e)
			{
				return e.handle == h;
			}),
		playingEffects.end()
	);

}

Effect* EffectManager::CreateEffect(const char* path)
{


	//// すでにロード済みならそれを返す
	//auto it = effectCache.find(path);
	//if (it != effectCache.end())
	//{
	//	return it->second;
	//}

	//// 新しく作る
	//effects.push_back(std::make_unique<Effect>(path));
	//Effect* e = effects.back().get();

	//effectCache[path] = e;
	//return e;


	
		auto it = effectCache.find(path);
		if (it != effectCache.end())
		{
			return it->second;
		}

		try
		{
			effects.push_back(std::make_unique<Effect>(path));
		}
		catch (...)
		{
			OutputDebugStringA(path);
			OutputDebugStringA(" load failed\n");
			return nullptr;
		}

		Effect* e = effects.back().get();
		effectCache[path] = e;
		return e;





}


//描画処理
void EffectManager::Render(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection)
{
	//ビュー&プロジェクション行列をEffekseerレンダラに設定
	effekseerRenderer->SetCameraMatrix(*reinterpret_cast<const Effekseer::Matrix44*>(&view));
	effekseerRenderer->SetProjectionMatrix(*reinterpret_cast<const Effekseer::Matrix44*>(&projection));

	//Effekseer描画開始
	effekseerRenderer->BeginRendering();

	//Effekseer描画実行
	//マネージャー単位で描画するので描画順を制御する場合はマネージャーを複数個作成し、
	//Draw()関数を実行する順序で制御できそう
	effekseerManager->Draw();

	//Effekseer描画終了
	effekseerRenderer->EndRendering();
}

Effect* EffectManager::GetThunderEffect()
{


	//static Effect* effect =
		//EffectManager::Instance().CreateEffect("Data/Effect/enagyBall.efkefc");

	return 0;


}


Effekseer::Handle EffectManager::PlayEffect(Effect* effect, const DirectX::XMFLOAT3& pos, float scale)
{
	//エフェクト制限
	static const int MAX_EFFECTS = 300;

	if (playingEffects.size() > MAX_EFFECTS)
		return -1;


	Effekseer::Handle h = effect->Play(pos, scale);


	
	playingEffects.push_back({ h, effect,pos });

	


	return h;
}
