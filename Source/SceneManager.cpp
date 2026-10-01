#include "SceneManager.h"

void SceneManager::Update(float elapsedTime)
{
	if (nextScene)
	{
		//古いシーンを終了処理
		Clear();

		//新しいシーンを設定
		currentScene = nextScene;
		nextScene = nullptr;

		//シーン初期化処理
		if (!currentScene->IsReady())
		{
			currentScene->Initialize();
		}
	}

	if (currentScene)
	{
		currentScene->Update(elapsedTime);
	}
}

void SceneManager::Render(float elapsedTime)
{
	if (currentScene)
		currentScene->Render(elapsedTime);
}

void SceneManager::DrawGUI()
{
	if (currentScene)
		currentScene->DrawGUI();
}

//シーンクリア
void SceneManager::Clear()
{
	if (currentScene)
	{
		currentScene->Finalize();
		delete currentScene;
		currentScene = nullptr;
	}
}

//シーン切り替え
void SceneManager::ChangeScene(Scene* scene)
{
	//新しいシーンを設定
	nextScene = scene;
}
