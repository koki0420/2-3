#include "Graphics.h"
#include "Effect.h"
#include "EffectManager.h"
#include <cassert>

//コンストラクタ
Effect::Effect(const char* filename)
{







	//エフェクトを読み込みする前にロックする
	//※マルチスレッドでEffectを作成するとDeviceContextを同時アクセスして
	//　フリーズする可能性があるので排他制御する
	std::lock_guard<std::mutex>lock(Graphics::Instance().GetMutex());


	//Effekseerのリソースを読み込む
	//EffekseerはUTF-16のファイルパス以外は対応してないため文字コード変換が必要
	char16_t utf16Filename[256];
	Effekseer::ConvertUtf8ToUtf16(utf16Filename, 256, filename);




	//Effekseer::Managerを取得
	Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();


	assert(effekseerManager.Get() != nullptr);


	
	
	

	
	//Effekseerエフェクトを読み込み
	
		effekseerEffect = Effekseer::Effect::Create(
			effekseerManager,
			(EFK_CHAR*)utf16Filename
		);





		if (effekseerEffect == nullptr)
		{
			std::string msg = "Load Failed : ";
			msg += filename;

			MessageBoxA(nullptr, msg.c_str(), "Effect Error", MB_OK);

			assert(false);

		}


}

//再生
Effekseer::Handle Effect::Play(const DirectX::XMFLOAT3& position, float scale)
{
	Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();

	Effekseer::Handle handle = effekseerManager->Play(effekseerEffect, position.x, position.y,
		position.z);
	effekseerManager->SetScale(handle, scale, scale, scale);


	return handle;
}

//停止
void Effect::Stop(Effekseer::Handle handle)
{
	Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();

	effekseerManager->StopEffect(handle);
}

//座標設定
void Effect::SetPosition(Effekseer::Handle handle, const DirectX::XMFLOAT3& position)
{
	Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();

	effekseerManager->SetLocation(handle, position.x, position.y, position.z);

}

//スケール設定
void Effect::SetScale(Effekseer::Handle handle, const DirectX::XMFLOAT3& scale)
{
	Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();

	effekseerManager->SetScale(handle, scale.x, scale.y, scale.z);
}

void Effect::SetMatrix(Effekseer::Handle handle, const Effekseer::Matrix43& matrix)
{

	if (handle >= 0)
	{
		EffectManager::Instance().GetEffekseerManager()->SetMatrix(handle, matrix);
	}

}


void Effect::SetRotation(
	Effekseer::Handle handle,
	float x,
	float y,
	float z)
{
	auto manager = EffectManager::Instance().GetEffekseerManager();

	manager->SetRotation(handle, x, y, z);
}



Effect::~Effect()
{

	
	
		effekseerEffect.Reset();
	

}