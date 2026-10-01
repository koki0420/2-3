#pragma once




// シーン基底
class Scene
{
public:
	Scene() = default;
	virtual ~Scene() = default;

	//初期化
	virtual void Initialize() = 0;
	//終了化
	virtual void Finalize() = 0;

	// 更新処理
	virtual void Update(float elapsedTime) {}

	// 描画処理
	virtual void Render(float elapsedTime) {}

	// GUI描画処理
	virtual void DrawGUI() {}

	//シーンの開始準備が完了しているか
	bool IsReady() const { return ready; }

	//準備完了設定
	void SetReady() { ready = true; }

private:
	bool ready = false;
};

