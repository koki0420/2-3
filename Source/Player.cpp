#include <imgui.h>
#include <ImGuizmo.h>
#include "Player.h"
#include <Collision.h>
#include <filesystem>
#include <Windows.h>
#include <cmath>
#include <memory>

constexpr float POS = 0.0f;
constexpr float  Head = 1.0f;

const std::array<Player::WeaponData, 3>Player::weaponData =
{
	{
		{"Sword",20.0f,140.0f,100.0f,0.50f,10.0f},
		{"Fist",12.0f,80.0f,65.0f,0.28f,6.0f},
		{"Scythe",30.0f,180.0f,160.0f,0.80f,15.0f}
	}
};

namespace
{
	float Clamp(float value, float low, float high)
	{
		return value < low ? low : (value > high ? high : value);
	}
	bool VailedWeapon(Player::Weapon weapon)
	{
		return weapon == Player::Weapon::Sword || weapon == Player::Weapon::Fist
			|| weapon == Player::Weapon::Scythe;
	}
}

Player::Player() { Initialize(); }

void Player::Initialize()
{
	health = maxHealth;
	blood = 0.0f;
	gauges.fill(0.0f);
	weapon = Weapon::Sword;
	invincibleTime = 0.0f;
	attacking = attackActive = false;
	attackTime = attackDuration = attackDamage = attackReach = 0.0f;
	attackMoveRate = 1.0f;
	hitEnemies.clear();
	previousKeys.fill(false);
	player.position = { 13, 0, 16 };
	player.angle = { 0,0,0 };
	player.scale = { 1, 1, 1 };
	player.velocity = { 0,0,0 };
	player.model = std::make_unique<Model>("Data/Model/Mr.Incredible/Mr.Incredible.mdl");

	player.UpdateTransform();
}

int Player::GetBloodLevel()const
{
	return blood >= 80.0f ? 2 : (blood >= 50.0f ? 1 : 0);
}

float Player::GetWeaponGauge(Weapon target)const
{
	return VailedWeapon(target) ? gauges[static_cast<std::size_t>(target)] : 0.0f;
}

const Player::WeaponData& Player::GetWeaponData()const
{
	return weaponData[static_cast<std::size_t>(weapon)];
}

bool Player::SetWeapon(Weapon next)
{
	if (IsDead() || attacking || attackActive || !VailedWeapon(next))return false;
	weapon = next;
	return true;
}

void Player::TakeDamage(float damage)
{
	if (damage <= 0.0f || IsDead() || invincibleTime > 0.0f)return;
	health = Clamp(health - damage, 0.0f, maxHealth);
	invincibleTime = 0.5f;
	if (IsDead())
	{
		attacking = attackActive = false;
		attackDamage = 0.0f;
	}

}

void Player::Heal(float amount)
{
	if (amount > 0.0f && !IsDead())health = Clamp(health + amount, 0.0f, maxHealth);
}

void Player::AddBlood(float amount)
{
	if (amount > 0.0f && !IsDead())blood = Clamp(blood + amount, 0.0f, maxBlood);
}

void Player::OnEnemyDefeated(float bloodAmount)
{
	//血液を拾った瞬間に呼ぶ
	AddBlood(bloodAmount);
}

bool Player::TryAttack(AttackType type)
{
	if (IsDead() || attacking || attackActive)return false;
	if (type != AttackType::Normal && type != AttackType::Blood 
		&& type != AttackType::Special)return false;
	const WeaponData& data = GetWeaponData();
	float& gauge = gauges[ static_cast<std::size_t>(weapon) ];
}

// 更新処理
void Player::Update(float elapsedTime, Object& S, const Camera& C)
{
	// プレイヤー移動処理
	{
		// 入力処理
		float axisX = 0.0f;
		float axisY = 0.0f;
		if (GetAsyncKeyState('W') & 0x8000) axisY += 1.0f;
		if (GetAsyncKeyState('S') & 0x8000) axisY -= 1.0f;
		if (GetAsyncKeyState('D') & 0x8000) axisX += 1.0f;
		if (GetAsyncKeyState('A') & 0x8000) axisX -= 1.0f;

		// カメラの方向
		const DirectX::XMFLOAT3& cameraFront = C.GetFront();
		const DirectX::XMFLOAT3& camemraRight = C.GetRight();
		float cameraFrontLengthXZ = sqrtf(cameraFront.x * cameraFront.x + cameraFront.z * cameraFront.z);
		float cameraRightLengthXZ = sqrtf(camemraRight.x * camemraRight.x + camemraRight.z * camemraRight.z);
		float cameraFrontX = cameraFront.x / cameraFrontLengthXZ;
		float cameraFrontZ = cameraFront.z / cameraFrontLengthXZ;
		float cameraRightX = camemraRight.x / cameraRightLengthXZ;
		float cameraRightZ = camemraRight.z / cameraRightLengthXZ;

		// 移動ベクトル
		const float speed = 1000.0f * elapsedTime;
		float vecX = cameraFrontX * axisY + cameraRightX * axisX;
		float vecZ = cameraFrontZ * axisY + cameraRightZ * axisX;
		float moveX = vecX * speed;
		float moveZ = vecZ * speed;
		float moveLength = sqrtf(moveX * moveX + moveZ * moveZ);
		
		player.position.x += moveX;
		player.position.z += moveZ;

		// 移動しているときだけ向きを更新する
		if (moveLength > 0.0f)
		{
			float vx = moveX / moveLength;
			float vz = moveZ / moveLength;
			player.angle.y = atan2f(vx, vz);
		}

	}

	player.UpdateTransform();
	//// プレイヤー行列更新処理
	//{
	//	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(player.scale.x, player.scale.y, player.scale.z);
	//	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(player.angle.x, player.angle.y, player.angle.z);
	//	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(player.position.x, player.position.y, player.position.z);
	//	DirectX::XMStoreFloat4x4(&player.transform, S * R * T);
	//}


}

// 描画処理
void Player::Render(float elapsedTime, RenderContext rc,bool Pause)
{
	/*ID3D11DeviceContext* dc = Graphics::Instance().GetDeviceContext();
	RenderState* renderState = Graphics::Instance().GetRenderState();*/
	ModelRenderer* modelRenderer = Graphics::Instance().GetModelRenderer();

	modelRenderer->Render(rc, player.transform, player.model.get(), ShaderId::Lambert);

}
