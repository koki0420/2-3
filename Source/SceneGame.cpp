#pragma once

#include "SceneGame.h"
#include <imgui.h>
#include <ImGuizmo.h>
#include <DirectXCollision.h>
#include <algorithm>

#include "Graphics.h"
#include "Collision.h"
#include <SceneManager.h>
#include"SceneTitle.h"
#include"SceneResult.h"
#include"SceneLoading.h"

#include <EffectManager.h>

#include "audio.h"
#include "AudioResource.h"




SceneGame::SceneGame()
{
	
}

SceneGame::~SceneGame()
{

	

}

void SceneGame::Initialize()
{

	stage.model = std::make_unique<Model>("Data/Model/Stage/stage4.mdl");

	enemy_num = 0;
	
	stage.scale.x = 0.2f;
	stage.scale.y = 0.2f;
	stage.scale.z = 0.2f;

	stage.position.y = -20;

	boss=std::make_unique<Boss>(DirectX::XMFLOAT3{ 0, 0, 100 });


	cameraController.SyncCameraToController(camera);

	for (int i = 0;i < 10;i += 1)
	{
		for (int j = 0;j < 10;j += 1)
		{
			auto enemy = std::make_unique<Enemy>(
				DirectX::XMFLOAT3{ -260 + 52.0f * j, 0, -260 + 52.0f * i });

			enemies.push_back({ enemy.get(), enemyMaxHP });
			EnemyManager::Instance().Register(std::move(enemy));
			++enemy_num;
		}
	}

	
	
	if(boss->Clear())
	{
		SceneManager::Instance().ChangeScene(new SceneLoading(new SceneResult));
	}
	
	player.Initialize();
	SpriteOVER = std::make_unique<Sprite>(Graphics::Instance().GetDevice(), "Data/Sprite/gameover_transparent.png");//
	EnemyManager::Instance().Target(player.GetPosition());
}	

void SceneGame::Finalize()
{
	enemies.clear();
	EnemyManager::Instance().Clear();
}

void SceneGame::Update(float elapsedTime)
{
	EnemyManager::Instance().Target(player.GetPosition());
	EnemyManager::Instance().Update(elapsedTime);
	stage.UpdateTransform();
	player.Update(elapsedTime, stage, camera);


	DirectX::XMFLOAT3 focus = player.GetPosition();
	focus.y += 10.0f;
	cameraController.SetFocus(focus);
	cameraController.Update();
	cameraController.SyncControllerToCamera(camera);


	if (Boss_Dead)
	{
		boss->Boss_Dead(Boss_Dead);
	

		if (!(boss->Boss_Alive()))
		{
			boss->Boss_Chage();
			Boss_Dead = false;
			if (!(boss->Clear()))Respawn();
		}
	}
	{
		if (enemy_num > 0)
		{
			boss->Boss_Set(false);
		}
		else
		{
			boss->Boss_Set(true);
			boss->Update(elapsedTime);
		}
	}

	
	//敵へのダメージ
	for (size_t i = 0;i < enemies.size();++i)
	{
		auto& target = enemies[i];
		if (!target.enemy->Alive())continue;
		if (player.CheckAttackHit(i, GetEnemyHitCenter(*target.enemy),
			enemyHitRadius))
		{
			target.hp -= player.GetPAtkDmg();
			if (target.hp <= 0.0f)DefeatEnemy(i);
		}
	}

	EffectManager::Instance().Update(elapsedTime);

	if (boss->Clear())
	{
		if(clearTimer>7)
		{
			SceneManager::Instance().ChangeScene(new SceneResult);
		}
		clearTimer += elapsedTime;
	}
	if (player.GetPlayerHP()<=0)
	{
		if(clearTimer>7)
		{
			SceneManager::Instance().ChangeScene(new SceneResult);
		}
		clearTimer += elapsedTime;
	}
}

void SceneGame::Render(float elapsedTime)
{
	ID3D11DeviceContext* dc = Graphics::Instance().GetDeviceContext();
	RenderState* renderState = Graphics::Instance().GetRenderState();
	ModelRenderer* modelRenderer = Graphics::Instance().GetModelRenderer();

	ShapeRenderer* shapeRenderer = Graphics::Instance().GetShapeRenderer();

	RenderContext rc;
	rc.deviceContext = dc;

	rc.renderState = renderState;
	rc.camera = &camera;

	DirectX::XMFLOAT4X4 vp;

	DirectX::XMStoreFloat4x4(&vp,
		DirectX::XMLoadFloat4x4(&camera.GetView()) *
		DirectX::XMLoadFloat4x4(&camera.GetProjection()));

	modelRenderer->Render(rc, stage.transform, stage.model.get(), ShaderId::Lambert);
	EnemyManager::Instance().Render(rc, modelRenderer);

	EffectManager::Instance().Render(rc.camera->GetView(), rc.camera->GetProjection());

	player.Render(elapsedTime, rc);

	boss->Render(rc, modelRenderer);


	shapeRenderer->Render(
		rc.deviceContext,
		rc.camera->GetView(),
		rc.camera->GetProjection()
	);


	if (showEnemyHitBoxes)
	{
		for (const auto& target : enemies)
		{
			if (!target.enemy->Alive()) continue;
			shapeRenderer->DrawSphere(
				GetEnemyHitCenter(*target.enemy), enemyHitRadius, { 0, 1, 0, 1 });
		}
	}

	{
		player.UIRender(elapsedTime, dc);
		if (player.GetPlayerHP() <= 0)
		{
			SpriteOVER->Render(dc, 300, 300, 0, 600, 100, 0, 1, 1, 1, min(1, clearTimer / 2));
		}
	}


}

void SceneGame::DrawGUI()
{
	ImVec2 displaySize = ImGui::GetIO().DisplaySize;
	ImVec2 pos = ImGui::GetMainViewport()->GetWorkPos();
	float width = 210;
	float height = 460;
	ImGui::SetNextWindowPos(ImVec2(pos.x + 10, pos.y + 10), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiCond_Once);

	if (ImGui::Begin("SceneGame"))
	{
		if (ImGui::Button("enemy"))
		{
			for (size_t i = 0; i < enemies.size(); ++i)
			{
				if (!enemies[i].enemy->Alive()) continue;
				DefeatEnemy(i);
				break;
			}
		}
		if (ImGui::Button("all_enemy"))
		{
			for (size_t i = 0; i < enemies.size(); ++i) DefeatEnemy(i);
		}
		ImGui::Text("Enemies alive: %d", enemy_num);
		ImGui::Checkbox("Show enemy hitboxes", &showEnemyHitBoxes);
		ImGui::SliderFloat("Enemy hit radius", &enemyHitRadius, 0.1f, 30.0f);
		ImGui::SliderFloat("Enemy hit height", &enemyHitHeight, 0.0f, 30.0f);
		if (ImGui::Button("boss"))
		{
			if (boss->Boss_Alive()&&enemy_num<=0)
			{
				Boss_Dead = true;
			}
		}

		ImGui::Separator();

		if (ImGui::CollapsingHeader("Camera",
			ImGuiTreeNodeFlags_DefaultOpen))
		{
			// ビュー行列の逆行列から、カメラのワールド座標を求める
			DirectX::XMFLOAT4X4 view = camera.GetView();

			DirectX::XMMATRIX inverseView = DirectX::XMMatrixInverse(
				nullptr,
				DirectX::XMLoadFloat4x4(&view));

			DirectX::XMFLOAT3 cameraPosition;

			DirectX::XMStoreFloat3(
				&cameraPosition,
				inverseView.r[3]);

			DirectX::XMFLOAT3 front = camera.GetFront();
			DirectX::XMFLOAT3 right = camera.GetRight();

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

			DirectX::XMFLOAT3 playerPosition = player.GetPosition();

			float dx = cameraPosition.x - playerPosition.x;
			float dy = cameraPosition.y - playerPosition.y;
			float dz = cameraPosition.z - playerPosition.z;

			float distance = sqrtf(dx * dx + dy * dy + dz * dz);

			ImGui::Text("Distance to player: %.2f", distance);
		}
		
		player.DrawGUI();


	}
	ImGui::End();
}

DirectX::XMFLOAT3 SceneGame::GetEnemyHitCenter(const Enemy& enemy)const
{
	auto center = enemy.GetPos();
	center.y += enemyHitHeight;
	return center;
}

void SceneGame::DefeatEnemy(size_t index)
{
	auto& target = enemies[index];
	if (!target.enemy->Alive())return;

	target.hp = 0.0f;
	EnemyManager::Instance().Enemy_Set(static_cast<int>(index),false);
	--enemy_num;

}

void SceneGame::Respawn()
{
	enemy_num = 0;
	for (size_t i = 0; i < enemies.size(); i++)
	{
		enemies[i].hp = enemyMaxHP;
		EnemyManager::Instance().Enemy_Set(static_cast<int>(i), true);
		enemy_num += 1;
	}

}

