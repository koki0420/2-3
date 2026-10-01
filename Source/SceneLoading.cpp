#include "SceneLoading.h"
#include "SceneManager.h"
#include "Graphics.h"
#include <windows.h>

void SceneLoading::Initialize()
{
	//スプライト初期化
	spriteLogo = std::make_unique<Sprite>(Graphics::Instance().GetDevice(), "Data/Sprite/LOAD.png");
	spriteBack1 = std::make_unique<Sprite>(Graphics::Instance().GetDevice(), "Data/Sprite/tutorial1.png");
	spriteBack2 = std::make_unique<Sprite>(Graphics::Instance().GetDevice(), "Data/Sprite/tutorial2.png");

	//スレッド開始
	thread = std::make_unique<std::thread>(LoadingThread, this);
}

void SceneLoading::Finalize()
{
	if (nextScene != nullptr)
	{
		delete nextScene;
		nextScene = nullptr;
	}
	if (thread != nullptr)
	{
		thread->join();
	}
}

//更新処理
void SceneLoading::Update(float elapsedTime)
{
	constexpr float speed = 180;

	angle += speed * elapsedTime;
	timer += elapsedTime;

	// 1F前の左クリック入力情報
	static bool prev = false;
	//現在の左クリック入力情報
	bool now = (GetAsyncKeyState(VK_LBUTTON) & 0x8000);

	// 1枚目且つクリック終わったら
	if (pageNum == 1 && (prev && !now))
	{
		//次のスプライトへ
		pageNum++;
	}
	//2枚目且つクリック
	if (pageNum == 2 && (prev && !now) && release)
	{
		isAllRead = true;

	}
	if (pageNum == 2 && (!prev && now))
	{
		release = true;

	}

	//次のシーンの準備が完了したらシーンを切り替える + 全部読まれたら
	if (nextScene->IsReady() && timer > 2.0f && isAllRead)
	{
		SceneManager::Instance().ChangeScene(nextScene);
		nextScene = nullptr;
	}

	prev = now;
}

//描画処理
void SceneLoading::Render(float elapsedTime)
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	RenderState* renderState = graphics.GetRenderState();

	//描画準備
	RenderContext rc;
	rc.deviceContext = dc;
	rc.renderState = graphics.GetRenderState();

	// 2Dスプライト描画
	{
		float screenWidth = static_cast<float>(graphics.GetScreenWidth());
		float screenHeight = static_cast<float>(graphics.GetScreenHeight());
		float spriteWidth = 280 * screenWidth / 1920;
		float spriteHeight = 280 * screenHeight / 1080;
		float positionX = screenWidth - spriteWidth - 20; // 20ピクセルだけずらす
		float positionY = screenHeight - spriteHeight - 20;

		//ローディング画面の背景描画
		if(pageNum == 1)
		{
			spriteBack1->Render(dc, 0.0f, 0.0f, 0.1f,
				screenWidth, screenHeight,
				0.0f,
				1.0f, 1.0f, 1.0f, 1.0f);
		}
		if (pageNum == 2)
		{
			spriteBack2->Render(dc, 0.0f, 0.0f, 0.1f,
				screenWidth, screenHeight,
				0.0f,
				1.0f, 1.0f, 1.0f, 1.0f);
		}

		//画面右下にローディングアイコン描画
		if(timer <= 2.0f)
		{
			spriteLogo->Render(dc, positionX, positionY, 0.0f,
				spriteWidth, spriteHeight,
				angle,
				1.0f, 1.0f, 1.0f, 1.0f);
		}
	}
}

void SceneLoading::DrawGUI()
{
}

void SceneLoading::LoadingThread(SceneLoading* scene)
{
	// COM関連の初期化でスレッドごとに呼ぶ必要がある
	CoInitialize(nullptr);

	//次のシーンの初期化を行う
	scene->nextScene->Initialize();

	//スレッドが終わる前にCOM関連の終了化
	CoUninitialize();

	//次のシーンの準備完了設定
	scene->nextScene->SetReady();

}




