#include <imgui.h>
#include <ImGuizmo.h>
#include "Player.h"
#include <Collision.h>
#include <filesystem>
#include <Gimmick.h>

constexpr float POS = 0.0f;
constexpr float  Head = 1.0f;

Player::Player()
{
	FallItem = false;
	FallGimmick = false;
	for (int i = 0;i < ITEM;i += 1)
	{
		item_alive[i] = true;
	}
	for (int i = 0;i <GIMMICK;i += 1)
	{
		gimmick_alive[i] = true;
	}
	player.onGround = false;
	player.position = { 13, POS, 44 };
	player.scale = { 0.01f, 0.01f, 0.01f };
	player.model = nullptr;

	Arm.onGround = true;
	Arm.position = { 0, 0, 0 };
	Arm.scale = { 0.01f, 0.02f, 0.01f };
	Arm.model = std::make_unique<Model>("Data/Model/Player/arm.mdl");

	Hand.onGround = true;
	Hand.scale = { 0.01f, 0.01f, 0.01f };
	Hand.model = std::make_unique<Model>("Data/Model/Player/hand.mdl");

	for (int i = 0;i < Item::ITEM_TYPE_NUM; i += 1)
	{
		Arm_Item[i].position = { 0, 0, 0 };
		Arm_Item[i].scale = { 0.01f, 0.01f, 0.01f };
		Arm_Item[i].angle = { 0,0,0 };
		Arm_Item[i].model = std::make_unique<Model>("Data/Model/Player/Ball.mdl");
	
	}

	UI[Interact::InteractionType::Bomb] = std::make_unique<Sprite>(Graphics::Instance().GetDevice(), "Data/Sprite/BOOM_UI.png");
	UI[Interact::InteractionType::Slash] = std::make_unique<Sprite>(Graphics::Instance().GetDevice(), "Data/Sprite/SLASH_UI.png");
	UI[Interact::InteractionType::Smash] = std::make_unique<Sprite>(Graphics::Instance().GetDevice(), "Data/Sprite/SMASH_UI.png");
	UI[Interact::InteractionType::Flash] = std::make_unique<Sprite>(Graphics::Instance().GetDevice(), "Data/Sprite/FLASH_UI.png");




	Target = std::make_unique<Sprite>(Graphics::Instance().GetDevice(), "Data/Sprite/Target.png");
	
	move_position = player.position;
	get_item = false;
	Switch_camera = true;



}

void Player::Initialize()
{
	bomb =EffectManager::Instance().CreateEffect("Data/Effect/Bomb.efk");
	sword =EffectManager::Instance().CreateEffect("./Data/Effect/sword.efkefc");
	smash =EffectManager::Instance().CreateEffect("./Data/Effect/smash.efkefc");
	flash =EffectManager::Instance().CreateEffect("./Data/Effect/Flash.efkefc");

	touch = EffectManager::Instance().CreateEffect("./Data/Effect/toutch.efkefc");
	move = EffectManager::Instance().CreateEffect("./Data/Effect/move.efkefc");
}

// 更新処理
void Player::Update(float elapsedTime, Object& S, const Camera& C)
{
	item_r = 1;
	item_g = 1;
	item_b = 1;
	HWND hwnd = GetActiveWindow();
	RECT rc;
	GetClientRect(hwnd, &rc);
	POINT center;
	center.x = (rc.right - rc.left) / 2;
	center.y = (rc.bottom - rc.top) / 2;

	//スクリーン座標の設定
	DirectX::XMVECTOR ScreenPosition, WorldPosition;
	DirectX::XMFLOAT3 screenPosition;
	screenPosition.x = static_cast<float>(center.x);
	screenPosition.y = static_cast<float>(center.y);

	float screenWidth = Graphics::Instance().GetScreenWidth();
	float screenHeight = Graphics::Instance().GetScreenHeight();
	//各行列を取得
	DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&C.GetView());
	DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&C.GetProjection());
	DirectX::XMMATRIX World = DirectX::XMMatrixIdentity();

	screenPosition.z = 0.0f;
	ScreenPosition = DirectX::XMLoadFloat3(&screenPosition);

	WorldPosition = DirectX::XMVector3Unproject(
		ScreenPosition,
		0, 0, screenWidth, screenHeight,
		0.0f, 1.0f,
		Projection, View, World);
	DirectX::XMFLOAT3 rayStart;
	DirectX::XMStoreFloat3(&rayStart, WorldPosition);

	screenPosition.z = 1.0f;
	ScreenPosition = DirectX::XMLoadFloat3(&screenPosition);

	DirectX::XMStoreFloat3(&screenPosition, ScreenPosition);
	WorldPosition = DirectX::XMVector3Unproject(
		ScreenPosition,
		0, 0, screenWidth, screenHeight,
		0.0f, 1.0f,
		Projection, View, World);
	DirectX::XMFLOAT3 rayEnd;
	DirectX::XMStoreFloat3(&rayEnd, WorldPosition);

	
	arm_player.x = move_position.x - player.position.x;
	arm_player.y = move_position.y - player.position.y;
	arm_player.z = move_position.z - player.position.z;
	DirectX::XMVECTOR arm_player_vec = DirectX::XMLoadFloat3(&arm_player);
	arm_player_vec = DirectX::XMVector3Normalize(arm_player_vec);
	DirectX::XMStoreFloat3(&arm_player, arm_player_vec);

	DirectX::XMFLOAT3 right;
	right.x = cosf(GetYaw());
	right.y = 0.0f;
	right.z = -sinf(GetYaw());

	// カメラの方向
	const DirectX::XMFLOAT3& cameraFront = C.GetFront();
	float cameraFrontLengthXZ = sqrtf(cameraFront.x * cameraFront.x + cameraFront.z * cameraFront.z);
	float cameraFrontX = cameraFront.x / cameraFrontLengthXZ;
	float cameraFrontZ = cameraFront.z / cameraFrontLengthXZ;

	// 進行方向を向くようにする
	front = cameraFront;
	float lenXZ = sqrtf(front.x * front.x + front.z * front.z);
	if (lenXZ > 0.0001f) {
		front.x /= lenXZ;
		front.z /= lenXZ;
	}
	player.angle.y = atan2f(front.x, front.z);
	Arm_Item[having_item_type].angle.y = atan2f(front.x, front.z);
	Hand.angle.y = atan2f(front.x, front.z);
	Arm.angle.y = atan2f(front.x, front.z);

	DirectX::XMFLOAT3 effectPos =
	{
		player.position.x + front.x * 1.5f,
		player.position.y + 1.75f,
		player.position.z + front.z * 1.5f
	};

	// 接地処理
	if (!(player.onGround))
	{
		// レイの始点と終点を求める
		DirectX::XMFLOAT3 s = { player.position.x, player.position.y + 0.5f, player.position.z };
		DirectX::XMFLOAT3 e = { player.position.x, player.position.y, player.position.z };

		// レイキャストを行い、交点を求める
		DirectX::XMFLOAT3 p, n;
		if (Collision::RayCast(s, e, S.transform, S.model.get(), p, n))
		{
			if (n.y > 0.5f)
			{
				player.position.y = 0;
				long_Hand = 0;
				Switch_camera = true;
				player.onGround = true;
			}
		}
		//架け橋との接地判定
		// G[3]は架け橋
		else if (Collision::RayCast(s, e, G[3]->transform, G[3]->model.get(), p, n))
		{
			if (n.y > 0.5f)
			{
				player.position.y = 0;
				long_Hand = 0;
				Switch_camera = true;
				player.onGround = true;
			}
		}
		
		else if (Collision::RayCast(s, e, G[6]->transform, G[6]->model.get(), p, n)&& !(gimmick_alive[6]))
		{
			if (n.y > 0.5f)
			{
				player.position.y = 0;
				long_Hand = 0;
				Switch_camera = true;
				player.onGround = true;
			}
		}
		else player.position.y -= 4.0f * elapsedTime;
	}
	
	//レイキャスト
	DirectX::XMFLOAT3 wallHitPos, wallHitNormal;
	bool hitWall = Collision::RayCast(rayStart, rayEnd, S.transform, S.model.get(), wallHitPos, wallHitNormal);
	float wallDist = FLT_MAX;
	if (hitWall) {
		DirectX::XMVECTOR s = XMLoadFloat3(&rayStart);
		DirectX::XMVECTOR w = XMLoadFloat3(&wallHitPos);
		wallDist = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(w, s)));

		if(wallDist <= 14.2f&&!(focus_gimmick)&&!(focus_item))
		{
			item_g = 0;
			item_b = 0;
		}
	}

	//アイテムをゲットしていたら
	if (!(GetAsyncKeyState(VK_LBUTTON) & 0x8000))
	{
		if (get_item && !(having_item))
		{
			if (long_Hand == 0)
			{
				having_item = true;
			}
		}
	}
	if (Switch_camera)
	{
		focus_item = false;
		focus_gimmick = false;
	}

	
	FocusItem(rayStart, rayEnd, wallDist);
	FocusGimick(rayStart, rayEnd, wallDist);
	
	//壁移動
	if (!(focus_item) && !(focus_gimmick))
	{
		WallMove(rayStart, rayEnd, S);
		DirectX::XMFLOAT3 r = { front.z, 0.0f, -front.x }; // 右方向（Y軸回転）
		DirectX::XMFLOAT3 l = { -r.x, 0.0f, -r.z }; // 左方向
		bool hitFront = false;
		bool hitRight = false;
		bool hitLeft = false;

		DirectX::XMFLOAT3 s = {
					player.position.x ,
					0.5f,
					player.position.z
		};

		auto RayCheck = [&](DirectX::XMFLOAT3 dir, bool& hitFlag)
			{
				DirectX::XMFLOAT3 e = {
					player.position.x + dir.x,
					0.5f,
					player.position.z + dir.z
				};

				DirectX::XMFLOAT3 p, n;
				if (Collision::RayCast(s, e, S.transform, S.model.get(), p, n))
				{
					hitFlag = true;
					return true;
				}
				return false;
			};
		RayCheck(front, hitFront);
		RayCheck(r, hitRight);
		RayCheck(l, hitLeft);

		if (hitFront)
		{
			move_position = player.position;
			player.position.x -= front.x * 0.5f;
			player.position.z -= front.z * 0.5f;
		}
		if (hitRight)
		{
			player.position.x -= r.x * 0.05f;
			player.position.z -= r.z * 0.05f;
		}
		if (hitLeft)
		{
			player.position.x -= l.x * 0.05f;
			player.position.z -= l.z * 0.05f;
		}
	}
	
	//落ちたら
	if (player.position.y < -5)
	{
		//リスポーン
		player.position = { 11,2,41 };
		player.onGround = false;
		//持っているアイテムをリセット
		having_item = false;
		get_item = false;
	}
	
	//腕の移動位置
	if (long_Hand != 0)ArmMove(front, right);
	
	//腕の初期位置
	else ArmStart(front, right);


	if (Speed >= 0.1f)
	{
		player.position.x += arm_player.x * Speed;
		player.position.y += arm_player.y * Speed;
		player.position.z += arm_player.z * Speed;
		Arm_Item[having_item_type].position.x += arm_player.x * Speed;
		Arm_Item[having_item_type].position.y += arm_player.y * Speed;
		Arm_Item[having_item_type].position.z += arm_player.z * Speed;

		if (handle_move < 0)
		{
			handle_move = move->Play(effectPos, 1.0f);
		}

		move->SetPosition(handle_move, effectPos);
	}
	else
	{
		if (handle_move >= 0)
		{
			move->Stop(handle_move);
			handle_move = -1;
		}
	}
	
	player.UpdateTransform();
	Arm.UpdateTransform();
	Arm_Item[having_item_type].UpdateTransform();
	Hand.UpdateTransform();
}

// 描画処理
void Player::Render(float elapsedTime, RenderContext rc,bool Pause)
{
	ID3D11DeviceContext* dc = Graphics::Instance().GetDeviceContext();
	ModelRenderer* modelRenderer = Graphics::Instance().GetModelRenderer();
	ShapeRenderer* shaperenderer = Graphics::Instance().GetShapeRenderer();
	
	float dw = Graphics::Instance().GetScreenWidth();
	float dh = Graphics::Instance().GetScreenHeight();

	modelRenderer->Render(rc, Hand.transform, Hand.model.get(), ShaderId::Lambert);
	modelRenderer->Render(rc, Arm.transform, Arm.model.get(), ShaderId::Lambert);

	if (having_item)
	{
		modelRenderer->Render(rc, Arm_Item[having_item_type].transform, Arm_Item[having_item_type].model.get(), ShaderId::Lambert);
	}
	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000 && Switch_camera && !Pause)
	{
		Target->P_Render(dc, dw / 2, dh / 2, 0, dw, dh, 0, item_r, item_g, item_b, 1);
		if (having_item)
		{
			UI[having_item_type]->Render(dc, dw/10 - 50, dh/10 - 30, 0, 256, 128,0, 1, 1, 1,1);
		}
	}

#ifndef DEBUG
	if (ImGui::Begin("player"))
	{
		//--所持中のアイテム--
		if (ImGui::CollapsingHeader("havingItem", ImGuiTreeNodeFlags_DefaultOpen))
		{
			const char* equippingLabels[] = { "Bomb", "Slash", "Smash", "Flash", "None"};
			int currentEquipment = static_cast<int>(having_item_type);

			if (ImGui::Combo("havingItemType", &currentEquipment, equippingLabels, IM_ARRAYSIZE(equippingLabels)))
			{
				having_item_type = static_cast<Interact::InteractionType>(currentEquipment);
				if (having_item_type == Interact::InteractionType::None)
				{
					having_item = false;
					get_item = false;
				}
				else
				{
					//having_item = true;
					get_item = true;
				}
			}
		}
		//------------------

		if (ImGui::CollapsingHeader("POS", ImGuiTreeNodeFlags_DefaultOpen))
		{

			ImGui::DragFloat3("playerPos", &player.position.x, 0.1f);
			ImGui::InputFloat3("HandPos", &Hand.position.x);
			ImGui::InputFloat("Hand", &long_Hand);
			ImGui::InputFloat("Arm", &Arm.scale.z);


			if (ImGui::Button("初期位置"))
			{
				player.position = { 15,1,41 };
				player.onGround = false;
			}
			if (ImGui::Button("ギミック"))
			{
				player.position = { 10,0,20 };
				//player.onGround = false;
			}
			if (ImGui::Button("ギミックあと"))
			{
				player.position = { 10,0,-20 };
				//player.onGround = false;
			}
			if (ImGui::Button("ひろま"))
			{
				player.position = { 0,0,0 };
				//player.onGround = false;
			}
		}
	}
	ImGui::End();
#endif
}

void Player::FocusItem(DirectX::XMFLOAT3 rayStart, DirectX::XMFLOAT3 rayEnd,float wallDist)
{
	bool Catch=true;
	Collision::SphereCastResult SCR;
	for (int i = 0;i < ITEM;i += 1)
	{
		if (item_alive[i] && Collision::IntersectRayVsSphere(rayStart, rayEnd, I[i]->position, 0.5f,&SCR))
		{
			
			DirectX::XMVECTOR s = XMLoadFloat3(&rayStart);
			float itemDist = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(SCR.position, s)));

			// 壁より奥にある item は無視
			if (itemDist > wallDist)continue;
			if (itemDist <= 14.0f)
			{
				item_r = 0;
				item_b = 0;
			}
			if (Switch_camera)Catch = false;
			focus_item = true;
			
				static bool prev = false;
				bool now = (GetAsyncKeyState(VK_LBUTTON) & 0x8000);
				if (!(now) && prev)
				{
					Catch = true;
					long_Hand = 1;
				}
				if (now)
				{
					DirectX::XMStoreFloat3(&move_position,SCR.position);
				}
				prev = now;
				if (Collision::sphereVssphere(Hand.position, 0.5f, move_position, 0.5f)&&Catch)
				{
					long_Hand = -1;
					get_item = true;
					having_item_type = item_type[i];
					item_alive[i] = false;
					break;
				}
			
		}
		if (long_Hand == 0 && !(item_alive[i]))item_alive[i] = true;
	}
}

void Player::FocusGimick(DirectX::XMFLOAT3 rayStart, DirectX::XMFLOAT3 rayEnd, float wallDist)
{
	DirectX::XMFLOAT3 hitPos, hitNormal;
	bool Catch = true;
	for (int i = 0;i < GIMMICK;i += 1)
	{
		if (gimmick_alive[i])
		{
			if (Collision::RayCast(rayStart, rayEnd, G[i]->transform, G[i]->model.get(), hitPos, hitNormal))
			{
				focus_gimmick = true;
				i_timer = 5.0f;
				//ギミック
				DirectX::XMVECTOR s = XMLoadFloat3(&rayStart);
				DirectX::XMVECTOR p = XMLoadFloat3(&hitPos);
				float gimmickDist = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(p, s)));

				if (gimmickDist > wallDist)continue;
				
				if (gimmickDist <= 15.0f)
				{
					item_r = 0;
					item_g = 0;
				}
				if (Switch_camera)Catch = false;
				if (having_item)
				{
					static bool prev = false;
					bool now = (GetAsyncKeyState(VK_LBUTTON) & 0x8000);
					if (!(now) && prev)
					{

						long_Hand = 2;
						Catch = true;
					}
					if (now)
					{
						move_position = hitPos;
					}
					
					prev = now;
					if(Catch)
					{
						if (Collision::RayCast(Arm_Item[having_item_type].position,
							{
							Arm_Item[having_item_type].position.x + front.x * 0.5f,
							Arm_Item[having_item_type].position.y,
							Arm_Item[having_item_type].position.z + front.z * 0.5f
							},
							G[i]->transform, G[i]->model.get(),
							hitPos, hitNormal) ||
							(Collision::sphereVssphere(Arm_Item[having_item_type].position, 0.5f, move_position, 1.0f)))
						{
							long_Hand = 0;
							get_item = false;
							Switch_camera = true;
							switch (having_item_type)
							{
							default:
								/*fallthrough*/
							case Interact::InteractionType::Bomb:
								//位置設定
								itemEffectPos = { hitPos.x, 0, hitPos.z };
								//エフェクト再生
								bomb->Play(itemEffectPos, 3.0f);
								// SE再生
								se[Interact::InteractionType::Bomb]->play();

								break;
							case Interact::InteractionType::Slash:
								//sword->Play(G[i]->position, 0.5f);
								itemEffectPos = hitPos;
								EffectManager::Instance().PlayEffect(sword, itemEffectPos, 1.2f);
								se[Interact::InteractionType::Slash]->play();
								break;
							case Interact::InteractionType::Smash:
								itemEffectPos = G[i]->position;
								//コンパイラ エラー C2360対策として、スコープで区切る
								{
									Effekseer::Handle handle = EffectManager::Instance().PlayEffect(smash, {itemEffectPos.x+2.5f,itemEffectPos.y+1.6f,itemEffectPos.z-5.0f}, 0.5f);
									smash->SetRotation(handle, 0, DirectX::XM_PI, 0); //y軸方向に半回転
								}
								se[Interact::InteractionType::Smash]->play();
								break;
							case Interact::InteractionType::Flash:
								itemEffectPos = hitPos;
								EffectManager::Instance().PlayEffect(flash, itemEffectPos, 10.0f);
								se[Interact::InteractionType::Flash]->play();
								break;
							}
							having_item = false;

							if (having_item_type == gimmick_type[i])
							{
								gimmick_alive[i] = false;
								if (i == 5)
								{
									FallGimmick = true;
									gimmick_alive[6] = false;
								}
								if (i == 4)
								{
									FallItem = true;
								}
							}
							move_position = player.position;
							i_timer = 0;
							break;
						}
					}
				}
			}
			if (i < 3 && !(having_item))
			{
				WallMove(rayStart, rayEnd, *G[i]);
				DirectX::XMFLOAT3 r = { front.z, 0.0f, -front.x }; // 右方向（Y軸回転）
				DirectX::XMFLOAT3 l = { -r.x, 0.0f, -r.z }; // 左方向
				bool hitFront = false;
				bool hitRight = false;
				bool hitLeft = false;

				DirectX::XMFLOAT3 s = {
							player.position.x ,
							0.5f,
							player.position.z
				};

				auto RayCheck = [&](DirectX::XMFLOAT3 dir, bool& hitFlag)
					{
						DirectX::XMFLOAT3 e = {
							player.position.x + dir.x,
							0.5f,
							player.position.z + dir.z
						};

						DirectX::XMFLOAT3 p, n;
						if (Collision::RayCast(s, e, G[i]->transform, G[i]->model.get(), p, n))
						{
							hitFlag = true;
							return true;
						}
						return false;
					};
				RayCheck(front, hitFront);
				RayCheck(r, hitRight);
				RayCheck(l, hitLeft);

				if (hitFront)
				{
					move_position = player.position;
					player.position.x -= front.x * 0.5f;
					player.position.z -= front.z * 0.5f;
				}
				if (hitRight)
				{
					player.position.x -= r.x * 0.05f;
					player.position.z -= r.z * 0.05f;
				}
				if (hitLeft)
				{
					player.position.x -= l.x * 0.05f;
					player.position.z -= l.z * 0.05f;
				}
			}
		}

		//if (i == 2 && !(having_item))
		//{
		//	focus_gimmick = true;
		//	WallMove(rayStart, rayEnd, *G[2]);
		//}
	}
}

void Player::WallMove(DirectX::XMFLOAT3 rayStart, DirectX::XMFLOAT3 rayEnd,Object& S)
{
	 //handWallHit = false;
	static float timer=3;
	DirectX::XMFLOAT3 hitPos, hitNormal;
	if (Collision::RayCast(rayStart, rayEnd, S.transform, S.model.get(), hitPos, hitNormal))
	{
		if (hitNormal.y > 0.2f)Speed = 0.0f;
		else
		{
			static bool prev = false;
			bool now = (GetAsyncKeyState(VK_LBUTTON) & 0x8000);
			if (!(now) && prev)
			{
				if (!(Collision::sphereVssphere(Arm.position, 0.3f, move_position, 0.5f)))
				{
					Switch_camera = false;
					long_Hand = 1;
				}
			}
			if (now && Switch_camera) move_position = hitPos;
			prev = now;
		}
	}
	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
	{
		if (move_position.y <= POS + 0.1f)move_position.y = POS + 0.1f;
	}
	if (!(Switch_camera))
	{
		if (Collision::sphereVssphere(player.position, 0.5f, move_position, 0.5f))
		{
			move_position = player.position;
			move_position.y = 0;
			Speed = 0;
			player.onGround = false;
			long_Hand = 0;
		}

		bool hitNow = Collision::sphereVssphere(
			Hand.position, 0.3f,
			move_position, 0.5f);

		if (hitNow)
		{
			timer -= 0.1f;
			if (!handWallHit)
			{
				OutputDebugStringA("Hit\n");
				float yaw = atan2f(hitNormal.x, hitNormal.z);
				// 接触した瞬間だけ

				auto handle = touch->Play(Hand.position,0.5f);

				touch->SetScale(handle, { 0.3f,0.3f,0.3f });
				touch->SetRotation(handle, 0, yaw, 0);

				handWallHit = true;
			}
			
			handWallHit = true;

			Hand.position = move_position;
			
			if (timer < 0)
			{
				timer = 0;
				Speed = 0.1f;
			}
			
			long_Hand = 0.1f;
		}
		else
		{
			handWallHit = false;
		}
		if (timer == 0 && Speed <= 0)
		{
			timer = 3;
		}
	}
}

void Player::ArmMove(DirectX::XMFLOAT3 f, DirectX::XMFLOAT3 r)
{
	
	Switch_camera = false;
	f.y *= 1.3f;
	DirectX::XMMATRIX rot = Arm.LookRotation(f); 
	Arm.SetRotationFromMatrix(rot);

	DirectX::XMFLOAT3 forward(
		rot.r[2].m128_f32[0],
		rot.r[2].m128_f32[1],
		rot.r[2].m128_f32[2]);
	if (long_Hand >= 2)
	{
		Arm_Item[having_item_type].position.x = Arm_Item[having_item_type].position.x + (f.x * 0.5f + r.x * 0.05f + forward.x) * 0.1f;
		Arm_Item[having_item_type].position.y = Arm_Item[having_item_type].position.y + f.y * 0.05f;
		Arm_Item[having_item_type].position.z = Arm_Item[having_item_type].position.z + (f.z * 0.5f + r.z * 0.05f + forward.z) * 0.1f;
		//ギミック
		i_timer -= 0.05f;
		if (i_timer <= 0.0f)
		{
			get_item = false;
			having_item = false;
			long_Hand = 0;
			Switch_camera = true;
			move_position = player.position;
			i_timer = 5.0f;
		}
	}
	else
	{
		if (Arm.scale.z < 0.01f)
		{
			Switch_camera = true;
			Arm.scale.z = 0.01f;
			long_Hand = 0;
			move_position = player.position;
		}
		//アイテム・壁
		else if (Arm.scale.z >= Player::ARM_RANGE_MAX)
		{
			long_Hand = -3;
		}
		if (long_Hand >= 1 || long_Hand <= -1)
		{
			Arm.scale.z += (0.1f * 0.01f) * long_Hand;
		}
		
		float dist = Arm.scale.z * 40;

		Hand.position.x = Arm.position.x + f.x * 0.5f + r.x * 0.05f + f.x * dist;
		if (forward.y >= 0)Hand.position.y = Arm.position.y + 0.3f + f.y * dist;
		else Hand.position.y = Arm.position.y +0.25f+ f.y * dist;
		Hand.position.z = Arm.position.z + f.z * 0.5f + r.z * 0.05f + f.z * dist;
	}

}

void Player::ArmStart(DirectX::XMFLOAT3 f, DirectX::XMFLOAT3 r)
{
	Arm.angle = { 0, player.angle.y, 0 };
	Hand.angle = { 0, player.angle.y, 0 };

	Arm.position.x = player.position.x + 1.0f * f.x + r.x * 0.3f;
	Arm.position.y = player.position.y + 1.2f;
	Arm.position.z = player.position.z + 1.0f * f.z + r.z * 0.3f;
	Arm.scale.z = 0.01f;

	Hand.position.x = Arm.position.x + f.x * 0.4f + r.x * 0.05f;
	Hand.position.y = Arm.position.y + 0.3f;
	Hand.position.z = Arm.position.z + f.z * 0.4f + r.z * 0.05f;


	Arm_Item[having_item_type].position.x = player.position.x + 1.0f * f.x + r.x * -0.5f;
	Arm_Item[having_item_type].position.y = player.position.y + 1.2f;
	Arm_Item[having_item_type].position.z = player.position.z + 1.0f * f.z + r.z * -0.5f;
}