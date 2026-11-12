#include "Player.h"
#include <Windows.h>
#include <imgui.h>

#include <cmath>
#include "ShapeRenderer.h"
#define PLAYERHPMAX 10000.0f
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
	bool isMoving = false;
	
	if (!isAttacking){isMoving = InputMove(elapsedTime, camera);}

	//武器切り替え
	WeaponChange();

	//Update
	UpdateMusouMode(elapsedTime);
	UpdateAttack(elapsedTime);
	pAnimation(elapsedTime, isMoving);

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

	Graphics::Instance().GetModelRenderer()->Render(rc, Weapon[weapon_num].transform, Weapon[weapon_num].model.get(), ShaderId::Lambert);

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
		Sword_Effect->Play({ player.position.x,player.position.y + 5.0f,player.position.z }, 7.0f);
		IsSwrod = true;
		Blad = false;
	}

	if (animation->GetCurrentFrame() >= 20.0f && !IsFight && Blad)
	{
		Fight_Effect->Play({ attackPosition.x,attackPosition.y ,attackPosition.z + 10.0f }, 7.0f);
		IsFight = true;
		Blad = false;
	}

	if (animation->GetCurrentFrame() >= 15.0f && !IsSlash && Blad)
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
	spriteHP->Render(dc, 60, 50, 0, 480 * (hp / PLAYERHPMAX), 32, 8, 48, 112 * (hp / PLAYERHPMAX), 32, 0, 1, 1, 1, 1);
	spriteW[GetBloodGaugeIndex()]->Render(dc, 10, 50, 0, 56, 56, 0, 0, 134, 134, 0, 1, 1, 1, 1);
	spriteGauge->Render(dc, 50, 82, 0, 280 , 28, 0, 48, 128 , 32, 0, 1, 1, 1, 1);
	spriteWP->Render(dc, 50, 82, 0, 280 * (bloodGauge[GetBloodGaugeIndex()] / 100), 28, 0, 48, 128 * (bloodGauge[GetBloodGaugeIndex()] / 100), 32, 0, 1, 1, 1, 1);

	
	spriteGauge->Render(dc, 80, 500, 0, 280, 28, 0, 48, 128, 32, 0, 1, 1, 1, 1);
	spriteWP->Render(dc, 80, 500, 0, 280 * (bloodGauge[0] / 100), 28, 0, 48, 128 * (bloodGauge[0] / 100), 32, 0, 1, 1, 1, 1);
	spriteW[0]->Render(dc, 60, 500, 0, 28, 28, 0, 0, 134, 134, 0, 1, 1, 1, 1);
	

	spriteGauge->Render(dc, 80, 532, 0, 280, 28, 0, 48, 128, 32, 0, 1, 1, 1, 1);
	spriteWP->Render(dc, 80, 532, 0, 280 * (bloodGauge[1] / 100), 28, 0, 48, 128 * (bloodGauge[1] / 100), 32, 0, 1, 1, 1, 1);
	spriteW[1]->Render(dc, 60, 532, 0, 28, 28, 0, 0, 134, 134, 0, 1, 1, 1, 1);
	
	spriteGauge->Render(dc, 80, 564, 0, 280, 28, 0, 48, 128, 32, 0, 1, 1, 1, 1);
	spriteWP->Render(dc, 80, 564, 0, 280 * (bloodGauge[2] / 100), 28, 0, 48, 128 * (bloodGauge[2] / 100), 32, 0, 1, 1, 1, 1);
	spriteW[2]->Render(dc, 60, 564, 0, 28, 28, 0, 0, 134, 134, 0, 1, 1, 1, 1);
		
	
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

const char* const Player::animationNames[13] =
{
	"Idle","Death","Drunk_Idle","Drunk_Scythe","Drunk_Sword",
	"Fight_Idle","Fight_Punch1","Fight_Punch2","Fight_kick",
	"Scythe_Attack","Sword_Attack1","Sword_Attack2","Sword_Walk"
};

void Player::setState(State next, bool loop)
{
	if (state == next)return;

	state = next;
	animation->PlayAnimation(animationNames[static_cast<int>(state)], loop);
}


void Player::pAnimation(float elapsedTime,bool isMoving)
{
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
	if (isAttacking)return;

	const ImGuiIO io = ImGui::GetIO();
	const bool normalPressed = (!io.WantCaptureMouse && (io.MouseDown[ImGuiMouseButton_Right]));
	const bool bloodPressed = (!io.WantCaptureMouse && (io.MouseDown[ImGuiMouseButton_Left]));

	if (!normalPressed && !bloodPressed)return;

	//ダメージ、倍率、攻撃範囲初期化

	pAtkDmg = PAD;
	//pAtkDmg = PLAYER_ATTACK_DAMAGE;
	bloodAttackMultiplier = 1.0f;
	float AttackRadius = PLAYER_ATTACK_RADIUS;
	float AttackDistance = PLAYER_ATTACK_DISTANCE;

	if (bloodPressed && !BloodAttack())return;
	
	State next;
	float duration;//時間
	const int index = GetBloodGaugeIndex();

	switch (weapon)
	{
	case WEAPON::Sword:
		next = State::Sword_Attack1;

		IsSwrod = false;

		break;
	case WEAPON::Scythe:
		next = State::Scythe_Attack;

		IsSlash = false;

		break;
	case WEAPON::Fist:
		next = State::Fight_Punch1;
		duration = 0.87f;

		IsFight = false;

		break;
	default:
		return;
	}

	if (bloodPressed) 
	{
		AttackRadius = WeaponAtkRadius[index];
		AttackDistance = bloodAtkDistance[index];
	}
	duration = WeaponDuration[index];

	//血液攻撃倍率をかける(血液攻撃でなければ等倍
	pAtkDmg *= bloodAttackMultiplier;
	//無双モードならさらに攻撃倍率をかけて攻撃速度を上げる
	if (IsMusou)pAtkDmg *= 1.5;
	attackSpeedMultiplier = IsMusou ? 1.3f : 1.0f;

	attackRadius = AttackRadius;
	attackDuration = duration;
	attackDistance = AttackDistance;
	isAttacking = true;
	attackTime = 0.0f;
	hitEnemies.clear();
	setState(next, false);

}

void Player::UpdateAttack(float elapsedTime)
{
	StartAttack();

	if (isAttacking)
	{
		//playerが向いている方向の前方へ配置
		attackPosition.x = player.position.x + sinf(player.angle.y) * attackDistance;
		attackPosition.y = player.position.y + attackHeight;
		attackPosition.z = player.position.z + cosf(player.angle.y) * attackDistance;

		attackTime += elapsedTime * attackSpeedMultiplier;
		if (attackTime >= attackDuration)
		{
			isAttacking = false;
			setState(State::Idle, true);
		}
	}

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
		return true;
	}

	return false;

}

bool Player::IsAttackActive()const
{
	return !IsDead
		&& !previewState
		&& isAttacking
		&& attackTime >= attackStartTime
		&& attackTime < attackEndTime;
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
	if (isAttacking||IsMusou)return;

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
		//weapon_num = 2;
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

bool Player::BloodAttack()
{
	//現在何の武器を使っているかを取得
	const int index = GetBloodGaugeIndex();
	float& gauge = bloodGauge[index];

	//コストが足りなければ返す
	if (gauge < BloodCosts[index])return false;

	//ゲージからコストを引き、通常攻撃に武器ごとの倍率をかける
	gauge -= BloodCosts[index];
	bloodAttackMultiplier = BloodMultipliers[index];

	Blad = true;

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

	gauge -= BloodGaugeConsumption[index] * elapsedTime;

	if (gauge <= 0.0f)
	{
		gauge = 0.0f;
		IsMusou = false;
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

	ImGui::SliderFloat("Attack damage", &pAtkDmg, 0.0f, 100.0f);
	ImGui::SliderFloat("PAD", &PAD, 0.0f, 100.0f);
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

	ImGui::SliderFloat("Attack radius", &attackRadius, 1.0f, 20.0f);
	ImGui::SliderFloat("Attack distance", &attackDistance, 0.0f, 20.0f);
	ImGui::SliderFloat("Attack height", &attackHeight, 1.0f, 30.0f);
	ImGui::Text("Clip duration: %.2f s", attackDuration);

	ImGui::SliderFloat(
		"Hit start", &attackStartTime, 0.0f, 2.4f);

	ImGui::SliderFloat(
		"Hit end", &attackEndTime, 0.0f, 2.4f);

	// 開始時間が終了時間を越えないようにする
	if (attackStartTime > attackEndTime)
	{
		attackStartTime = attackEndTime;
	}

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
