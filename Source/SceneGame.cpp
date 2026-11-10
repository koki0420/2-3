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

	boss.BOSS=std::make_unique<Boss>(DirectX::XMFLOAT3{ 0, 0, 100 });
	boss.hp = 100.0f;

	cameraController.SyncCameraToController(camera);

	DirectX::XMFLOAT3 position;
	for (int i = 0;i < 16;i += 1)
	{
		for (int j = 0;j < 16;j += 1)
		{
			position = { -260.0f + (520.0f / 15.0f) * i,-25,-260.0f + (520.0f / 15.0f) * j };
			auto enemy = std::make_unique<Enemy>(position);

			 enemies.push_back({ enemy.get(), enemyMaxHP });
			 EnemyManager::Instance().Register(std::move(enemy));
			 ++enemy_num;
		}
	}
	
	if(boss.BOSS->Clear())
	{
		SceneManager::Instance().ChangeScene(new SceneLoading(new SceneResult));
	}
	
	player.Initialize();

	cameraController.Reset(player.GetPosition(), player.GetAngle().y);

	cameraController.SyncControllerToCamera(camera);

	SpriteOVER = std::make_unique<Sprite>(Graphics::Instance().GetDevice(), "Data/Sprite/gameover_transparent.png");//
	SpriteCLEAR = std::make_unique<Sprite>(Graphics::Instance().GetDevice(), "Data/Sprite/gameclear_smooth_transparent.png");//
	EnemyManager::Instance().Target(player.GetPosition());

	HitEffect = EffectManager::Instance().CreateEffect("Data/Effect/Hit.efkefc");
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


	cameraController.SetFocus(player.GetPosition());

	cameraController.Update(elapsedTime, player.GetAngle().y);

	cameraController.SyncControllerToCamera(camera);

	boss.BOSS->Update(elapsedTime);

	if (Boss_Dead)
	{
		boss.BOSS->Boss_Dead(Boss_Dead);
		if (boss.hp <= 0)
		{
			boss.hp = 100.0f;
		}
		if (!(boss.BOSS->Boss_Alive()))
		{
			Respawn();
			boss.BOSS->Boss_Chage();
			Boss_Dead = false;
		}
	}
	else
	{
		if (enemy_num > 0)
		{
			boss.BOSS->Boss_Set(false);
		}
		else 
		{
			boss.BOSS->Boss_Set(true);	
		}
	}

	enemy_num = 0;
	for (size_t i = 0;i < enemies.size();++i)
	{
		auto& target = enemies[i];
		//“G‚Ö‚Ìƒ_ƒ[ƒW
		if (!target.enemy->Alive())
		{	
			continue;
		}
		if (player.CheckAttackHit(i, GetEnemyHitCenter(*target.enemy),
			enemyHitRadius))
		{
			target.enemy->SetKnockBack(true);
			target.hp -= player.GetPAtkDmg();
			DirectX::XMFLOAT3 epos = target.enemy->GetPos();
			HitEffect->Play({epos.x,epos.y+10.0f,epos.z}, 5.0f);
			if (target.hp <= 0.0f)
			{
				DefeatEnemy(i);
				player.AddBlood(10.0f);
				
			}
		}

		//“G‚©‚ç‚ÌUŒ‚

		if (target.enemy->GetEnemyAnimation()->IsPlaying("Punch")
			&&target.enemy->GetEnemyAnimation()->GetCurrentFrame() >= 24&&
			target.enemy->GetEnemyAnimation()->GetCurrentFrame() <=48)
		{
			player.CheckTakeAttack(target.enemy->GetPos(), 20.0f);
		}

		if (EnemyManager::Instance().Enemy_Alive(i))
		{
			enemy_num += 1;
		}
	}

	//ƒ{ƒX‚Ö‚Ìƒ_ƒ[ƒW
	if (boss.BOSS->Boss_Alive())
	{
		if (player.CheckAttackHit(0, boss.BOSS->GetPos(),
			40.0f))
		{
			boss.hp -= player.GetPAtkDmg();
			HitEffect->Play(boss.BOSS->GetPos(),1.0f);
			if (boss.hp <= 0.0f)
			{
				if (Boss_Dead = false)boss.hp = 1000.0f;
				Boss_Dead = true;
			}
		}
		if (boss.hp <= 0.0f)
		{
			Boss_Dead = true;
		}
		//“G‚©‚ç‚ÌUŒ‚
		if (boss.BOSS->GetMiddleAnimation()->IsPlaying("Jump")
			&& boss.BOSS->GetMiddleAnimation()->GetCurrentFrame() >= 70 &&
			boss.BOSS->GetMiddleAnimation()->GetCurrentFrame() <= 90)
		{
			player.CheckTakeAttack(boss.BOSS->GetPos(), 50.0f);
		}

		//“G‚©‚ç‚ÌUŒ‚
		if (boss.BOSS->GetBossAnimation()->IsPlaying("Attack")
			&& boss.BOSS->GetBossAnimation()->GetCurrentFrame() >= 36 &&
			boss.BOSS->GetBossAnimation()->GetCurrentFrame() <= 45)
		{
			player.CheckTakeAttack(boss.BOSS->GetPos(), 50.0f);
		}

		//“G‚©‚ç‚ÌUŒ‚
		if (boss.BOSS->GetBossAnimation()->IsPlaying("Jump_Attack")
			&& boss.BOSS->GetBossAnimation()->GetCurrentFrame() >= 48 &&
			boss.BOSS->GetBossAnimation()->GetCurrentFrame() <= 70)
		{
			player.CheckTakeAttack(boss.BOSS->GetPos(), 50.0f);
		}

	}
	EffectManager::Instance().Update(elapsedTime);

	if (boss.BOSS->Clear())
	{
		if(clearTimer>4)
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

	rc.high_light = true;

	rc.highlightColor = { 0.1, 0.05f, 0.0, 1 };
	modelRenderer->Render(rc, stage.transform, stage.model.get(), ShaderId::Lambert);

	rc.highlightColor = { 0.1, 0.1, 0.2, 1 };
	EnemyManager::Instance().Render(rc, modelRenderer);

	EffectManager::Instance().Render(rc.camera->GetView(), rc.camera->GetProjection());


	rc.highlightColor = { 0.2, 0.2, 0.2, 1 };
	player.Render(elapsedTime, rc);

	rc.highlightColor = { 0.5, 0.0, 0.5, 1 };
	boss.BOSS->Render(rc, modelRenderer);


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
		if (boss.BOSS->Clear())
		{
			SpriteCLEAR->Render(dc, 320, 300, 0, 600, 100, 0, 1, 1, 1, min(1, clearTimer / 2));
		}
		if (player.GetPlayerHP() <= 0)
		{
			SpriteOVER->Render(dc, 320, 300, 0, 600, 100, 0, 1, 1, 1, min(1, clearTimer / 2));
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
			if (boss.BOSS->Boss_Alive()&&enemy_num<=0)
			{
				boss.hp = 0.0f;
				Boss_Dead = true;
			}
		}
		ImGui::Separator();
		ImGui::Text("Boss");
		ImGui::Text("HP: %.0f", boss.hp);

		ImGui::Separator();

		if (ImGui::CollapsingHeader("Camera",
			ImGuiTreeNodeFlags_DefaultOpen))
		{
			// ƒrƒ…[s—ñ‚Ì‹ts—ñ‚©‚çAƒJƒƒ‰‚Ìƒ[ƒ‹ƒhÀ•W‚ð‹‚ß‚é
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
	
	if (!(target.enemy->Alive()))
	{
		return;
	}

	
	target.hp = 0.0f;
	EnemyManager::Instance().Enemy_Set(static_cast<int>(index),false);
}

void SceneGame::Respawn()
{
	if (!(boss.BOSS->Clear()))
	{
		enemy_num = 0;
		for (size_t i = 0; i < enemies.size(); i++)
		{
			enemies[i].hp = enemyMaxHP;
			EnemyManager::Instance().Enemy_Set(static_cast<int>(i), true);
		}
	}
	else
	{
		enemy_num += 1;
	}
}

