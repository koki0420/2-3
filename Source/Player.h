#pragma once


#include "Graphics.h"
#include"Interaction.h"
#include <Sprite.h>
#include <EffectManager.h>
#include"Object.h"
#include "audio.h"
#include <unordered_set>

#include <imgui.h>

//前方宣言
class audio_source_voice;


class Player
{
public:
	enum class Weapon { Sword, Fist, Scythe };;
	enum class AttackType{Normal,Blood,Special};

	struct WeaponData
	{
		const char* name;
		float damage;
		float reach;//モデルと同じワールド単位
		float radius;
		float duration;
		float bloodCost;
	};

public:
	Player();
	~Player() {};

	void Initialize();


	// 更新処理
	void Update(float elapsedTime, Object& S, const Camera& C);
	// 描画処理
	void Render(float elapsedTime, RenderContext rc,bool Pause);

	void DrawGUI() {

		if (ImGui::CollapsingHeader("player", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::DragFloat3("pPosition", &player.position.x, 0.1f);
			ImGui::DragFloat3("pScale", &player.angle.x, 0.1f);
		}
	}

	void TakeDamage(float damage);
	void Heal(float amount);
	void AddBlood(float amount);
	void OnEnemyDefeated(float bloodAmount = 15.0f);
	bool SetWeapon(Weapon next);
	bool TryAttack(AttackType type);

	//攻撃更新後に敵ごとに呼ぶ。trueなら敵にGetAttackDamage()を適用
	//enemyIdは敵ごとに一意で、生存中に変化しないIDを渡す
	//同じ攻撃では同じ敵に1回だけ命中。敵側の球中心・半径はワールド座標
	bool TryHitEnemy(std::uint64_t enemyId,
		const DirectX::XMFLOAT3& center, float radius);

	const DirectX::XMFLOAT3& GetPos() { return player.position; };
	const DirectX::XMFLOAT3& GetAngle() { return player.angle; };
	const DirectX::XMFLOAT3& GetScale() { return player.scale; };
	float GetHealth()const { return health; }
	float GetBlood()const { return blood; }
	float GetMaxHealth()const { return maxHealth; }
	float GetMaxBlood()const { return maxBlood; }
	bool IsDead()const { return health <= 0.0f; }
	bool IsAttacking()const { return attacking; }
	bool IsAttackActive()const { return attackActive; }
	float GetAttackDamage()const { return attackDamage; }
	Weapon GetWeapon()const { return weapon; }
	int GetBloodLevel()const;//0:通常、1:血液50以上、2:血液80以上
	float GetWeaponGauge(Weapon target)const;
	const WeaponData& GetWeaponData()const;

protected:
	Object player;

private:
	static constexpr float maxHealth = 100.0f;
	static constexpr float maxBlood = 100.0f;
	static constexpr float maxGauge = 100.0f;
	static const std::array<WeaponData, 3>weaponData;

	float health = maxHealth;
	float blood = 0.0f;
	float moveSpeed = 1000.0f;
	float invincibleTime = 0.0f;
	Weapon weapon = Weapon::Sword;
	std::array<float, 3>gauges = {};
	bool attacking = false;
	bool attackActive = false;
	float attackTime = 0.0f;
	float attackDuration = 0.0f;
	float attackDamage = 0.0f;
	float attackReach = 0.0f;
	float attackRadius = 0.0f;
	float attackMoveRate = 1.0f;
	AttackType attackType = AttackType::Normal;
	std::unordered_set<std::uint64_t>hitEnemies;
	std::array<bool, 6>previousKeys = {};

};

