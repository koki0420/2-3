#pragma once

#include"Scene.h"

//シーンマネージャー
class SceneManager
{
private:
	//コンストラクタをprivateにすることで、
	//インスタンスの取得方法をInstance()関数に限る
	SceneManager(){}
	~SceneManager(){}

public:
	//唯一のインスタンス取得
	static SceneManager& Instance()
	{
		static SceneManager instance;
		return instance;
	}

	//更新処理
	void Update(float elapsedTime);

	//描画処理
	void Render(float elapsedTime);

	// GUI描画
	void DrawGUI();

	//シーンクリア
	void Clear();

	//シーン切り替え
	void ChangeScene(Scene* scene);

private:
	Scene* currentScene = nullptr;
	Scene* nextScene = nullptr;
};