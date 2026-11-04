#include "Player.h"
#include <Windows.h>
#include <imgui.h>

#include <cmath>
#include "ShapeRenderer.h"
#define PLAYERHPMAX 100
Player::Player()
{


	Initialize();

	spriteHPGauge = std::make_unique<Sprite>(Graphics::Instance().GetDevice(), "Data/Sprite/gauge_bar_neon-green_01_ff4444_128.png");

	spriteHPGauge = std::make_unique<Sprite>(Graphics::Instance().GetDevice(), "Data/Sprite/gauge_bar_neon-green_01_ff4444_128.png");


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
    player.position = { 0, -15, 0 };

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
    
}

void Player::Update(float elapsedTime, Object& stage, const Camera& camera)
{
	//死んでいたら実行しない
	if (IsDead)
	{
		animation->Update(elapsedTime);
		return;
	}

	// debug
	if (previewState)
	{
		animation->Update(elapsedTime);
		return;
	}

	//無敵時間
	if (invincibleTime > 0.0f)
	{
		invincibleTime -= elapsedTime;
		if (invincibleTime < 0.0f)
		{
			invincibleTime = 0.0f;
		}
	}

	bool isMoving = false;
	
	if (!isAttacking)
	{
		isMoving = InputMove(elapsedTime, camera);
	}

	WeaponChange();

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

	Graphics::Instance().GetModelRenderer()->Render(rc, Weapon[weapon_num].transform, Weapon[weapon_num].model.get(), ShaderId::Lambert);





	DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&rc.camera->GetView());
	DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&rc.camera->GetProjection());
	DirectX::XMFLOAT4X4 VP;
	DirectX::XMStoreFloat4x4(&VP, V * P);
	if (isAttacking)
	{
		sword_Line.Render(Graphics::Instance().GetDeviceContext(), VP);

	}
}
	

void Player::UIRender(float elapsedTime, ID3D11DeviceContext* dc)
{
	spriteHPGauge->Render(dc, 50, 50, 0, 480, 32, 0, 48, 128, 32, 0, 1, 1, 1, 1);
	spriteHP->Render(dc, 50, 50, 0, 480 * (playerHP / PLAYERHPMAX), 32, 0, 48, 128 * (playerHP / PLAYERHPMAX), 32, 0, 1, 1, 1, 1);
	spriteW[GetBloodGaugeIndex()]->Render(dc, 10, 50, 0, 56, 56, 0, 0, 134, 134, 0, 1, 1, 1, 1);
	spriteGauge->Render(dc, 50, 82, 0, 280 , 28, 0, 48, 128 , 32, 0, 1, 1, 1, 1);
	spriteWP->Render(dc, 50, 82, 0, 280 * (bloodGauge[GetBloodGaugeIndex()] / 100), 28, 0, 48, 128 * (bloodGauge[GetBloodGaugeIndex()] / 100), 32, 0, 1, 1, 1, 1);

	switch (weapon_num)
	{

	case 0:
		spriteGauge->Render(dc, 80, 500, 0, 280, 28, 0, 48, 128, 32, 0, 1, 1, 1, 1);
		spriteWP->Render(dc, 80, 500, 0, 280 * (bloodGauge[0] / 100), 28, 0, 48, 128 * (bloodGauge[0] / 100), 32, 0, 1, 1, 1, 1);
		spriteW[0]->Render(dc, 60, 500, 0, 28, 28, 0, 0, 134, 134, 0, 1, 1, 1, 1);
		break;
	case 1:

		spriteGauge->Render(dc, 80, 532, 0, 280, 28, 0, 48, 128, 32, 0, 1, 1, 1, 1);
		spriteWP->Render(dc, 80, 532, 0, 280 * (bloodGauge[1] / 100), 28, 0, 48, 128 * (bloodGauge[1] / 100), 32, 0, 1, 1, 1, 1);
		spriteW[1]->Render(dc, 60, 532, 0, 28, 28, 0, 0, 134, 134, 0, 1, 1, 1, 1);
		break;
	case 2:
		spriteGauge->Render(dc, 80, 564, 0, 280, 28, 0, 48, 128, 32, 0, 1, 1, 1, 1);
		spriteWP->Render(dc, 80, 564, 0, 280 * (bloodGauge[2] / 100), 28, 0, 48, 128 * (bloodGauge[2] / 100), 32, 0, 1, 1, 1, 1);
		spriteW[2]->Render(dc, 60, 564, 0, 28, 28, 0, 0, 134, 134, 0, 1, 1, 1, 1);
		break;
	}
}



bool Player::InputMove(float elapsedTime, Camera camera)
{
	// 蜈･蜉帛?逅?
	float axisX = 0.0f;
	float axisY = 0.0f;
	if (GetAsyncKeyState('W') & 0x8000) axisY += 1.0f;
	/*if (GetAsyncKeyState('S') & 0x8000) axisY -= 1.0f;*/
	if (GetAsyncKeyState('D') & 0x8000) axisX += 1.0f;
	if (GetAsyncKeyState('A') & 0x8000) axisX -= 1.0f;

	//const bool isMoving = axisX != 0.0f||axisY != 0.0f;

	////停止から移動を始めた瞬間だけ、基準方向を保存する
	//if (isMoving && !wasMoving)
	//{
	//	moveFront = camera.GetFront();
	//	moveRight = camera.GetRight();
	//}

	//wasMoving = isMoving;

	// 繧ｫ繝｡繝ｩ縺ｮ譁ｹ蜷?
	const DirectX::XMFLOAT3& cameraFront = camera.GetFront();
	const DirectX::XMFLOAT3& camemraRight = camera.GetRight();

	/*const DirectX::XMFLOAT3& cameraFront = moveFront;
	const DirectX::XMFLOAT3& camemraRight = moveRight;*/
	float cameraFrontLengthXZ = sqrtf(cameraFront.x * cameraFront.x + cameraFront.z * cameraFront.z);
	float cameraRightLengthXZ = sqrtf(camemraRight.x * camemraRight.x + camemraRight.z * camemraRight.z);
	float cameraFrontX = cameraFront.x / cameraFrontLengthXZ;
	float cameraFrontZ = cameraFront.z / cameraFrontLengthXZ;
	float cameraRightX = camemraRight.x / cameraRightLengthXZ;
	float cameraRightZ = camemraRight.z / cameraRightLengthXZ;

	// 遘ｻ蜍輔?繧ｯ繝医Ν
	const float moveMultiplier = IsMusou ? 2.0f : 1.0f;
	const float speed = playerMoveSpeed * moveMultiplier * elapsedTime;
	float vecX = cameraFrontX * axisY + cameraRightX * axisX;
	float vecZ = cameraFrontZ * axisY + cameraRightZ * axisX;
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
	"Fight_Idle","Fight_Punch1","Fight_Punch2","Fight_Kick",
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
	/*switch (state)
	{
	case State::Idle:
	{
		if (isMoving)
		{
			state = State::Sword_Walk;
			animation->PlayAnimation("Sword_Walk", true);
		}
		break;
	}
	case State::Sword_Walk:
		if (!isMoving)
		{
			state = State::Idle;
			animation->PlayAnimation("Idle", true);
		}
		break;
	}*/
	
	/*if (!isMoving)
	{
		state = State::Idle;
		animation->PlayAnimation("Idle", true);
	}*/

	if (!isAttacking)
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
	
	const bool normalPressed = (GetAsyncKeyState('J') & 0x8000) != 0;
	const bool bloodPressed = (GetAsyncKeyState('K') & 0x8000) != 0;

	if (!normalPressed && !bloodPressed)return;


	pAtkDmg = PAD;
	bloodAttackMultiplier = 1.0f;
	float AttackRadius = atkR;

	if (bloodPressed && !BloodAttack())return;

	State next;
	float duration;

	switch (weapon)
	{
	case WEAPON::Sword:
		next = State::Sword_Attack1;
		duration = 2.4f;
		break;
	case WEAPON::Scythe:
		next = State::Scythe_Attack;
		duration = 1.1f;
		if (bloodPressed)AttackRadius = 50.0f;
		break;
	case WEAPON::Fist:
		next = State::Fight_Punch1;
		duration = 0.87f;
		break;
	default:
		return;
	}

	attackRadius = AttackRadius;
	pAtkDmg *= bloodAttackMultiplier;
	if (IsMusou)pAtkDmg *= 1.5;
	attackSpeedMultiplier = IsMusou ? 1.3f : 1.0f;
	attackDuration = duration;
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

	playerHP -= damage;

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
	if (isAttacking)return;

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
	const int index = GetBloodGaugeIndex();
	const float costs[] = { 5.0f,10.0f,2.5f };
	const float multipliers[] = { 1.3f,1.5f,1.1f };

	float& gauge = bloodGauge[index];

	if (gauge < costs[index])return false;

	gauge -= costs[index];
	bloodAttackMultiplier = multipliers[index];
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

	float& gauge = bloodGauge[GetBloodGaugeIndex()];
	gauge -= 10.0f * elapsedTime;

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
