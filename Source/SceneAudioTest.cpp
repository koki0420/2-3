#pragma once

#include "SceneAudioTest.h"
#include <imgui.h>
#include <ImGuizmo.h>
#include <DirectXCollision.h>
#include "Graphics.h"
#include "Collision.h"


SceneAudioTest::SceneAudioTest()
{
	//p = std::make_unique<Player>();
	//item[0] = std::make_unique<Item>(DirectX::XMFLOAT3(0, 0, 3), 0);

	//ID3D11Device* device = Graphics::Instance().GetDevice();
	//float screenWidth = Graphics::Instance().GetScreenWidth();
	//float screenHeight = Graphics::Instance().GetScreenHeight();

	//// カメラ設定
	//camera.SetPerspectiveFov(
	//	DirectX::XMConvertToRadians(45),	// 画角
	//	screenWidth / screenHeight,			// 画面アスペクト比
	//	0.1f,								// ニアクリップ
	//	1000.0f								// ファークリップ
	//);

	//camera.SetLookAt(
	//	{ 10, 5,3 },		// 視点
	//	{ 13, 5, 16 },	// 注視点
	//	{ 0, 1, 0 }		// 上ベクトル
	//);
	//for (int i = 0;i < ITEM;i += 1)
	//{
	//	p->SetItemType(item[i]->GetItemType(), i);
	//}


	//cameraController.SyncCameraToController(camera);

	//// モデル
	//stage.model = std::make_unique<Model>("Data/Model/Greybox/Greybox.mdl");

	////audio_buffers[0] = std::make_shared<audio_buffer>(L".\\009.wav");
	//audio_buffers[0] = std::make_shared<audio_buffer>(L".\\Data\\Sounds\\SoundEffects\\Bomb.wav");
	////audio_buffers[2] = std::make_shared<audio_buffer>(L".\\footsteps-dry-leaves-g.wav");
	////audio_buffers[3] = std::make_shared<audio_buffer>(L".\\explosion-8-bit.wav");

	////bgm[0] = new audio_source_voice(audio_buffers[0]);
	//se[0] = std::make_unique <audio_source_voice>(audio_buffers[0]);
	////se[1] = new audio_source_voice(audio_buffers[1]);
}

SceneAudioTest::~SceneAudioTest()
{
	// XAUDIO2
	/*for (audio_source_voice* p : bgm)
	{
		delete p;
	}
	for (audio_source_voice* p : se)
	{
		delete p;
	}*/
	//audio_device::uninitialize();
}

void SceneAudioTest::Initialize()
{
	p = std::make_unique<Player>();
	//item[0] = std::make_unique<Item>(DirectX::XMFLOAT3(0, 0, 3), 0);
	//item[1] = std::make_unique<Item>(DirectX::XMFLOAT3(-3, 0, 3), 1);

	ID3D11Device* device = Graphics::Instance().GetDevice();
	float screenWidth = Graphics::Instance().GetScreenWidth();
	float screenHeight = Graphics::Instance().GetScreenHeight();

	// カメラ設定
	camera.SetPerspectiveFov(
		DirectX::XMConvertToRadians(45),	// 画角
		screenWidth / screenHeight,			// 画面アスペクト比
		0.1f,								// ニアクリップ
		1000.0f								// ファークリップ
	);

	camera.SetLookAt(
		{ 10, 5,3 },		// 視点
		{ 13, 5, 16 },	// 注視点
		{ 0, 1, 0 }		// 上ベクトル
	);
	for (int i = 0;i < ITEM;i += 1)
	{
		p->SetItemType(item[i]->GetItemType(), i);
	}


	cameraController.SyncCameraToController(camera);

	// モデル
	stage.model = std::make_unique<Model>("Data/Model/Greybox/Greybox.mdl");

	audio_buffers[0] = std::make_shared<audio_buffer>(L".\\Data\\Sounds\\SoundEffects\\Bomb.wav");
	

	//bgm[0] = new audio_source_voice(audio_buffers[0]);
	se[0] = std::make_unique <audio_source_voice>(audio_buffers[0]);
	//se[1] = new audio_source_voice(audio_buffers[1]);
}

void SceneAudioTest::Finalize()
{
}


void SceneAudioTest::Update(float elapsedTime)
{
	// プレイヤー更新

	for (int i = 0;i < ITEM;i += 1)
	{
		//item[i]->Update();
	}

	p->Update(elapsedTime, stage, camera);

	if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
	{
		ShowCursor(TRUE);
	}
	else
	{
		camera.Update(p->GetPlayerPos(), p->GetCamera());
	}
	if (GetAsyncKeyState('1') & 1)
	{
		/*if (se[0]->queuing())
		{
			se[0]->allStop();
		}
		else
		{
			se[0]->play(255);
		}*/
		se[0]->play(0);
		se[0]->volume(1.0f);
	}

	if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
	{
		ShowCursor(TRUE);
	}
	else
	{
		camera.Update(p->GetPlayerPos(), p->GetCamera());
	}
}

void SceneAudioTest::Render(float elapsedTime)
{
	ID3D11DeviceContext* dc = Graphics::Instance().GetDeviceContext();
	RenderState* renderState = Graphics::Instance().GetRenderState();
	ModelRenderer* modelRenderer = Graphics::Instance().GetModelRenderer();

	// モデル描画
	RenderContext rc;
	rc.deviceContext = dc;
	rc.renderState = renderState;
	rc.camera = &camera;
	modelRenderer->Render(rc, stage.transform, stage.model.get(), ShaderId::Lambert);

	p->Render(elapsedTime, rc,true);
	for (int i = 0;i < ITEM;i += 1)
	{
		item[ITEM]->Render(rc);
	}
}

