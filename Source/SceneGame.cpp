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

#include"ItemManager.h"
#include"GimmickManager.h"
#include "audio.h"
#include "AudioResource.h"
#include "SoundSource.h"

SceneGame::SceneGame()
{
	//ID3D11Device* device = Graphics::Instance().GetDevice();
	//float screenWidth = Graphics::Instance().GetScreenWidth();
	//float screenHeight = Graphics::Instance().GetScreenHeight();

	//p = std::make_unique<Player>();
	//item[0] = std::make_unique<Item>(DirectX::XMFLOAT3(0, 0, 3), 0);
	//item[1] = std::make_unique<Item>(DirectX::XMFLOAT3(-3, 0, 3), 1);

	//gimmick[0]= std::make_unique<Gimmick>(DirectX::XMFLOAT3(-6, 0, 3), 0);

	//for (int i = 0;i < ITEM;i += 1)
	//{
	//	p->SetItemType(item[i]->GetItemType(),i);
	//}
	//for (int i = 0;i < GIMMICK;i += 1)
	//{
	//	p->SetGimmickType(gimmick[i]->GetGimmickType(), i);
	//}

	//// �J�����ݒ�
	//camera.SetPerspectiveFov(
	//	DirectX::XMConvertToRadians(45),	// ��p
	//	screenWidth / screenHeight,			// ��ʃA�X�y�N�g��
	//	0.1f,								// �j�A�N���b�v
	//	1000.0f								// �t�@�[�N���b�v
	//);

	//camera.SetLookAt(
	//	{10, 5,3 },		// ���_
	//	{ 13, 5, 16 },	// �����_
	//	{ 0, 1, 0 }		// ��x�N�g��
	//);

	//cameraController.SyncCameraToController(camera);

	//stage.scale.x = 0.1f;
	//stage.scale.y = 0.1f;
	//stage.scale.z = 0.1f;
	//// ���f��
	//stage.model = std::make_unique<Model>("Data/Model/Stage/stage_plane.mdl");
	//
	//// XAUDIO2
	////audio_buffers[0] = std::make_shared<audio_buffer>(L".\\009.wav");
	//audio_buffers[1] = std::make_shared<audio_buffer>(L".\\Data\\Sounds\\SoundEffects\\Bomb.wav");
	////audio_buffers[2] = std::make_shared<audio_buffer>(L".\\footsteps-dry-leaves-g.wav");
	////audio_buffers[3] = std::make_shared<audio_buffer>(L".\\explosion-8-bit.wav");

	////bgm[0] = new audio_source_voice(audio_buffers[0]);
	////se[0] = new audio_source_voice(audio_buffers[1]);
	////se[1] = new audio_source_voice(audio_buffers[1]);


}

SceneGame::~SceneGame()
{
	for (int i = 0; i < ITEM; i++)
	{
		ItemManager::instance().Unregister(item[i].get());
	}
	for (int i = 0; i < GIMMICK; i++)
	{
		GimmickManager::instance().Unregister(gimmick[i].get());
	}
}

void SceneGame::Initialize()
{
	ID3D11Device* device = Graphics::Instance().GetDevice();
	float screenWidth = Graphics::Instance().GetScreenWidth();
	float screenHeight = Graphics::Instance().GetScreenHeight();

	p = std::make_unique<Player>();
	//ITEM; //���m�F�p
	DirectX::XMFLOAT3 itemPos[ITEM] =
	{
		//�����̈ʒu����A�����̍����������ʒu�������ݒ肷��
		{soundSourcesPos[0].x, soundSourcesPos[0].y + 1.53f,soundSourcesPos[0].z},	// TV
		{soundSourcesPos[1].x, soundSourcesPos[1].y + 1.53f,soundSourcesPos[1].z},	// TV
		{soundSourcesPos[2].x, soundSourcesPos[2].y + 1.53f,soundSourcesPos[2].z},	// TV
		{soundSourcesPos[3].x, soundSourcesPos[3].y + 1.53f,soundSourcesPos[3].z},	//Gramophone
		{soundSourcesPos[4].x, soundSourcesPos[4].y + 1.53f,soundSourcesPos[4].z},	//Gramophone
		{soundSourcesPos[5].x, soundSourcesPos[5].y + 1.53f,soundSourcesPos[5].z},	//Gramophone
		{soundSourcesPos[6].x, soundSourcesPos[6].y + 1.53f,soundSourcesPos[6].z},	//Gramophone
	};
	item[0] = std::make_unique<Item>(itemPos[0], Interact::InteractionType::Bomb);
	item[1] = std::make_unique<Item>(itemPos[1], Interact::InteractionType::Slash);
	item[2] = std::make_unique<Item>(itemPos[2], Interact::InteractionType::Flash); //dammy
	item[3] = std::make_unique<Item>(itemPos[3], Interact::InteractionType::Smash);
	item[4] = std::make_unique<Item>(itemPos[4], Interact::InteractionType::Slash); //dammy
	item[5] = std::make_unique<Item>(itemPos[5], Interact::InteractionType::Slash);
	item[6] = std::make_unique<Item>(itemPos[6], Interact::InteractionType::Bomb);
	//GIMMICK; //���m�F�p
	gimmick[0] = std::make_unique<Gimmick>(DirectX::XMFLOAT3(-10,	0, 25),	Gimmick::GimmickType::BreakableWall); //�������̓M�~�b�N���Ƃ̃^�C�v
	gimmick[1] = std::make_unique<Gimmick>(DirectX::XMFLOAT3(11,	0, 25.2f), Gimmick::GimmickType::BreakableWall);
	gimmick[2] = std::make_unique<Gimmick>(DirectX::XMFLOAT3(0 ,		0, -10), Gimmick::GimmickType::MovableContainer);
	gimmick[3] = std::make_unique<Gimmick>(DirectX::XMFLOAT3(10.5f, 0, 15), Gimmick::GimmickType::KnockedDownBridge);
	gimmick[4] = std::make_unique<Gimmick>(DirectX::XMFLOAT3(itemPos[0].x-3, 0, itemPos[0].z-2), Gimmick::GimmickType::StringedUpRope);
	gimmick[5] = std::make_unique<Gimmick>(DirectX::XMFLOAT3(10.5f, 1.0f, 0), Gimmick::GimmickType::StringedUpRope);
	gimmick[6] = std::make_unique<Gimmick>(DirectX::XMFLOAT3(10.5f, 1.2f, -2), Gimmick::GimmickType::DroppedBridge);
	

	//����
	//SOUND_SOURCE_NUM; //���m�F�p
	DirectX::XMFLOAT3 ssAngle[4] =
	{
		{0, 0,					0},							//������
		{0, DirectX::XM_PIDIV2,	0},							//�E����
		{0, DirectX::XM_PI,		0},							//�O����
		{0, DirectX::XM_PI + DirectX::XM_PIDIV2,	0},		//������
	};
	soundSources[0] = std::make_unique<SoundSource>(soundSourcesPos[0], ssAngle[3],SoundSource::ModelType::TV);			//������
	soundSources[1] = std::make_unique<SoundSource>(soundSourcesPos[1], ssAngle[3],SoundSource::ModelType::TV);			//������
	soundSources[2] = std::make_unique<SoundSource>(soundSourcesPos[2], ssAngle[3],SoundSource::ModelType::TV);			//������
	soundSources[3] = std::make_unique<SoundSource>(soundSourcesPos[3], ssAngle[1],SoundSource::ModelType::Gramophone);	//�E����
	soundSources[4] = std::make_unique<SoundSource>(soundSourcesPos[4], ssAngle[1],SoundSource::ModelType::Gramophone);	//�E����
	soundSources[5] = std::make_unique<SoundSource>(soundSourcesPos[5], ssAngle[2],SoundSource::ModelType::Gramophone);	//�O����
	soundSources[6] = std::make_unique<SoundSource>(soundSourcesPos[6], ssAngle[2],SoundSource::ModelType::Gramophone);	//�O����

	for (int i = 0;i < ITEM;i += 1)
	{
		p->SetItemType(item[i]->GetItemType(), i);
		ItemManager::instance().Register(item[i].get());
	}
	for (int i = 0;i < GIMMICK;i += 1)
	{
		//�M�~�b�N�ɑΉ�����A�C�e���̎�ނ�o�^
		p->SetGimmickType(gimmick[i]->GetTriggerItemType(), i);
		GimmickManager::instance().Register(gimmick[i].get());
	}

	for (int i = 0; i < LIGHT_MAX; i++)
	{
		Light[i].model = std::make_unique<Model>("Data/Model/object/Light.mdl");
	}

	p->Initialize();

	// �J�����ݒ�
	camera.SetPerspectiveFov(
		DirectX::XMConvertToRadians(45),	// ��p
		screenWidth / screenHeight,			// ��ʃA�X�y�N�g��
		0.1f,								// �j�A�N���b�v
		1000.0f								// �t�@�[�N���b�v
	);

	camera.SetLookAt(
		{ 10, 5,3 },		// ���_
		{ 13, 5, 16 },	// �����_
		{ 0, 1, 0 }		// ��x�N�g��
	);

	cameraController.SyncCameraToController(camera);

	stage.scale.x = 0.1f;
	stage.scale.y = 0.1f;
	stage.scale.z = 0.1f;
	// ���f��
	stage.model = std::make_unique<Model>("Data/Model/Stage/map(comp).mdl");

	pauseSprite = std::make_unique<Sprite>(Graphics::Instance().GetDevice());
	GameSprite = std::make_unique<Sprite>(Graphics::Instance().GetDevice(),"Data/Sprite/Resume_game.png");
	TitleSprite = std::make_unique<Sprite>(Graphics::Instance().GetDevice(),"Data/Sprite/Return_to_Title.png");
	ReTrySprite = std::make_unique<Sprite>(Graphics::Instance().GetDevice(),"Data/Sprite/Reset_Game.png");

	// XAUDIO2
	//�A�C�e���̌��ʉ�
	AudioResource::audio_buffers[AudioResourceNumber::ItemBomb] = std::make_shared<audio_buffer>(L".\\Data\\Sounds\\SoundEffects\\Bomb.wav");	//wav�̐��������
	AudioResource::audio_buffers[AudioResourceNumber::ItemSlash] = std::make_shared<audio_buffer>(L".\\Data\\Sounds\\SoundEffects\\Slash.wav");
	AudioResource::audio_buffers[AudioResourceNumber::ItemSmash] = std::make_shared<audio_buffer>(L".\\Data\\Sounds\\SoundEffects\\Smash.wav");
	AudioResource::audio_buffers[AudioResourceNumber::ItemFlash] = std::make_shared<audio_buffer>(L".\\Data\\Sounds\\SoundEffects\\Flash.wav");
	//�V�X�e����
	AudioResource::audio_buffers[AudioResourceNumber::SystemOpenPause] = std::make_shared<audio_buffer>(L".\\Data\\Sounds\\SoundEffects\\OpenPause.wav");
	AudioResource::audio_buffers[AudioResourceNumber::SystemSlectButton] = std::make_shared<audio_buffer>(L".\\Data\\Sounds\\SoundEffects\\SlectButton.wav");
	//se[0] = std::make_unique <audio_source_voice>(audio_buffers[0]);	//�A�C�e���̐��������
	//se[1] = std::make_unique <audio_source_voice>(audio_buffers[1]);
	//se[0] = std::make_unique <audio_source_voice>(audio_buffers[2]);
	
	for (int i = 0; i < Item::ITEM_TYPE_NUM; i++) //player->having_item_type�p
	{
		sePlayer[i] = std::make_unique <audio_source_voice>(AudioResource::audio_buffers[i]);
		p->SetSE(sePlayer[i].get(), i);
	}
	for (int i = 0; i < ITEM; i++) // item->se�p
	{
		seItem[i] = std::make_unique <audio_source_voice>(AudioResource::audio_buffers[item[i]->GetItemType()]);
		item[i]->SetSE(seItem[i].get());
	}
	// i��audio_buffers�̉��Ԗڂ��A����seSystem�̉��Ԗڂ�
	for (int i = Item::ITEM_TYPE_NUM, j = 0; j < AudioResourceNumber::CountNum - Item::ITEM_TYPE_NUM; i++, j++)
	{
		seSystem[j] = std::make_unique <audio_source_voice>(AudioResource::audio_buffers[i]);
	}
	//item[0]->SetSE(se[0].get());
	//item[1]->SetSE(se[1].get());

	//���C�g�̈ʒu��ݒ�
	Light[0].position = { 12,6,42 };
	Light[1].position = { 12,6,32 };
	
	//�R���e�i
	Light[3].position = { 8,6,-18 };

	//�L��
	Light[4].position = { -2,5.75f,-15 };
	Light[5].position = { -4,5.75f,12 };
	Light[6].position = { -14,5.75f,10 };
	Light[7].position = { -14,5.75f,20 };
	Light[8].position = { -14,5.75f,0 };
	Light[9].position = { -14,5.75f,-10 };
	Light[10].position = { -7,5.75f,-5 };
	Light[11].position = { 0,5.75f,0 };
	Light[12].position = { 2,5.75f,20 };

	Light[17].position = { 0,100,0 };
	Light[19].position = { 0,100,0 };

	//��
	Light[13].position = { 11,9,-8 };
	Light[2].position = { 11,9, 3 };
	Light[14].position = { 11,9,15 };

	//����
	Light[15].position = { -23,6.0f,5.5f };
	Light[16].position = { -12,6.0f,-18 };


	Light[18].position = { -9,5.75f,27 };

	goalPos = { -11,2,35 };
	goalEffect = EffectManager::Instance().CreateEffect("Data/Effect/Goal_Light.efkefc");
}	

void SceneGame::Finalize()
{

}

void SceneGame::Update(float elapsedTime)
{

	POINT mousePos;
	GetCursorPos(&mousePos); // ��ʑS�̂̍��W�i�X�N���[�����W�j

	// �Q�[���E�B���h�E���̍��W�ɕϊ�
	ScreenToClient(Graphics::Instance().GetWindowHandle(), &mousePos);

	// mousePos.x, mousePos.y ���E�B���h�E���̃}�E�X���W
	int mx = mousePos.x;
	int my = mousePos.y;

	// 1F�O�̍��N���b�N���͏��
	static bool prev = false;
	//���݂̍��N���b�N���͏��
	bool now = (GetAsyncKeyState(VK_LBUTTON) & 0x8000);


	//���C�g�X�V
	for (int i = 0; i < LIGHT_MAX; i++)
	{
		Light[i].scale = { 0.1f, 0.1f, 0.1f };

		Light[i].UpdateTransform();
	}

	if (pause)
	{
		while (ShowCursor(TRUE) <= 0) {}
		float S_W = Graphics::Instance().GetScreenWidth();
		float S_H = Graphics::Instance().GetScreenHeight();
		
		/*ReTrySprite->Render(dc, S_W / 4, S_H / 70, 0, 0.5f * S_W, 0.4f * S_H, 0, 0, 0, 1, ReTry_alpha);

		GameSprite->Render(dc, S_W / 4, S_H / 3.6f, 0, 0.5f * S_W, 0.4f * S_H, 0, 1, 0, 0, game_alpha);

		TitleSprite->Render(dc, S_W / 4, S_H / 1.8f, 0, 0.5f * S_W, 0.4f * S_H, 1, 1, 1, 0, title_alpha);*/

		//���g���CUI�͈͓̔��Ȃ�
		if (mx >= S_W / 4    && mx <= S_W / 4 + 0.5f * S_W &&
			my >= S_H / 10 && my <= S_H / 10 + 0.25f * S_H)
		{
			////�PF�O�ɑI�𒆂łȂ������Ȃ��
			//if (!isRetrySelected)
			//{
			//	//�����Ă���SystemSlectButton��SE���~�߂�
			//	seSystem[AudioResourceNumber::SystemSlectButton - Item::ITEM_TYPE_NUM]->stop();
			//	//�V�����炷
			//	seSystem[AudioResourceNumber::SystemSlectButton - Item::ITEM_TYPE_NUM]->play();
			//}
			ReTry_alpha = 0.8f;
			if (now)
			{
				retryFlag = true;
				//SceneManager::Instance().ChangeScene(new SceneLoading(new SceneGame));
			}
			// 1F��̃t���O���X�V
			//isRetrySelected = true;
		}
		else
		{
			// 1F��̃t���O���X�V
			//isRetrySelected = false;
			retryFlag = false;
			ReTry_alpha = 0.4f;
		}
		// ���g���C�t���O�������Ă����
		if (retryFlag)
		{
			//�L�[���������ƃ��g���C
			if (!(now) && prev)
			{
				SceneManager::Instance().ChangeScene(new SceneLoading(new SceneGame));
			}
		}

		
		if (mx >= S_W / 4          && mx <= S_W / 4 + 0.5f * S_W &&
			my >= S_H / 2.6f  && my <= S_H / 2.6f + 0.23f * S_H)
		{
			////�PF�O�ɑI�𒆂łȂ������Ȃ��
			//if (!isResumeSelected)
			//{
			//	//�����Ă���SystemSlectButton��SE���~�߂�
			//	seSystem[AudioResourceNumber::SystemSlectButton - Item::ITEM_TYPE_NUM]->stop();
			//	//�V�����炷
			//	seSystem[AudioResourceNumber::SystemSlectButton - Item::ITEM_TYPE_NUM]->play();
			//}
			game_alpha = 0.8f;
			if (now)
			{
				resumeFlag = true;
				//pause = false;
				//pause_flag = true;
			}
			//isResumeSelected = true;
		}
		else 
		{
			//isResumeSelected = false;
			resumeFlag = false;
			game_alpha = 0.4f;
		}
		if (resumeFlag)
		{
			//�L�[����������
			if (!(now) && prev)
			{
				pause = false;
				pause_flag = true;
			}	
		}

		if (mx >= S_W / 4 && mx <= S_W / 4 + 0.5f * S_W &&
			my >= S_H / 1.6f && my <= S_H / 1.6f + 0.25f * S_H)
		{
			////�PF�O�ɑI�𒆂łȂ������Ȃ��
			//if (!isToTitleSelected)
			//{
			//	//�����Ă���SystemSlectButton��SE���~�߂�
			//	seSystem[AudioResourceNumber::SystemSlectButton - Item::ITEM_TYPE_NUM]->stop();
			//	//�V�����炷
			//	seSystem[AudioResourceNumber::SystemSlectButton - Item::ITEM_TYPE_NUM]->play();
			//}
			title_alpha = 0.8f;
			if (now)
			{
				toTitleFlag = true;
				//SceneManager::Instance().ChangeScene(new SceneLoading(new SceneTitle));
			}
			//isToTitleSelected = true;
		}
		else 
		{
			//isToTitleSelected = false;
			toTitleFlag = false;
			title_alpha = 0.4f;
		}
		if (toTitleFlag)
		{
			if (!(now) && prev)
			{
				SceneManager::Instance().ChangeScene(new SceneTitle);
			}
		}

		static bool PAUSE = false;
		if (PAUSE)
		{
			static bool prev = false;
			bool now = (GetAsyncKeyState(VK_ESCAPE) & 0x8000);
			if (!(now) && prev)
			{
				pause = false;
				PAUSE = false;
				
			}
			prev = now;
		}
		else if (!(GetAsyncKeyState(VK_ESCAPE) & 0x8000))
		{
			PAUSE = true;
		}	
	}
	else
	{
		static bool prev = false;
		bool now = (GetAsyncKeyState(VK_ESCAPE) & 0x8000);
		if (!(now) && prev)
		{
			pause = true;
		
		}
		prev = now;
	}
	if (!(GetAsyncKeyState(VK_LBUTTON) & 0x8000))
	{
		pause_flag = false;
	}
	
	if(!(pause_flag)&&!(pause))
	{
		while (ShowCursor(FALSE) >= 0) {}
		if(!(GetAsyncKeyState(VK_SHIFT) & 0x8000))
		{
			camera.Update(p->GetPlayerPos(), p->GetCamera());
			
			for (int i = 0;i < ITEM;i += 1)
			{
				item[i]->SetAlive(p->GetAliveitem(i));
				p->SetItem(item[i]->GetItem(), i);
			}
			ItemManager::instance().Update(p->GetPlayerPos());
			for (int i = 0;i < GIMMICK;i += 1)
			{
				gimmick[i]->SetFallGimmick(p->CutRopeGimmick());
				gimmick[i]->SetAlive(p->GetAliveGimmick(i));
				p->SetGimmick(gimmick[i]->GetGimmick(), i);
			}
			GimmickManager::instance().Update(elapsedTime);
			// �v���C���[�X�V
			p->Update(elapsedTime, stage, camera);
			stage.UpdateTransform();

			
			if (p->CutRopeItem())
			{
				if (soundSources[0]->SoundPos())
				{
					item[0].get()->SetItemPosY(1.53f);
				}
				else
				{
					soundSources[0]->SubPosition(0.05f);
					item[0].get()->SubItemPosY(-0.05f);
				}
			}

			//�����X�V
			int i = 0;
			for (auto& ss : soundSources)
			{
				if (!ss) continue;

				//��ނ��ƂɃT�C�Y�ύX
				if(ss->GetModelType() == SoundSource::ModelType::Gramophone)
				{
					ss->SetSize(gramoSizeGUI);
				}
				if(ss->GetModelType() == SoundSource::ModelType::TV)
				{
					ss->SetSize(TVSizeGUI);
				}

				ss->SetPosition(soundSourcesPos[i]);
				
				ss->Update();
				i++;
			}
			

		

			// SE����
			//�v���C���[�̌���
			//float playerDirX = sinf(p->GetYaw());
			//float playerDirZ = cosf(p->GetYaw());
			//�v���C���[�̑O�����x�N�g��
			DirectX::XMVECTOR playerFrontVec{ sinf(p->GetYaw()) , 0, cosf(p->GetYaw()) }; //�v���C���[��y���ɑ΂��Ă�����]�����Ȃ����߂�������0
			//�v���C���[�̏�����x�N�g��
			DirectX::XMVECTOR playerUpVec{ 0, 1, 0 }; //��ɏ����
			//�v���C���[�̉E�����x�N�g��
			DirectX::XMVECTOR playerRightVec = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(playerUpVec, playerFrontVec));
			// Item��SE
			for (int i = 0; i < ITEM; i++)
			{
				//float toPlayerVecX{}, toPlayerVecZ{}; //���������A�p���̌v�Z�p�ɋL��
				DirectX::XMFLOAT3 toPlayerVec;
				// sqrtf()���g���������͏d�����߁A��x�v�Z�������ʂ��L�����Ă���
				float playerToItemLength = culcPlayerToItemLength(item[i].get(), toPlayerVec);

				//���ȏ㋗��������Ă����SE�͍Đ����Ȃ�
				if (playerToItemLength < HEAR_SE_RANGE_MAX)
				{
					//�A�C�e�������ɉ����Đ�����
					item[i]->GetSE()->play(0); //�A�C�e���������ɏC������
					
					//�p��------------------------------------------
					//SetSEPan(item[i].get(), toPlayerVecX, toPlayerVecZ, playerToItemLengthXZ, i);									

					float panValue;
					DirectX::XMStoreFloat(&panValue, DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&toPlayerVec), playerRightVec));
					panValue *= 0.5f; //���̂܂܎g���ƕЕ��ɋ�����肷����ׁA������߂�
					// resultCross����-1.0�`�E1.0�͈̔͂ɒ������Ă���p����ύX
					item[i]->GetSE()->pan(panValue);
					//--------------------------------------------------			
					//����-------------------------------------------
					//SetSEVolume(item[i].get(), playerToItemLength, i);
					//�p�������E�ɐU�������ɏ������ʂ�����������
					float panAmount = fabsf(panValue);
					float panVolumeCompensation =
						1.0f - panAmount * 0.2f;

					//��������
					//float seVolume = std::clamp(1.0f - playerToItemLengthXZ / HEAR_SE_RANGE_MAX, 0.0f, 1.0f); //�f�t�H���g�̃{�����[���Ɋ|����W��
					// 1 - x^2 �������ŋߋ����ł͕ω����������A�������ŋ}�������Ă����̂����P
					float seVolume = powf(0.5f, playerToItemLength / 2.5f); //�f�t�H���g�̃{�����[���Ɋ|����W�� //�Q�����Ƃɔ���(0.5�{)
					seVolume = sqrtf(seVolume);
					float se_defaultVolume = Item::ItemMasterData[item[i]->GetItemType()].Volume; // MasterData�Ɉˑ�������
					item[i]->GetSE()->volume(se_defaultVolume* seVolume* coefficientVolumeGUI * panVolumeCompensation);
					//----------------------------------------------					

				}
			}
			// �������̃A�C�e��
			for (int i = 0; i < Item::ITEM_TYPE_NUM; i++)
			{
				//float toPlayerVecX{}, toPlayerVecZ{}; //���������A�p���̌v�Z�p�ɋL��
				DirectX::XMFLOAT3 toPlayerVec;
				// sqrtf()���g���������͏d�����߁A��x�v�Z�������ʂ��L�����Ă���
				float playerToItemLength = culcPlayerToItemLength(p->GetItemEffectPos(), toPlayerVec);

				//���������ȓ��Ȃ��
				if (playerToItemLength < HEAR_SE_RANGE_MAX)
				{
					//�p��------------------------------------------
					float panValue;
					DirectX::XMStoreFloat(&panValue, DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&toPlayerVec), playerRightVec));
					panValue *= 0.5f; //���̂܂܎g���ƕЕ��ɋ�����肷����ׁA������߂�


					//// 2D�̊O�ςō��E����
					//float resultCross = playerDirX * toPlayerVecZ - playerDirZ * toPlayerVecX;

					// resultCross����-1.0�`�E1.0�͈̔͂ɒ������Ă���p����ύX
					p->GetSE(i)->pan(panValue);
					//--------------------------------------------------					

					//����-------------------------------------------
					//�p�������E�ɐU�������ɏ������ʂ�����������
					float panAmount = fabsf(panValue);
					float panVolumeCompensation =
						1.0f - panAmount * 0.2f;

					//��������
					//float seVolume = std::clamp(1.0f - playerToItemLengthXZ / HEAR_SE_RANGE_MAX, 0.0f, 1.0f); //�f�t�H���g�̃{�����[���Ɋ|����W��
					// 1 - x^2 �������ŋߋ����ł͕ω����������A�������ŋ}�������Ă����̂����P
					float seVolume = powf(0.5f, playerToItemLength / 2.0f); //�f�t�H���g�̃{�����[���Ɋ|����W�� //�Q�����Ƃɔ���(0.5�{)
					seVolume = sqrtf(seVolume);
					float se_defaultVolume = Item::ItemMasterData[i].Volume; // MasterData�Ɉˑ�������
					p->GetSE(i)->volume(se_defaultVolume* seVolume* coefficientVolumeGUI * panVolumeCompensation);
					//----------------------------------------------
				}
			}
		}
	}


	//�S�[������

	if (!goalReached)
	{
		if (Collision::sphereVssphere(
			p->GetPlayerPos(),
			1.0f,
			goalPos,
			5.0f))
		{
			goalReached = true;
			goalTimer = 0.0f;

			

			float yaw = p->GetYaw();

			DirectX::XMFLOAT3 pos =
			{
               camera.GetEye().x + camera.GetFront().x * 2.0f,
	           camera.GetEye().y ,
	           camera.GetEye().z + camera.GetFront().z * 1.5f

			};

			goalEffectHandle =  goalEffect->Play(pos,2.0f);


			goalEffect->SetPosition(
				goalEffectHandle,
				pos);

			
		}
	}
	else
	{
		goalTimer += elapsedTime;

		if (goalTimer >= 3.0f)
		{
			SceneManager::Instance().ChangeScene(
				new SceneResult);
		}
	}


	//���N���b�N�̏����L�^
	prev = now;
}

void SceneGame::Render(float elapsedTime)
{
	ID3D11DeviceContext* dc = Graphics::Instance().GetDeviceContext();
	RenderState* renderState = Graphics::Instance().GetRenderState();
	ModelRenderer* modelRenderer = Graphics::Instance().GetModelRenderer();

	ShapeRenderer* shapeRenderer = Graphics::Instance().GetShapeRenderer();


	// ���f���`��
	RenderContext rc;
	rc.deviceContext = dc;
	rc.renderState = renderState;
	rc.camera = &camera;
	
	modelRenderer->Render(rc, stage.transform, stage.model.get(), ShaderId::Lambert);

	
	//shapeRenderer->DrawSphere(goalPos, 3.5f, { 1,0,0,1 });
	//shapeRenderer->Render(dc,camera.GetView(),camera.GetProjection());

	//���C�g�`��
	for (int i = 0; i < LIGHT_MAX; ++i)
	{
		modelRenderer->Render(rc, Light[i].transform, Light[i].model.get(), ShaderId::Lambert);
	}


	for (auto& ss : soundSources)
	{
		if (!ss) continue;
		ss->Render(rc);
	}
	ItemManager::instance().Render(rc);
	GimmickManager::instance().Render(rc);

	EffectManager::Instance().Render(rc.camera->GetView(), rc.camera->GetProjection());

	p->Render(elapsedTime, rc,pause);
	if(pause)
	{
		float S_W = Graphics::Instance().GetScreenWidth();
		float S_H = Graphics::Instance().GetScreenHeight();
		pauseSprite->Render(dc, 0, 0,0, S_W, S_H, 0, 1, 1, 1, 0.5f);
		
		ReTrySprite->Render(dc, S_W / 4, S_H / 70, 0, 0.5f * S_W, 0.4f * S_H, 0, 1, 1, 0.3f, ReTry_alpha);

		GameSprite->Render(dc, S_W / 4, S_H / 3.6f , 0, 0.5f * S_W, 0.4f * S_H, 0, 1, 1, 0.3f, game_alpha);

		TitleSprite->Render(dc, S_W / 4, S_H / 1.8f , 0, 0.5f * S_W, 0.4f * S_H, 1, 1, 1, 0.3f, title_alpha);
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
		if (ImGui::CollapsingHeader("audio", ImGuiTreeNodeFlags_DefaultOpen))
		{
			// ���݂̒l��\��
			ImGui::InputInt("itemNum", &itemNum);
			//// ���Z�{�^��
			//ImGui::SameLine();
			//if (ImGui::Button("-1")) {
			//	itemNum--;
			//}			
			//// ���Z�{�^��
			//ImGui::SameLine();
			//if (ImGui::Button("+")) {
			//	itemNum++;
			//}
			itemNum = std::clamp(itemNum, 0, ITEM - 1);

			ImGui::InputFloat("pPosVecX", &positionVecX);
			ImGui::InputFloat("pPosVecZ", &positionVecZ);
			ImGui::InputFloat("pDirX", &playerDirectionX);
			ImGui::InputFloat("pDirZ", &playerDirectionZ);
			ImGui::InputFloat("cross", &resultCrossGUI);
			ImGui::InputFloat("dot", &resultDotGUI);
		}
		if (ImGui::CollapsingHeader("se", ImGuiTreeNodeFlags_DefaultOpen))
		{
			//SE�S�̂̃{�����[��
			ImGui::DragFloat("volume", &coefficientVolumeGUI, 0.01f, 0.0f, 5.0f);
			//SE���Ƃ̃f�t�H���g�̃{�����[�� //����̐ݒ肾�ƒ������Ă����f����Ȃ�
			ImGui::DragFloat("bombVolume",  &bombVolume, 0.01f, 0.0f, 10.0f);
			ImGui::DragFloat("slashVolume", &slashVolume, 0.01f, 0.0f, 10.0f);
			ImGui::DragFloat("smashVolume", &smashVolume, 0.01f, 0.0f, 10.0f);

			//ImGui::DragFloat("resultVolume", &coefficientVolumeGUI, 0.01f, 0.0f, 10.0f);
		}
		if (ImGui::CollapsingHeader("soundSource", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::DragFloat("gramoSize", &gramoSizeGUI, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("TVSize", &TVSizeGUI, 0.01f, 0.0f, 1.0f);
		}
	}
	ImGui::End();
	if (ImGui::Begin("SoundSource"))
	{
		
		if (ImGui::CollapsingHeader("sound0", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::DragFloat3("pos0", &soundSourcesPos[0].x, 0.01f, -30.0f, 30.0f);
		}
		if (ImGui::CollapsingHeader("sound1", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::DragFloat3("pos1", &soundSourcesPos[1].x, 0.01f, -30.0f, 30.0f);
		}
		if (ImGui::CollapsingHeader("sound2", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::DragFloat3("pos2", &soundSourcesPos[2].x, 0.01f, -30.0f, 30.0f);
		}
		if (ImGui::CollapsingHeader("sound3", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::DragFloat3("pos3", &soundSourcesPos[3].x, 0.01f, -30.0f, 30.0f);
		}
		if (ImGui::CollapsingHeader("sound4", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::DragFloat3("pos4", &soundSourcesPos[4].x, 0.01f, -30.0f, 30.0f);
		}
		if (ImGui::CollapsingHeader("sound5", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::DragFloat3("pos5", &soundSourcesPos[5].x, 0.01f, -30.0f, 30.0f);
		}
		if (ImGui::CollapsingHeader("sound6", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::DragFloat3("pos6", &soundSourcesPos[6].x, 0.01f, -30.0f, 30.0f);
		}
	}
	ImGui::End();
}

//// SE�̃p���̔䗦�̌v�Z
//void SceneGame::SetSEPan(Item* item, DirectX::XMFLOAT3 posVec, float playerToItemLength, int itemNumber)
//{
//	�ύX���ĂȂ��I�I�I;
//
//	//�v���C���[�̌���
//	float playerDirX = sinf(p->GetYaw());
//	float playerDirZ = cosf(p->GetYaw());
//	
//	// 2D�̊O�ςō��E����
//	float resultCross = playerDirX * toPlayerVecZ - playerDirZ * toPlayerVecX;
//	// ���ςŁA�v���C���[�̌����ƃv���C���[���特���܂ł̊p�x���Z�o
//	//float resultDot = playerDirX * posVecX + playerDirZ * posVecZ;
//	
//	//GUI�p�ϐ��Ɋi�[
//	if (itemNumber == itemNum)
//	{
//		positionVecX = toPlayerVecX;
//		positionVecZ = toPlayerVecZ;
//		playerDirectionX = playerDirX;
//		playerDirectionZ = playerDirZ;
//		resultCrossGUI = resultCross;
//	}
//	
//	//resultDotGUI = resultDot;
//	// resultCross����-1.0�`�E1.0�͈̔͂ɒ������Ă���p����ύX
//	item->GetSE()->pan(-resultCross);
//}

////���ʂ̋�������
//void SceneGame::SetSEVolume(Item* item, float playerToItemLengthXZ, int itemNumber)
//{
//	//�㉺�ړ��ł͋����������N����Ȃ��d�l
//	//float seVolume = std::clamp(1.0f - (playerToItemLengthXZ / HEAR_SE_RANGE_MAX), 0.0f, 1.0f); //�f�t�H���g�̃{�����[���Ɋ|����W��
//	//seVolume *= seVolume; // (1 - x)^2
//	float seVolume = powf(0.5f, playerToItemLengthXZ / 2.0f); //�f�t�H���g�̃{�����[���Ɋ|����W�� //�Q�����Ƃɔ���(0.5�{)
//	seVolume = sqrtf(seVolume); // 
//	float se_defaultVolume = Item::ItemMasterData[/*static_cast<ItemType>*/(item->GetItemType())].Volume;
//	item->GetSE()->volume(se_defaultVolume * seVolume * coefficientVolumeGUI);
//
//	////SE�f�t�H���g�{�����[�������p
//	//switch (item->GetItemType())
//	//{
//	//case 0:
//	//	item->GetSE()->volume(bombVolume * seVolume);
//	//	//GUI�p�ϐ��Ɋi�[
//	//	if (itemNumber == itemNum)
//	//	{
//	//		resultVolumeGUI = smashVolume * seVolume;
//	//	}
//	//	break;
//	//case 1:
//	//	item->GetSE()->volume(slashVolume * seVolume);
//	//	//GUI�p�ϐ��Ɋi�[
//	//	if (itemNumber == itemNum)
//	//	{
//	//		resultVolumeGUI = bombVolume * seVolume;
//	//	}
//	//	break;
//	//case 2:
//	//	item->GetSE()->volume(smashVolume * seVolume);
//	//	//GUI�p�ϐ��Ɋi�[
//	//	if (itemNumber == itemNum)
//	//	{
//	//		resultVolumeGUI = smashVolume * seVolume;
//	//	}
//	//	break;
//	//}
//	
//
//
//	
//}

float SceneGame::culcPlayerToItemLength(Item* item, DirectX::XMFLOAT3& posVec)
{
	//�v���C���[���特���ւ̃x�N�g��������o��
	posVec = { item->GetPos().x - p->GetPlayerPos().x, item->GetPos().y - p->GetPlayerPos().y, item->GetPos().z - p->GetPlayerPos().z };
	
	//���K��
	float playerToItemLength = sqrtf(posVec.x * posVec.x + posVec.y * posVec.y + posVec.z * posVec.z);
	return playerToItemLength;
}

//�A�C�e���𓊂������p
float SceneGame::culcPlayerToItemLength(DirectX::XMFLOAT3 itemPos, DirectX::XMFLOAT3& posVec)
{
	//�v���C���[���特���ւ̃x�N�g��������o��
	//posVecX = itemPos.x - p->GetPlayerPos().x;
	//posVecZ = itemPos.z - p->GetPlayerPos().z;
	posVec = { itemPos.x - p->GetPlayerPos().x, itemPos.y - p->GetPlayerPos().y, itemPos.z - p->GetPlayerPos().z }; //�Q�ƕԂ�
	//���K��
	float playerToItemLength = sqrtf(posVec.x * posVec.x + posVec.y * posVec.y + posVec.z * posVec.z);
	return playerToItemLength;
}

