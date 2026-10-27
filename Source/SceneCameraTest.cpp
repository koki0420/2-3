#include "SceneCameraTest.h"
#include "EnemyManager.h"
#include "Graphics.h"
#include <imgui.h>
#include <cmath>

SceneCameraTest::SceneCameraTest() { Initialize(); }
SceneCameraTest::~SceneCameraTest() { Finalize(); }

void SceneCameraTest::Initialize()
{
    stage.scale = { 1, 1, 1 };
    stage.model = std::make_unique<Model>("Data/Model/Stage/stage4.mdl");
    stage.UpdateTransform();
    // コンストラクタ以外からのシーン再初期化にも対応する。
    player.Initialize();
    pause = false;
    const float width = Graphics::Instance().GetScreenWidth();
    const float height = Graphics::Instance().GetScreenHeight();
    camera.SetPerspectiveFov(45.0f * GameplayMath::Pi / 180.0f,
        height > 0.0f ? width / height : 16.0f / 9.0f, 1.0f, 10000.0f);
    followCamera.Reset(camera, player.GetPos(), player.GetAngle().y);
    SpawnTestEnemies();
}
void SceneCameraTest::SpawnTestEnemies()
{
    auto& manager = EnemyManager::Instance();
    enemyCombat.Clear();
    manager.Clear();
    const auto& center = player.GetPos();
    // 動作確認用。実ゲームのスポーン処理ができたら、ここを置き換える。
    for (int i = 0; i < 24; ++i)
    {
        const float theta = i * (2.0f * GameplayMath::Pi / 24.0f);
        const float radius = 220.0f + (i % 3) * 90.0f;
        auto enemy = std::make_unique<Enemy>(DirectX::XMFLOAT3{
            center.x + std::sin(theta) * radius, center.y,
            center.z + std::cos(theta) * radius });
        enemyCombat.Register(*enemy);
        manager.Register(std::move(enemy));
    }
}
void SceneCameraTest::Finalize()
{
    enemyCombat.Clear();
    EnemyManager::Instance().Clear();
}

void SceneCameraTest::Update(float elapsedTime)
{
    if (pause || !std::isfinite(elapsedTime) || elapsedTime <= 0.0f) return;
    player.Update(elapsedTime, stage, camera);
    // 元のManagerのUpdateをそのまま使用。追跡AIなどは追加しない。
    /*EnemyManager::Instance().Update(elapsedTime);
    enemyCombat.Update(elapsedTime, player);*/
    const bool context = ImGui::GetCurrentContext() != nullptr;
    const bool manual = context && !ImGui::GetIO().WantCaptureMouse && ImGui::IsMouseDown(1);
    const float mouseX = manual ? ImGui::GetIO().MouseDelta.x : 0.0f;
    const float mouseY = manual ? ImGui::GetIO().MouseDelta.y : 0.0f;
    followCamera.Update(camera, player.GetPos(), player.GetAngle().y,
        elapsedTime, manual, mouseX, mouseY);
}

void SceneCameraTest::Render(float elapsedTime)
{
    auto& graphics = Graphics::Instance();
    ModelRenderer* renderer = graphics.GetModelRenderer();
    RenderContext rc;
    rc.deviceContext = graphics.GetDeviceContext();
    rc.renderState = graphics.GetRenderState();
    rc.camera = &camera;
    renderer->Render(rc, stage.transform, stage.model.get(), ShaderId::Lambert);
    player.Render(elapsedTime, rc, pause);
    EnemyManager::Instance().Render(rc, renderer);
}

void SceneCameraTest::DrawGUI()
{
    ImGui::SetNextWindowSize(ImVec2(380, 650), ImGuiCond_Once);
    if (ImGui::Begin("SceneCameraTest"))
    {
        ImGui::Checkbox("Pause", &pause);
        ImGui::Text("Alive enemies: %d", static_cast<int>(enemyCombat.GetAliveCount()));
        if (ImGui::Button("Respawn test enemies")) SpawnTestEnemies();
        if (ImGui::CollapsingHeader("Follow camera", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Text("Right mouse drag: limited look / Release: recenter");
            ImGui::DragFloat("Distance", &followCamera.distance, 5.0f, 50.0f, 3000.0f);
            ImGui::DragFloat("Focus height", &followCamera.focusHeight, 1.0f, 0.0f, 500.0f);
            ImGui::DragFloat("Camera pitch (rad)", &followCamera.basePitch, 0.01f, 0.31f, 1.09f);
            ImGui::DragFloat("Turn follow speed", &followCamera.turnSpeed, 0.1f, 0.5f, 15.0f);
            ImGui::DragFloat("Position follow speed", &followCamera.positionSpeed, 0.1f, 1.0f, 30.0f);
        }
        player.DrawGUI();
    }
    ImGui::End();
}
