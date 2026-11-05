#include "SceneCameraTest.h"
#include "EnemyManager.h"
#include "Graphics.h"
#include <imgui.h>
#include <cmath>
#include "ShapeRenderer.h"

SceneCameraTest::SceneCameraTest() { }
SceneCameraTest::~SceneCameraTest() {}

void SceneCameraTest::Initialize()
{
    player = new Player();
    camera = new Camera();
    cameraController = new CameraController;
    stage.model = std::make_unique<Model>("Data/Model/Stage/stage4.mdl");

    enemy_num = 0;

    stage.scale.x = 0.2f;
    stage.scale.y = 0.2f;
    stage.scale.z = 0.2f;

    stage.position.y = -20;

    boss = std::make_unique<Boss>(DirectX::XMFLOAT3{ 0, 0, 100 });

    for (int i = 0;i < 10;i += 1)
    {
        for (int j = 0;j < 10;j += 1)
        {
            enemies.emplace_back(std::make_unique<Enemy>(DirectX::XMFLOAT3{ -260 + 52.0f * j, 0, -260 + 52.0f * i }));
        }
    }
    for (size_t i = 0; i < enemies.size(); i++)
    {
        enemyList.push_back(enemies[i].get());
        enemyHP.push_back(100.0f);

        EnemyManager::Instance().Register(std::move(enemies[i]));

        enemy_num += 1;
    }
    /*spriteGauge = std::make_unique<Sprite>(Graphics::Instance().GetDevice(), "Data/Sprite/gauge_bar_neon-green_01_ff4444_128.png");
    spriteHP = std::make_unique<Sprite>(Graphics::Instance().GetDevice(), "Data/Sprite/gauge_bar_blue-gold_01_ff4444_128.png");

    spriteW1 = std::make_unique<Sprite>(Graphics::Instance().GetDevice(), "Data/Sprite/sword_transparent.png");
    spriteW2 = std::make_unique<Sprite>(Graphics::Instance().GetDevice(), "Data/Sprite/scythe_transparent .png");
    spriteW3 = std::make_unique<Sprite>(Graphics::Instance().GetDevice(), "Data/Sprite/glove_transparent .png");*/


    cameraController->SyncCameraToController(*camera);

}

void SceneCameraTest::Finalize()
{
    enemyList.clear();
    enemyHP.clear();
    enemies.clear();

    EnemyManager::Instance().Clear();
}

void SceneCameraTest::Update(float elapsedTime)
{
    DirectX::XMFLOAT3 focus = player->GetPosition();
    focus.y += 1.0f;
    cameraController->SetFocus(focus);
    cameraController->Update(elapsedTime,player->GetAngle().y);
    cameraController->SyncControllerToCamera(*camera);

    if (Boss_Dead)
    {
        Respawn();
        boss->Boss_Chage();
        Boss_Dead = false;
    }
    {
        if (enemy_num > 0) boss->Boss_Set(false);
        else boss->Boss_Set(true);
        boss->Update(elapsedTime);
    }

    stage.UpdateTransform();

    EnemyManager::Instance().Update(elapsedTime);
    player->Update(elapsedTime, stage, *camera);


    //enemyPositionには敵の球の中心、enemyRadiusには半径を入れる
    for (size_t i = 0; i < enemyList.size(); i++)
    {
        // 倒れている敵は調べない
        if (!enemyList[i]->Alive()) 
        {
            continue;
        }

        // 敵の判定の中心
        /*DirectX::XMFLOAT3 enemyPosition = enemyList[i]->GetPos();
        enemyPosition.y += 10.0f;*/
        DirectX::XMFLOAT3 enemyPosition = enemyList[i]->GetPos();
        enemyPosition.y += enemyCollisionHeight;

        if (player->CheckAttackHit(i,enemyPosition, enemyRadius))
        {
            enemyHP[i] -= player->GetPAtkDmg();

            if (enemyHP[i] <= 0.0f)
            {
                EnemyManager::Instance().Enemy_Set(
                    static_cast<int>(i), false);

                enemy_num -= 1;

                //この敵の処理はここで終える
                continue;
            }
        }

        //player->CheckTakeAttack(i, enemyPosition, enemyRadius);

    }
    
}

void SceneCameraTest::Render(float elapsedTime)
{
    ID3D11DeviceContext* dc = Graphics::Instance().GetDeviceContext();
    RenderState* renderState = Graphics::Instance().GetRenderState();
    ModelRenderer* modelRenderer = Graphics::Instance().GetModelRenderer();

    ShapeRenderer* shapeRenderer = Graphics::Instance().GetShapeRenderer();

    RenderContext rc;
    rc.deviceContext = dc;

    rc.renderState = renderState;
    rc.camera = camera;

    modelRenderer->Render(rc, stage.transform, stage.model.get(), ShaderId::Lambert);
    EnemyManager::Instance().Render(rc, modelRenderer);
    player->Render(elapsedTime, rc);
    boss->Render(rc, modelRenderer);

    //{
    //    spriteGauge->Render(dc, 50, 50, 0, 480, 32, 0, 48, 128, 32, 0, 1, 1, 1, 1);
    //    spriteHP->Render(dc, 50, 50, 0, 480, 32, 0, 48, 128, 32, 0, 1, 1, 1, 1);
    //    //


    //    spriteGauge->Render(dc, 80, 500, 0, 280, 28, 0, 48, 128, 32, 0, 1, 1, 1, 1);
    //    spriteHP->Render(dc, 80, 500, 0, 280, 28, 0, 48, 128, 32, 0, 1, 1, 1, 1);
    //    spriteW1->Render(dc, 60, 500, 0, 28, 28, 0, 0, 134, 134, 0, 1, 1, 1, 1);

    //    spriteGauge->Render(dc, 80, 532, 0, 280, 28, 0, 48, 128, 32, 0, 1, 1, 1, 1);
    //    spriteHP->Render(dc, 80, 532, 0, 280, 28, 0, 48, 128, 32, 0, 1, 1, 1, 1);
    //    spriteW2->Render(dc, 60, 532, 0, 28, 28, 0, 0, 134, 134, 0, 1, 1, 1, 1);

    //    spriteGauge->Render(dc, 80, 564, 0, 280, 28, 0, 48, 128, 32, 0, 1, 1, 1, 1);
    //    spriteHP->Render(dc, 80, 564, 0, 280, 28, 0, 48, 128, 32, 0, 1, 1, 1, 1);
    //    spriteW3->Render(dc, 60, 564, 0, 28, 28, 0, 0, 134, 134, 0, 1, 1, 1, 1);

    //}


    //debug
    if (showEnemyCollision)
    {
        for (Enemy* enemy : enemyList)
        {
            if (enemy == nullptr || !enemy->Alive())
            {
                continue;
            }

            DirectX::XMFLOAT3 center = enemy->GetPos();
            center.y += enemyCollisionHeight;

            shapeRenderer->DrawSphere(
                center,
                enemyRadius,
                { 0, 1, 0, 1 });
        }
    }

    shapeRenderer->Render(
        rc.deviceContext,
        rc.camera->GetView(),
        rc.camera->GetProjection()
    );

}

void SceneCameraTest::DrawGUI()
{
    ImGui::SetNextWindowSize(ImVec2(380, 650), ImGuiCond_Once);
    if (ImGui::Begin("SceneCameraTest"))
    {
        if (ImGui::Button("enemy"))
        {
            if (enemy_num > 0)
            {
                EnemyManager::Instance().Enemy_Set(enemy_num_debug, false);
                enemy_num -= 1;
                enemy_num_debug += 1;
            }
        }
        if (ImGui::Button("all_enemy"))
        {
            if (enemy_num > 0)
            {
                for (size_t i = 0;i < enemies.size();i += 1)
                {
                    EnemyManager::Instance().Enemy_Set(i, false);
                    enemy_num -= 1;
                }
            }
        }
        if (ImGui::Button("boss"))
        {
            if (boss->Boss_Alive() && enemy_num <= 0)
            {
                Boss_Dead = true;
            }
        }

        ImGui::Separator();
        ImGui::Text("Enemy collision");

        ImGui::Checkbox("Show enemy collision", &showEnemyCollision);
        ImGui::SliderFloat("Enemy radius", &enemyRadius, 0.1f, 10.0f);
        ImGui::SliderFloat(
            "Enemy collision height", &enemyCollisionHeight, -30.0f, 10.0f);

        ImGui::Separator();

        if (ImGui::CollapsingHeader("Camera",
            ImGuiTreeNodeFlags_DefaultOpen))
        {
            // ビュー行列の逆行列から、カメラのワールド座標を求める
            DirectX::XMFLOAT4X4 view = camera->GetView();

            DirectX::XMMATRIX inverseView = DirectX::XMMatrixInverse(
                nullptr,
                DirectX::XMLoadFloat4x4(&view));

            DirectX::XMFLOAT3 cameraPosition;

            DirectX::XMStoreFloat3(
                &cameraPosition,
                inverseView.r[3]);

            DirectX::XMFLOAT3 front = camera->GetFront();
            DirectX::XMFLOAT3 right = camera->GetRight();

            ImGui::Text("Position");
            ImGui::Text("X: %.2f  Y: %.2f  Z: %.2f",
                cameraPosition.x,
                cameraPosition.y,
                cameraPosition.z);

            ImGui::Text("Front direction");
            ImGui::Text("X: %.3f  Y: %.3f  Z: %.3f",
                front.x, front.y, front.z);

            ImGui::Text("Right direction");
            ImGui::Text("X: %.3f  Y: %.3f  Z: %.3f",
                right.x, right.y, right.z);

            DirectX::XMFLOAT3 playerPosition = player->GetPosition();

            float dx = cameraPosition.x - playerPosition.x;
            float dy = cameraPosition.y - playerPosition.y;
            float dz = cameraPosition.z - playerPosition.z;

            float distance = sqrtf(dx * dx + dy * dy + dz * dz);

            ImGui::Text("Distance to player: %.2f", distance);
        }

        player->DrawGUI();
    }
    ImGui::End();
}


void SceneCameraTest::Respawn()
{
    enemy_num = 0;
    enemy_num_debug = 0;

    for (size_t i = 0; i < enemyList.size(); i++)
    {
        enemyHP[i] = 100.0f;

        EnemyManager::Instance().Enemy_Set(
            static_cast<int>(i), true);

        enemy_num += 1;
    }
}