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

	spriteGauge = std::make_unique<Sprite>(Graphics::Instance().GetDevice(), "Data/Sprite/gauge_bar-square_black_01_ff4444_128.png");
	spriteHP = std::make_unique<Sprite>(Graphics::Instance().GetDevice(), "Data/Sprite/gauge_bar_blue-gold_01_44cc44_128.png");
	spriteWP = std::make_unique<Sprite>(Graphics::Instance().GetDevice(), "Data/Sprite/gauge_bar-square_blue-gold_01_ff4444_128.png");

	spriteW[0] = std::make_unique<Sprite>(Graphics::Instance().GetDevice(), "Data/Sprite/sword_transparent.png");
	spriteW[1] = std::make_unique<Sprite>(Graphics::Instance().GetDevice(), "Data/Sprite/scythe_transparent.png");
	spriteW[2] = std::make_unique<Sprite>(Graphics::Instance().GetDevice(), "Data/Sprite/glove_transparent.png");


}
Player::~Player()
{
	if (animation)
	{
		delete animation;
	}
}
void Player::Initialize()
{
    
    player.position = { 0, -15, 0 };
    player.angle = { 0, 0, 0 };
    player.scale = { 0.08, 0.08, 0.08 };
    player.velocity = { 0, 0, 0 };
    player.model = std::make_unique<Model>("Data/Model/Player/Player.mdl");
	animation = new Animation(*player.model);

	state = State::Idle;
	animation->PlayAnimation("Idle", true);
    
}

void Player::Update(float elapsedTime, Object& stage, const Camera& camera)
{

	if (IsDead)return;


	// debug
	if (previewState)
	{
		animation->Update(elapsedTime);
		return;
	}

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
	UpdateAttack(elapsedTime);
	pAnimation(elapsedTime, isMoving);
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

	pPos.y += 2.0f;

	Graphics::Instance().GetShapeRenderer()->DrawSphere(
		pPos,
		playerRadius,
		{ 1, 1, 0, 1 });
}

void Player::UIRender(float elapsedTime, ID3D11DeviceContext* dc)
{
	spriteHPGauge->Render(dc, 50, 50, 0, 480, 32, 0, 48, 128, 32, 0, 1, 1, 1, 1);
	spriteHP->Render(dc, 50, 50, 0, 480*(playerHP/PLAYERHPMAX), 32, 0, 48, 128 * (playerHP / PLAYERHPMAX), 32, 0, 1, 1, 1, 1);
	spriteW[wt]->Render(dc, 10, 50, 0, 56, 56, 0, 0, 134, 134, 0, 1, 1, 1, 1);
	spriteGauge->Render(dc, 50, 82, 0, 280, 28, 0, 48, 128, 32, 0, 1, 1, 1, 1);
	spriteWP->Render(dc, 50, 82, 0, 280, 28, 0, 48, 128, 32, 0, 1, 1, 1, 1);

	spriteGauge->Render(dc, 80, 500, 0, 280, 28, 0, 48, 128, 32, 0, 1, 1, 1, 1);
	spriteWP->Render(dc, 80, 500, 0, 280, 28, 0, 48, 128, 32, 0, 1, 1, 1, 1);
	spriteW[wt]->Render(dc, 60, 500, 0, 28, 28, 0, 0, 134, 134, 0, 1, 1, 1, 1);

	spriteGauge->Render(dc, 80, 532, 0, 280, 28, 0, 48, 128, 32, 0, 1, 1, 1, 1);
	spriteWP->Render(dc, 80, 532, 0, 280, 28, 0, 48, 128, 32, 0, 1, 1, 1, 1);
	spriteW[(1+ wt)%3]->Render(dc, 60, 532, 0, 28, 28, 0, 0, 134, 134, 0, 1, 1, 1, 1);

	spriteGauge->Render(dc, 80, 564, 0, 280, 28, 0, 48, 128, 32, 0, 1, 1, 1, 1);
	spriteWP->Render(dc, 80, 564, 0, 280, 28, 0, 48, 128, 32, 0, 1, 1, 1, 1);
	spriteW[(2 + wt)%3]->Render(dc, 60, 564, 0, 28, 28, 0, 0, 134, 134, 0, 1, 1, 1, 1);
}



bool Player::InputMove(float elapsedTime, Camera camera)
{
	// 入力�?�?
	float axisX = 0.0f;
	float axisY = 0.0f;
	if (GetAsyncKeyState('W') & 0x8000) axisY += 1.0f;
	if (GetAsyncKeyState('S') & 0x8000) axisY -= 1.0f;
	if (GetAsyncKeyState('D') & 0x8000) axisX += 1.0f;
	if (GetAsyncKeyState('A') & 0x8000) axisX -= 1.0f;

	// カメラの方�?
	const DirectX::XMFLOAT3& cameraFront = camera.GetFront();
	const DirectX::XMFLOAT3& camemraRight = camera.GetRight();
	float cameraFrontLengthXZ = sqrtf(cameraFront.x * cameraFront.x + cameraFront.z * cameraFront.z);
	float cameraRightLengthXZ = sqrtf(camemraRight.x * camemraRight.x + camemraRight.z * camemraRight.z);
	float cameraFrontX = cameraFront.x / cameraFrontLengthXZ;
	float cameraFrontZ = cameraFront.z / cameraFrontLengthXZ;
	float cameraRightX = camemraRight.x / cameraRightLengthXZ;
	float cameraRightZ = camemraRight.z / cameraRightLengthXZ;

	// 移動�?クトル
	const float speed = 5.0f * elapsedTime;
	float vecX = cameraFrontX * axisY + cameraRightX * axisX;
	float vecZ = cameraFrontZ * axisY + cameraRightZ * axisX;
	float moveX = vecX * speed;
	float moveZ = vecZ * speed;
	float moveLength = sqrtf(moveX * moveX + moveZ * moveZ);

	player.position.x += moveX;
	player.position.z += moveZ;

	//移動して�?��ときだけその方向へ向く
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



void Player::pAnimation(float elapsedTime,bool isMoving)
{
	switch (state)
	{
	case State::Idle:
	{
		if (isMoving)
		{
			state = State::Run;
			animation->PlayAnimation("Sword_Walk", true);
		}
		break;
	}
	case State::Run:
		if (!isMoving)
		{
			state = State::Idle;
			animation->PlayAnimation("Idle", true);
		}
		break;

	}

	animation->Update(elapsedTime);

}

//攻�?��始�?�?
void Player::StartAttack()
{
	if (isAttacking)return;

	if (GetAsyncKeyState('J') & 0x8000)
	{
		state = State::SwordAttack1;
		isAttacking = true;
		attackTime = 0.0f;
		hitEnemies.clear();
		animation->PlayAnimation("Sword_Attack1", false);
	}
	
	
}

void Player::UpdateAttack(float elapsedTime)
{
	StartAttack();

	if (isAttacking)
	{
		//playerが向�?���?��方向�?前方へ配置
		attackPosition.x = player.position.x + sinf(player.angle.y) * attackDistance;
		attackPosition.y = player.position.y + attackHeight;
		attackPosition.z = player.position.z + cosf(player.angle.y) * attackDistance;

		attackTime += elapsedTime;
		if (attackTime > attackDuration)
		{
			state = State::Idle;
			isAttacking = false;
			animation->PlayAnimation("Idle", true);
		}
	}

}

bool Player::CheckAttackHit(
	size_t enemyIndex,DirectX::XMFLOAT3 enemyPosition, float enemyRadius)
{
	//攻�?��て�?���?��また�?命中済みなら判定しな�?
	if (!IsAttackActive())return false;

	//今回すでに当たった敵なら判定しな�?
	if (hitEnemies.count(enemyIndex) > 0)return false;

	//2つの�??中�??差
	float dx = attackPosition.x - enemyPosition.x;
	float dy = attackPosition.y - enemyPosition.y;
	float dz = attackPosition.z - enemyPosition.z;

	float radius = attackRadius + enemyRadius;

	if (dx * dx + dy * dy + dz * dz <= radius * radius)
	{
		//こ�?敵に当たったことを記録する
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

		//死亡したら攻�?��止める
		isAttacking = false;
		hitEnemies.clear();
		attackTime = 0.0f;
		return;
	}
	invincibleTime = 0.5f;

}

bool Player::CheckTakeAttack(
	size_t enemyIndex, DirectX::XMFLOAT3 enemyPosition, float enemyRadius)
{

	if (IsDead || previewState || invincibleTime)return false;

	DirectX::XMFLOAT3 center = player.position;
	center.y += 2.0f;
	
	//2つの�??中�??差
	float dx = center.x - enemyPosition.x;
	float dy = center.y - enemyPosition.y;
	float dz = center.z - enemyPosition.z;


	float radius = playerRadius + enemyRadius;

	/*spriteHPGauge->Render(dc, 50, 50, 0, 480, 32, 0, 48, 128, 32, 0, 1, 1, 1, 1);
	spriteHP->Render(dc, 50, 50, 0, 480, 32, 0, 48, 128, 32, 0, 1, 1, 1, 1);
	spriteW[wt]->Render(dc, 10, 50, 0, 56, 56, 0, 0, 134, 134, 0, 1, 1, 1, 1);
	spriteGauge->Render(dc, 50, 82, 0, 280, 28, 0, 48, 128, 32, 0, 1, 1, 1, 1);
	spriteWP->Render(dc, 50, 82, 0, 280, 28, 0, 48, 128, 32, 0, 1, 1, 1, 1);*/


	if (dx * dx + dy * dy + dz * dz <= radius * radius)
	{
		takeDamage(10.0f);
		return true;
	}

	return false;

}

void Player::DrawGUI()
{
	ImGui::Separator();
	ImGui::Text("Player");
	ImGui::Text("HP: %.0f", playerHP);
	ImGui::Text("Dead: %s", IsDead ? "Yes" : "No");
	ImGui::Text("Invincible time: %.2f", invincibleTime);

	ImGui::SliderFloat("Attack damage", &pAtkDmg, 0.0f, 100.0f);
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

		// 保存されて�?��ラジアンを、見やすい度数法に変換する
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

	const char* stateNames[] =
	{
		"Idle",
		"Run",
		"SwordAttack1"
	};

	//ImGui::Text("State: %s", stateNames[static_cast<int>(state)]);
	ImGui::Text("Attacking: %s", isAttacking ? "Yes" : "No");
	ImGui::Text("Hit enemies: %d", static_cast<int>(hitEnemies.size()));
	ImGui::Text("Attack time: %.2f / %.2f",
		attackTime, attackDuration);

	ImGui::Checkbox("Show attack collision", &showAttackCollision);

	ImGui::SliderFloat("Attack radius", &attackRadius, 0.1f, 10.0f);
	ImGui::SliderFloat("Attack distance", &attackDistance, 0.0f, 10.0f);
	ImGui::SliderFloat("Attack height", &attackHeight, -30.0f, 10.0f);
	ImGui::SliderFloat("Attack duration", &attackDuration, 0.1f, 5.0f);

	ImGui::SliderFloat(
		"Hit start", &attackStartTime, 0.0f, attackDuration);

	ImGui::SliderFloat(
		"Hit end", &attackEndTime, 0.0f, attackDuration);

	// 攻�??体�?時間を短くした場合も、�?���?��収め�?
	if (attackEndTime > attackDuration)
	{
		attackEndTime = attackDuration;
	}

	// 開始時間が終�?��間を�?��な�?���?��する
	if (attackStartTime > attackEndTime)
	{
		attackStartTime = attackEndTime;
	}

	ImGui::Text("Attack active: %s",
		IsAttackActive() ? "Yes" : "No");

	ImGui::Separator();

	// チェ�?��を変更した瞬間に実行す�?
	if (ImGui::Checkbox("State preview", &previewState))
	{
		state = State::Idle;
		isAttacking = false;
		
		attackTime = 0.0f;

		animation->PlayAnimation("Idle", true);
	}

	if (previewState)
	{
		int selectedState = static_cast<int>(state);

		// 選択を変更した瞬間に実行す�?
		if (ImGui::Combo("Preview state", &selectedState,
			stateNames, IM_ARRAYSIZE(stateNames)))
		{
			state = static_cast<State>(selectedState);

			switch (state)
			{
			case State::Idle:
				animation->PlayAnimation("Idle", true);
				break;

			case State::Run:
				animation->PlayAnimation("Sword_Walk", true);
				break;

			case State::SwordAttack1:
				// 確認用なので繰り返し再生する
				animation->PlayAnimation("Sword_Attack1", true);
				break;

			case State::SwordAttack2:
				animation->PlayAnimation("Sword_Attack2", true);
				break;
			}
		}
	}
}
