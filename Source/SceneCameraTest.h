#pragma once

#pragma once
#include "Scene.h"
#include "Object.h"
#include "Camera.h"
#include "Player.h"
#include "FollowCamera.h"
#include "EnemyCombatLink.h"

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
private:
    void SpawnTestEnemies();
    Camera camera;
    FollowCamera followCamera;
    Object stage;
    Player player;
    EnemyCombatLink enemyCombat;
    bool pause = false;
};
