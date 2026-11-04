#pragma once

#pragma once
#include "Scene.h"
#include "Object.h"
#include "Camera.h"
#include "Player.h"
#include"Enemy.h"
#include "boss.h"
#include "Sprite.h"
#include "CameraController.h"

class SceneCameraTest : public Scene
{
public:
    SceneCameraTest();
    ~SceneCameraTest();
    void Initialize() override;
    void Finalize() override;
    void Update(float elapsedTime) override;
    void Render(float elapsedTime) override;
    void DrawGUI() override;

    const Camera* GetCamera() const override
    {
        return camera;
    }

    void Respawn();

private:
    
    Camera* camera = nullptr;
    CameraController* cameraController=nullptr;
    Object stage;
    Player* player = nullptr;

    std::vector < Enemy* >enemyList;//敵へアクセスするための一覧
    std::vector<float>enemyHP;//敵ごとのHP

    float enemyRadius = 1.0f;


    std::vector<std::unique_ptr<Enemy>> enemies;
    std::unique_ptr<Boss> boss;
    int enemy_num = 0;

    bool Boss_Dead = false;

    //デバッグ用
    int enemy_num_debug = 0;

    //std::unique_ptr<Sprite> spriteGauge = nullptr;
    //std::unique_ptr<Sprite> spriteHP = nullptr;
    //std::unique_ptr<Sprite> spriteW1 = nullptr;
    //std::unique_ptr<Sprite> spriteW2 = nullptr;
    //std::unique_ptr<Sprite> spriteW3 = nullptr;

    //debug
    bool showEnemyCollision = false;
    float enemyCollisionHeight = 1.0f;

};
