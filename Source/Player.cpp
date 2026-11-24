#include "Player.h"
#include <Windows.h>
#include <imgui.h>

#include <cmath>
#include "ShapeRenderer.h"

Player::Player()
{
	Initialize();

	spriteHPGauge = std::make_unique<Sprite>(Graphics::Instance().GetDevice(), "Data/Sprite/gauge_bar_neon-green_01_ff4444_128.png");
	spriteGauge = std::make_unique<Sprite>(Graphics::Instance().GetDevice(), "Data/Sprite/gauge_bar-square_black_01_ff4444_128.png");
	spriteHP = std::make_unique<Sprite>(Graphics::Instance().GetDevice(), "Data/Sprite/gauge_bar_blue-gold_01_44cc44_128.png");
	spriteWP = std::make_unique<Sprite>(Graphics::Instance().GetDevice(), "Data/Sprite/gauge_bar-square_blue-gold_01_ff4444_128.png");

	spriteW[0] = std::make_unique<Sprite>(Graphics::Instance().GetDevice(), "Data/Sprite/sword_transparent.png");
	spriteW[1] = std::make_unique<Sprite>(Graphics::Instance().GetDevice(), "Data/Sprite/scythe_transparent.png");
	spriteW[2] = std::make_unique<Sprite>(Graphics::Instance().GetDevice(), "Data/Sprite/glove_transparent.png");
	spriteHITCOUNT = std::make_unique<Sprite>(Graphics::Instance().GetDevice(), "Data/Sprite/hitcount.png");
	spriteHIT = std::make_unique<Sprite>(Graphics::Instance().GetDevice(), "Data/Sprite/hit_transparent .png");
	
	Sword_Effect = EffectManager::Instance().CreateEffect("Data/Effect/player_Sword.efkefc");
	Fight_Effect = EffectManager::Instance().CreateEffect("Data/Effect/Smash.efkefc");
	Slash_Effect = EffectManager::Instance().CreateEffect("Data/Effect/Slash.efkefc");

	IsSwrod = true;
	IsFight = true;
	IsSlash = true; 
}
Player::~Player() = default;

void Player::Initialize()
{

	animation.reset();
    player.position = { 0, -28, 0 };

    player.angle = { 0, 0, 0 };
    player.scale = { 0.25, 0.25, 0.25 };
    player.velocity = { 0, 0, 0 };
    player.model = std::make_unique<Model>("Data/Model/Player/Player.mdl");

	animation = std::make_unique<Animation>(*player.model);

	Weapon[0].model = std::make_unique<Model>("Data/Model/Weapon/Sword.mdl");
	Weapon[0].scale = { 0.5f,0.5f,0.5f };
	Weapon[1].model = std::make_unique<Model>("Data/Model/Weapon/Scythe.mdl");
	Weapon[1].scale = { 0.25f,0.25f,0.25f };

	weapon_num = 0;

	state = State::Idle;
	animation->PlayAnimation("Idle", true);
	playerHP = PLAYER_HP_MAX;


	//残像用

}

void Player::Update(float elapsedTime, Object& stage, const Camera& camera)
{
	//死んでいたら実行しない
	if (IsDead){animation->Update(elapsedTime);return;}

	// debug debug中はほかの動きができないようにreturn
	if (previewState){animation->Update(elapsedTime);return;}

	//無敵時間
	if (invincibleTime > 0.0f)
	{
		invincibleTime -= elapsedTime;
		if (invincibleTime < 0.0f)
		{
			invincibleTime = 0.0f;
		}
	}
	nisehptime = max(nisehptime - elapsedTime, 0.0f);
	nisehit = min(nisehit + 1, hit);
	hittime -= elapsedTime;
	if (hittime < 0)
	{
		hittime = 0;
		hit = 0;
		nisehit = 0;
	}
	bool isMoving = false;
	
	StartDodge();

	if (isDodging)
	{
		UpdateDodge(elapsedTime);
	}
	else if (!isAttacking)
	{
		isMoving = InputMove(elapsedTime, camera);
	}

	//武器切り替え
	WeaponChange();

	//Update
	UpdateMusouMode(elapsedTime);
	StartAttack();

	if (isAttacking)UpdateAttack(elapsedTime);
	else pAnimation(elapsedTime, isMoving);


	if (weapon_num < 2)
	{
		const char* rightHandName = "mixamorig:RightHand";

		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(Weapon[weapon_num].scale.x, Weapon[weapon_num].scale.y, Weapon[weapon_num].scale.z);
		DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(Weapon[weapon_num].angle.x, Weapon[weapon_num].angle.y, Weapon[weapon_num].angle.z);
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(Weapon[weapon_num].position.x, Weapon[weapon_num].position.y, Weapon[weapon_num].position.z);
		DirectX::XMStoreFloat4x4(&Weapon[weapon_num].transform, S * R * T);

		for (const Model::Node& node : player.model->GetNodes())
		{
			if (strcmp(node.name, rightHandName) == 0)
			{
				DirectX::XMMATRIX w_trans = DirectX::XMLoadFloat4x4(&Weapon[weapon_num].transform);
				DirectX::XMMATRIX g_trans = DirectX::XMLoadFloat4x4(&node.globalTransform);
				DirectX::XMMATRIX p_trans = DirectX::XMLoadFloat4x4(&player.transform);
				w_trans = (w_trans * g_trans) * p_trans;
				DirectX::XMStoreFloat4x4(&Weapon[weapon_num].transform, w_trans);
				break;
			}
		}

		//エフェクト用
		sword_Line.Update(Weapon[weapon_num].transform);
	}

	


}

void Player::Render(float elapsedTime, RenderContext rc)
{
	if (player.model) Graphics::Instance().GetModelRenderer()->Render(
		rc, player.transform, player.model.get(), ShaderId::Lambert);

	//playerの当たり判定、攻撃範囲描画
#ifdef _DEBUG
	if (showAttackCollision && IsAttackActive())
	{
		Graphics::Instance().GetShapeRenderer()->DrawSphere(
			attackPosition,
			attackRadius,
			{ 1, 0, 0, 1 });
	}

	DirectX::XMFLOAT3 pPos = player.position;

	pPos.y += playerRadiusHeight;

	Graphics::Instance().GetShapeRenderer()->DrawSphere(
		pPos,
		playerRadius,
		{ 1, 1, 0, 1 });
#endif

	if (weapon_num < 2)
	{
		Graphics::Instance().GetModelRenderer()->Render(rc, Weapon[weapon_num].transform, Weapon[weapon_num].model.get(), ShaderId::Lambert);
	}
	DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&rc.camera->GetView());
	DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&rc.camera->GetProjection());
	DirectX::XMFLOAT4X4 VP;
	DirectX::XMStoreFloat4x4(&VP, V * P);
	if (isAttacking)
	{
		sword_Line.Render(Graphics::Instance().GetDeviceContext(), VP);

	}

	if (animation->GetCurrentFrame() >= 0 && !IsSwrod && Blad)
	{
		EffectManager::Instance().PlayEffect(Sword_Effect, { player.position.x,player.position.y + 5.0f,player.position.z }, player.angle, 7.0f);
		IsSwrod = true;
		Blad = false;
	}

	if (animation->GetCurrentFrame() >= 0.0f && !IsFight && Blad)
	{
		EffectManager::Instance().PlayEffect(Fight_Effect, { attackPosition.x,attackPosition.y ,attackPosition.z + 10.0f }, player.angle, 7.0f);
		IsFight = true;
		Blad = false;
	}

	if (animation->GetCurrentFrame() >= 0.0f && !IsSlash && Blad)
	{
		Slash_Effect->Play({ player.position.x,player.position.y + 5.0f,player.position.z }, 6.0f);
		IsSlash = true;
		Blad = false;
	}
}
	

void Player::UIRender(float elapsedTime, ID3D11DeviceContext* dc)
{
	float hp = playerHP + ((niseHP - playerHP) * (nisehptime));
	//
	spriteHPGauge->Render(dc, 60, 50, 0, 480, 32, 8, 48, 112, 32, 0, 1, 1, 1, 1);
	spriteHP->Render(dc, 60, 50, 0, 480 * (hp / PLAYER_HP_MAX), 32, 8, 48, 112 * (hp / PLAYER_HP_MAX), 32, 0, 1, 1, 1, 1);
	spriteW[GetBloodGaugeIndex()]->Render(dc, 10, 50, 0, 56, 56, 0, 0, 134, 134, 0, 1, 1, 1, 1);
	spriteGauge->Render(dc, 50, 82, 0, 280 , 28, 0, 48, 128 , 32, 0, 1, 1, 1, 1);
	spriteWP->Render(dc, 50, 82, 0, 280 * (bloodGauge[GetBloodGaugeIndex()] / 100), 28, 0, 48, 128 * (bloodGauge[GetBloodGaugeIndex()] / 100), 32, 0, 1, 1, 1, 1);

	
	
		
	for (int i = 0;i < 3;i++)
	{
		if (GetBloodGaugeIndex() == i)
		{
			spriteGauge->Render(dc, 80, 500 + 32 * i, 0, 280, 28, 0, 48, 128, 32, 0, 1, 1, 1, 1);
			spriteWP->Render(dc, 80, 500 + 32 * i, 0, 280 * (bloodGauge[i] / 100), 28, 0, 48, 128 * (bloodGauge[i] / 100), 32, 0, 1, 1, 1, 1);
			spriteW[i]->Render(dc, 60, 500 + 32 * i, 0, 28, 28, 0, 0, 134, 134, 0, 1, 1, 1, 1);
		}
		else
		{
			spriteGauge->Render(dc, 80, 500 + 32 * i, 0, 280, 28, 0, 48, 128, 32, 0, 1, 1, 1, 0.5);
			spriteWP->Render(dc, 80, 500 + 32 * i, 0, 280 * (bloodGauge[i] / 100), 28, 0, 48, 128 * (bloodGauge[i] / 100), 32, 0, 1, 1, 1, 0.5);
			spriteW[i]->Render(dc, 60, 500 + 32 * i, 0, 28, 28, 0, 0, 134, 134, 0, 1, 1, 1, 1);
		}
		

	}

	if (hit > 0)
	{
		float hitsize = (hit - nisehit) / 10.0f;
		for (int count = nisehit, i=0;count > 0;count /= 10,i++)
		{
			spriteHITCOUNT->Render(dc, 1100-(i*46* (hitsize+1)), 600-31* (hitsize), 0, 46* (hitsize + 1), 62*(hitsize + 1), 46 * (count % 10), 0, 46, 62, 0, 1, 1, 1, hittime/5.0);
		}
		spriteHIT->Render(dc, 1100+46* (hitsize + 1), 618, 0, 87, 44, 0, 0, 87, 44, 0, 1, 1, 1, hittime / 5.0);
	}
}



bool Player::InputMove(float elapsedTime, Camera camera)
{
	float axisX = 0.0f;
	float axisY = 0.0f;
	if (GetAsyncKeyState('W') & 0x8000) axisY += 1.0f;
	if (GetAsyncKeyState('S') & 0x8000) axisY -= 1.0f;
	if (GetAsyncKeyState('D') & 0x8000) axisX += 1.0f;
	if (GetAsyncKeyState('A') & 0x8000) axisX -= 1.0f;

	const DirectX::XMFLOAT3& cameraFront = camera.GetFront();

	//カメラが見下ろしていても、プレイヤーは地面に沿って動く
	const float frontLength = sqrtf(cameraFront.x * cameraFront.x + cameraFront.z * cameraFront.z);
	if (frontLength < 0.0001f)return false;

	const float frontX = cameraFront.x / frontLength;
	const float frontZ = cameraFront.z / frontLength;

	//地面上の前方向から右方向を作る
	const float rightX = frontZ;
	const float rightZ = -frontX;

	float vecX = frontX * axisY + rightX * axisX;
	float vecZ = frontZ * axisY + rightZ * axisX;
	
	//斜め移動が早くならないようにする
	const float length = sqrtf(vecX * vecX + vecZ * vecZ);
	if (length > 1.0f) { vecX /= length;vecZ /= length; }

	//無双モードなら移動速度を早くする
	const float moveMultiplier = IsMusou ? 2.0f : 1.0f;

	const float speed = playerMoveSpeed * moveMultiplier * elapsedTime;

	float moveX = vecX * speed;
	float moveZ = vecZ * speed;
	float moveLength = sqrtf(moveX * moveX + moveZ * moveZ);

	player.position.x += moveX;
	player.position.z += moveZ;

	//移動しているときだけその方向へ向く
	if (moveLength > 0.0f)
	{
		player.angle.y = atan2f(moveX, moveZ);
	}
	
	{
		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(player.scale.x, player.scale.y, player.scale.z);
		DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(player.angle.x, player.angle.y, player.angle.z);
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(player.position.x, player.position.y, player.position.z);
		DirectX::XMStoreFloat4x4(&player.transform, S * R * T);
	}
	
	return moveLength > 0.0f;
}

const char* const Player::animationNames[16] =
{
	"Idle","Death","Drunk_Idle","Drunk_Scyth","Drunk_Swrod",
	"Fight_Idle","Fight_Punch","Fight_Punch2","Fight_Kick",
	"Scyth_Attack","Sword_Attack1","Sword_Attack2","Sword_Walk",
	"Walking","Dodge","Run"
};

void Player::setState(State next, bool loop)
{
	if (state == next)return;

	state = next;
	animation->PlayAnimation(animationNames[static_cast<int>(state)], loop);
}


void Player::pAnimation(float elapsedTime,bool isMoving)
{
	if (isDodging)
	{
		const float speed = 1.63f / dodgeDuration;
		animation->Update(elapsedTime * speed);
		return;
	}

	if (!isAttacking && IsMusou) 
	{
		setState(isMoving ? State::Sword_Walk : State::Drunk_Idle, true);
	}
	else if (!isAttacking)
	{
		setState(isMoving ? State::Sword_Walk : State::Idle, true);
	}

	const float animationSpeed = isAttacking ? attackSpeedMultiplier : 1.0f;
	animation->Update(elapsedTime * animationSpeed);
}

//攻撃開始処理
void Player::StartAttack()
{
	if (isAttacking||isDodging)return;

	const ImGuiIO io = ImGui::GetIO();
	const bool normalPressed = (!io.WantCaptureMouse && (io.MouseDown[ImGuiMouseButton_Right]));
	const bool bloodPressed = (!io.WantCaptureMouse && (io.MouseDown[ImGuiMouseButton_Left]));
	if (!normalPressed && !bloodPressed)return;
	
	if (bloodPressed && !BloodAttack())
	{
		Blood_Attack = false;
		return;
	}
	Blood_Attack = bloodPressed;
	
	const int index = GetBloodGaugeIndex();
	const int attackType = bloodPressed ? (IsMusou ? 2 : 1) : 0;
	currentAttackSetting = attackSetting[index][attackType];

	attackModeSpeedMultiplier = IsMusou ? 1.3f : 1.0f;

	switch (weapon)
	{
	case WEAPON::Sword:		IsSwrod = false; break;
	case WEAPON::Scythe:	IsSlash = false; break;
	case WEAPON::Fist:		IsFight = false; break;
	default:return;
	}

	currentMotionIndex = 0;
	isAttacking = true;
	StartAttackMotion();
}

void Player::StartAttackMotion()
{
	const AttackMotion& motion = currentAttackSetting.motions[currentMotionIndex];
	attackTime = 0.0f;
	attackDuration = motion.duration;
	attackSpeedMultiplier = motion.animationSpeed * attackModeSpeedMultiplier;
	currentHitIndex = -1;
	hitEnemies.clear();
	pAtkDmg = 0.0f;

	state = motion.motion;
	animation->PlayAnimation(animationNames[static_cast<int>(state)], false);
	animation->Update(0.0f);

}

void Player::UpdateAttack(float elapsedTime)
{
	if (!isAttacking)return;

	//フレーム時間を保存
	float remainingTime = elapsedTime;

	while (isAttacking && remainingTime > 0.0f) 
	{
		const float timeToEnd = (attackDuration - attackTime) / attackSpeedMultiplier;
		const bool finished = remainingTime >= timeToEnd;
		const float step = finished ? timeToEnd : remainingTime;
		attackTime += step * attackSpeedMultiplier;
		animation->Update(step * attackSpeedMultiplier);
		remainingTime -= step;

		if (!finished)break;

		++currentMotionIndex;
		if (currentMotionIndex >= currentAttackSetting.motionCount)
		{
			isAttacking = false;
			Blood_Attack = false;
			currentHitIndex = -1;
			hitEnemies.clear();
			pAtkDmg = 0.0f;
			setState(IsMusou ? State::Drunk_Idle : State::Idle, true);
			animation->Update(remainingTime);
			return;
		}
		StartAttackMotion();
	}

	const AttackMotion& motion = currentAttackSetting.motions[currentMotionIndex];
	int nextHitIndex = -1;
	for (int i = 0;i < motion.hitCount;++i)
	{
		if (attackTime >= motion.hits[i].start && attackTime < motion.hits[i].end)
		{
			nextHitIndex = i;
			break;
		}
	}

	if (nextHitIndex >= 0 && nextHitIndex != currentHitIndex)
		hitEnemies.clear();
	currentHitIndex = nextHitIndex;

	if (currentHitIndex < 0)
	{
		pAtkDmg = 0.0f;
		return;
	}
	
	const HitSetting& hit = motion.hits[currentHitIndex];
	attackRadius = hit.radius;
	attackDistance = hit.distance;
	pAtkDmg = hit.damage;

	attackPosition.x = player.position.x + sinf(player.angle.y) * attackDistance;
	attackPosition.y = player.position.y + attackHeight;
	attackPosition.z = player.position.z + cosf(player.angle.y) * attackDistance;

}

bool Player::CheckAttackHit(
	size_t enemyIndex,DirectX::XMFLOAT3 enemyPosition, float enemyRadius)
{
	//攻撃していない、または命中済みなら判定しない
	if (!IsAttackActive())return false;

	//すでに当たったなら判定しない
	if (hitEnemies.count(enemyIndex) > 0)return false;
	
	float dx = attackPosition.x - enemyPosition.x;
	float dy = attackPosition.y - enemyPosition.y;
	float dz = attackPosition.z - enemyPosition.z;

	float radius = attackRadius + enemyRadius;

	if (dx * dx + dy * dy + dz * dz <= radius * radius)
	{
		//この敵に当たったことを記録する
		hitEnemies.insert(enemyIndex);
		hit++;
		hittime = 5.0f;
		return true;
	}

	return false;
}

bool Player::IsAttackActive()const
{
	return !IsDead
		&& !previewState
		&& isAttacking
		&& currentHitIndex >= 0;
}

void Player::takeDamage(float damage)
{
	if (IsDead || invincibleTime > 0.0f || damage <= 0.0f)return;
	niseHP = playerHP;
	playerHP -= damage;
	nisehptime = 1.0f;

	if (playerHP <= 0.0f)
	{
		playerHP = 0.0f;
		IsDead = true;
		previewState = false;
		state = State::Death;
		animation->PlayAnimation("Death", false);

		//死亡したら攻撃を止める
		isAttacking = false;
		hitEnemies.clear();
		attackTime = 0.0f;
		IsMusou = false;
		return;
	}
	invincibleTime = 0.5f;

}

bool Player::CheckTakeAttack(
	DirectX::XMFLOAT3 enemyPosition, float enemyRadius)
{

	if (IsDead || previewState || invincibleTime)return false;

	DirectX::XMFLOAT3 center = player.position;
	center.y += playerRadiusHeight;
	
	float dx = center.x - enemyPosition.x;
	float dy = center.y - enemyPosition.y;
	float dz = center.z - enemyPosition.z;

	float radius = playerRadius + enemyRadius;

	if (dx * dx + dy * dy + dz * dz <= radius * radius)
	{
		takeDamage(10.0f);
		return true;
	}

	return false;

}

void Player::WeaponChange()
{
	if (isAttacking||isDodging||IsMusou)return;

	if (GetAsyncKeyState('1') & 0x8000)
	{
		weapon = WEAPON::Sword;
		weapon_num = 0;
	}
	else if (GetAsyncKeyState('2') & 0x8000)
	{
		weapon = WEAPON::Scythe;
		weapon_num = 1;
	}
	else if (GetAsyncKeyState('3') & 0x8000)
	{
		weapon = WEAPON::Fist;
		weapon_num = 2;
	}

}

void Player::AddBlood(float amount)
{
	float& gauge = bloodGauge[GetBloodGaugeIndex()];
	gauge += amount;

	if (gauge > 100.0f)
	{
		gauge = 100.0f;
	}

}

void Player::RenderColorPass(RenderContext& rc)
{
	if (!player.model)return;

	Graphics::Instance().GetModelRenderer()->Render(
		rc,
		player.transform,
		player.model.get(),
		ShaderId::Lambert);
}

void Player::RenderVelocityPass(const RenderContext& rc, const DirectX::XMFLOAT4X4& previousViewProjection)
{
	if (!player.model) return;

	Graphics::Instance().GetModelRenderer()->RenderVelocity(
		rc,
		player.transform,
		player.model.get(),
		previousViewProjection);
}

bool Player::BloodAttack()
{
	//現在何の武器を使っているかを取得
	const int index = GetBloodGaugeIndex();
	float& gauge = bloodGauge[index];

	if(IsMusou){}
	else
	{
		//コストが足りなければ返す
		if (gauge < bloodCosts[index])
		{
			return false;
		}
		//ゲージからコストを引く
		gauge -= bloodCosts[index];
		
	}

	Blad = true;
	Blood_Attack = true;
	return true;

}

int Player::GetBloodGaugeIndex()const
{
	switch (weapon)
	{
	case WEAPON::Sword:return 0;
	case WEAPON::Scythe:return 1;
	case WEAPON::Fist:return 2;
	}
	return 0;
}

void Player::ChangeMusouMode()
{

	if (IsMusou || isAttacking)return;

	//Rキーを押して血液量が十分なら無双モードへ
	if (!(GetAsyncKeyState('R') & 0x8000))return;
	if (bloodGauge[GetBloodGaugeIndex()] >= 90.0f)
	{
		IsMusou = true;
	}
}

void Player::UpdateMusouMode(float elapsedTime)
{
	ChangeMusouMode();

	if (!IsMusou) return;

	const int index = GetBloodGaugeIndex();
	float& gauge = bloodGauge[GetBloodGaugeIndex()];

	gauge -= bloodGaugeConsumption[index] * elapsedTime;

	if (gauge <= 0.0f)
	{
		gauge = 0.0f;
		IsMusou = false;
	}
}

void Player::StartDodge()
{
	const bool pressed =
		(GetAsyncKeyState(VK_SPACE) & 0x8000) != 0;

	// 押した瞬間だけtrueになる
	const bool triggered = pressed && !wasDodgePressed;
	wasDodgePressed = pressed;

	if (!triggered)return;

	if (isDodging || isAttacking)return;
	if (ImGui::GetIO().WantCaptureKeyboard)return;

	isDodging = true;
	dodgeTime = 0.0f;

	// プレイヤーが向いている方向
	dodgeDirection.x = sinf(player.angle.y);
	dodgeDirection.y = 0.0f;
	dodgeDirection.z = cosf(player.angle.y);

	// 既に残っている無敵時間を短くしない
	if (invincibleTime < dodgeInvincibleTime)
	{
		invincibleTime = dodgeInvincibleTime;
	}

	state = State::Dodge;
	animation->PlayAnimation(
		animationNames[static_cast<int>(state)], false);
}

void Player::UpdateDodge(float elapsedTime)
{
	if (!isDodging)return;

	// 最終フレームで移動しすぎないようにする
	const float remaining = dodgeDuration - dodgeTime;
	const float moveTime =
		elapsedTime < remaining ? elapsedTime : remaining;

	const float speed = dodgeDistance / dodgeDuration;

	player.position.x += dodgeDirection.x * speed * moveTime;
	player.position.z += dodgeDirection.z * speed * moveTime;

	dodgeTime += moveTime;

	// 座標の変更を描画用の行列に反映する
	const DirectX::XMMATRIX S = DirectX::XMMatrixScaling(
		player.scale.x, player.scale.y, player.scale.z);

	const DirectX::XMMATRIX R =
		DirectX::XMMatrixRotationRollPitchYaw(
			player.angle.x, player.angle.y, player.angle.z);

	const DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(
		player.position.x, player.position.y, player.position.z);

	DirectX::XMStoreFloat4x4(&player.transform, S * R * T);

	if (dodgeTime >= dodgeDuration)
	{
		isDodging = false;
	}
}

///////////////////////////////////////////////////////
void Player::DrawGUI()
{
	ImGui::Separator();
	ImGui::Text("Player");
	ImGui::Text("HP: %.0f", playerHP);
	ImGui::Separator();
	ImGui::Text("Blood gauges");

	const char* weaponNames[] = { "Sword", "Scythe", "Fist" };
	const int equipped = GetBloodGaugeIndex();

	for (int i = 0; i < 3; ++i)
	{
		const float blood = bloodGauge[i];

		ImGui::Text("%s%s: %.0f / 100",
			weaponNames[i],
			i == equipped ? " (Equipped)" : "",
			blood);

		ImGui::ProgressBar(blood / 100.0f);
	}

	if (ImGui::Button("Add blood +15"))
	{
		AddBlood(15.0f);
	}

	ImGui::SameLine();
	if (ImGui::Button("Fill blood"))
	{
		AddBlood(100.0f);
	}

	ImGui::SameLine();
	if (ImGui::Button("Reset blood"))
	{
		bloodGauge[equipped] = 0.0f;
	}

	ImGui::Text("Dead: %s", IsDead ? "Yes" : "No");
	ImGui::Text("Invincible time: %.2f", invincibleTime);

	ImGui::Text("Attack damage: %.1f", pAtkDmg);
	ImGui::SliderFloat("Player radius", &playerRadius, 0.1f, 10.0f);

	if (ImGui::Button("Take damage 10"))
	{
		takeDamage(10.0f);
	}
	if (ImGui::CollapsingHeader("Player transform",
		ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Text("Position");
		ImGui::Text("X: %.2f  Y: %.2f  Z: %.2f",
			player.position.x,
			player.position.y,
			player.position.z);

		// ラジアンを見やすい度数法に変換する
		ImGui::Text("Rotation (degrees)");
		ImGui::Text("X: %.1f  Y: %.1f  Z: %.1f",
			DirectX::XMConvertToDegrees(player.angle.x),
			DirectX::XMConvertToDegrees(player.angle.y),
			DirectX::XMConvertToDegrees(player.angle.z));

		ImGui::Text("Scale");
		ImGui::Text("X: %.3f  Y: %.3f  Z: %.3f",
			player.scale.x,
			player.scale.y,
			player.scale.z);

		ImGui::Text("Attack center");
		ImGui::Text("X: %.2f  Y: %.2f  Z: %.2f",
			attackPosition.x,
			attackPosition.y,
			attackPosition.z);
	}


	ImGui::Text("State: %s", animationNames[static_cast<int>(state)]);
	ImGui::Text("Attacking: %s", isAttacking ? "Yes" : "No");
	ImGui::Text("Hit enemies: %d", static_cast<int>(hitEnemies.size()));
	ImGui::Text("Attack time: %.2f / %.2f",
		attackTime, attackDuration);

	ImGui::Checkbox("Show attack collision", &showAttackCollision);

	ImGui::Text("Current hit: %d", currentHitIndex + 1);
	ImGui::Text("Current damage: %.1f", pAtkDmg);
	ImGui::Text("Current speed: %.2f", attackSpeedMultiplier);

	// 編集対象を保存する。実際の装備は変更しない
	static int editWeapon = 0;
	static int editAttackType = 0;

	const char* editWeaponNames[] = { "Sword", "Scythe", "Fist" };
	const char* attackTypeNames[] = { "Normal", "Blood","Musou" };

	ImGui::Separator();
	ImGui::Text("Attack settings");

	ImGui::Combo("Edit weapon", &editWeapon,
		editWeaponNames, IM_ARRAYSIZE(editWeaponNames));

	ImGui::Combo("Attack type", &editAttackType,
		attackTypeNames, IM_ARRAYSIZE(attackTypeNames));

	// 設定表そのものを編集する
	AttackSetting& setting =
		attackSetting[editWeapon][editAttackType];

	ImGui::Text("Playing motion: %d / %d",
		isAttacking ? currentMotionIndex + 1 : 0,
		isAttacking ? currentAttackSetting.motionCount : 0);
	ImGui::SliderInt("Motion count", &setting.motionCount, 1, 5);
	if (setting.motionCount < 1)setting.motionCount = 1;
	if (setting.motionCount > 5)setting.motionCount = 5;
	ImGui::SliderFloat("Hit height (shared)", &attackHeight, 0.0f, 40.0f);

	// Matches animationNames, using lengths from the supplied Player.mdl.
	static const float clipDurations[16] =
	{
		1.966667f, 2.966667f, 3.966667f, 1.1f, 3.133333f,
		2.2f, 0.866667f, 2.266667f, 1.6f, 1.1f,
		2.4f, 1.8f, 1.366667f, 1.033333f, 1.633333f, 0.6f
	};
	for (int m = 0; m < setting.motionCount; ++m)
	{
		ImGui::PushID(m);
		ImGui::Separator();
		ImGui::Text("Motion %d", m + 1);
		AttackMotion& motion = setting.motions[m];
		int selected = static_cast<int>(motion.motion);
		if (ImGui::Combo("Animation", &selected,
			animationNames, IM_ARRAYSIZE(animationNames)))
		{
			motion.motion = static_cast<State>(selected);
			motion.duration = clipDurations[selected];
		}
		ImGui::SliderFloat("Duration", &motion.duration,
			0.01f, clipDurations[static_cast<int>(motion.motion)]);
		if (motion.duration < 0.01f)motion.duration = 0.01f;
		const float clipLength = clipDurations[static_cast<int>(motion.motion)];
		if (motion.duration > clipLength)motion.duration = clipLength;
		ImGui::SliderFloat("Animation speed", &motion.animationSpeed, 0.1f, 3.0f);
		if (motion.animationSpeed < 0.1f)motion.animationSpeed = 0.1f;
		if (motion.animationSpeed > 3.0f)motion.animationSpeed = 3.0f;
		ImGui::SliderInt("Hit count", &motion.hitCount, 0, 5);
		if (motion.hitCount < 0)motion.hitCount = 0;
		if (motion.hitCount > 5)motion.hitCount = 5;

		for (int i = 0; i < motion.hitCount; ++i)
		{
			ImGui::PushID(i);
			ImGui::Text("Hit %d", i + 1);
			HitSetting& hit = motion.hits[i];
			ImGui::SliderFloat("Start", &hit.start, 0.0f, motion.duration);
			ImGui::SliderFloat("End", &hit.end, 0.0f, motion.duration);
			ImGui::SliderFloat("Radius", &hit.radius, 0.1f, 50.0f);
			ImGui::SliderFloat("Distance", &hit.distance, 0.0f, 50.0f);
			ImGui::SliderFloat("Damage", &hit.damage, 0.0f, 500.0f);
			if (hit.radius < 0.1f)hit.radius = 0.1f;
			if (hit.distance < 0.0f)hit.distance = 0.0f;
			if (hit.damage < 0.0f)hit.damage = 0.0f;

			const float earliest = i == 0 ? 0.0f : motion.hits[i - 1].end;
			if (hit.start < earliest)hit.start = earliest;
			if (hit.start > motion.duration)hit.start = motion.duration;
			if (hit.end < hit.start)hit.end = hit.start;
			if (hit.end > motion.duration)hit.end = motion.duration;
			if (hit.start == hit.end)ImGui::Text("Set End later than Start.");
			ImGui::PopID();
		}
		ImGui::PopID();
	}

	ImGui::Text("Settings apply to the next attack.");

	ImGui::Text("Attack active: %s",
		IsAttackActive() ? "Yes" : "No");

	ImGui::Separator();

	if (IsDead)return;

	// チェックを変更した瞬間に実行する
	if (ImGui::Checkbox("State preview", &previewState))
	{
		isAttacking = false;
		attackTime = 0.0f;
		hitEnemies.clear();
		setState(State::Idle, true);
	}

	if (previewState)
	{
		int selectedState = static_cast<int>(state);
		if (ImGui::Combo("Preview state", &selectedState,
			animationNames, IM_ARRAYSIZE(animationNames)))
		{
			setState(static_cast<State>(selectedState), true);
		}
	}
}

