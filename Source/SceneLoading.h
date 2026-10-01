#pragma once

#include "Scene.h"

#include "Sprite.h"
#include <thread>

//ローディングシーン
class SceneLoading : public Scene
{
public:
	SceneLoading(Scene* nextScene) : nextScene(nextScene){}
	~SceneLoading() override{}

	void Initialize()override;
	void Finalize()override;

	//更新処理
	void Update(float elapsedTime) override;

	//描画処理
	void Render(float elapsedTime) override;

	// GUI描画
	void DrawGUI() override;

private:
	//ローディングスレッド
	static void LoadingThread(SceneLoading* scene);

private:
	std::unique_ptr<Sprite> spriteLogo = nullptr;
	std::unique_ptr<Sprite> spriteBack1 = nullptr;
	std::unique_ptr<Sprite> spriteBack2 = nullptr;
	float angle = 0.0f;
	float timer = 0.0f;

	//チュートリアル
	int pageNum = 1;	//何枚目のスプライトか
	bool isAllRead = false;	//全てのスプライトを表示し、クリックされたか
	bool release = false; //押された瞬間

	Scene* nextScene = nullptr;
	std::unique_ptr<std::thread> thread = nullptr;
};