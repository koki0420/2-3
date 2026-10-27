#pragma once


#include <array>
#include <cstdint>
#include <memory>
#include <unordered_set>
#include "Graphics.h"
#include "Camera.h"
#include "Object.h"
#include "PlayerAnimation.h"

class Player
{
public:
    enum class Weapon { Sword, Fist, Scythe };
    enum class AttackType { Normal, Blood, Special };
    struct WeaponData
    {
        const char* name;
        float damage, reach, radius, duration, bloodCost;
    };
    // é¿ç€ÇÃmdlÇ…é˚ò^Ç≥ÇÍÇƒÇ¢ÇÈî‘çÜÅB-1 ÇÕñ¢äÑìñÅB
    struct AnimationClips
    {
        int idle = -1, run = -1, damage = -1, death = -1;
        std::array<int, 3> weaponIdle = { { -1, -1, -1 } };
        std::array<int, 3> normal = { { -1, -1, -1 } };
        std::array<int, 3> blood = { { -1, -1, -1 } };
        std::array<int, 3> special = { { -1, -1, -1 } };
    };

    Player();
    ~Player() = default;
    void Initialize();
    void Update(float elapsedTime, Object& stage, const Camera& camera);
    void Render(float elapsedTime, RenderContext rc, bool pause);
    void DrawGUI();
    void TakeDamage(float damage);
    void Heal(float amount);
    void AddBlood(float amount);
    void OnEnemyDefeated(float bloodAmount = 15.0f);
    bool SetWeapon(Weapon next);
    bool TryAttack(AttackType type);
    bool TryStartMusou();
    bool TryHitEnemy(std::uint64_t enemyId, const DirectX::XMFLOAT3& center, float radius);
    void SetAnimationClips(const AnimationClips& clips);

    const DirectX::XMFLOAT3& GetPos() const { return player.position; }
    const DirectX::XMFLOAT3& GetAngle() const { return player.angle; }
    const DirectX::XMFLOAT3& GetScale() const { return player.scale; }
    float GetHealth() const { return health; }
    float GetBlood() const { return blood; }
    float Getblood() const { return blood; }
    float GetMaxHealth() const { return maxHealth; }
    float GetMaxBlood() const { return maxBlood; }
    bool IsDead() const { return health <= 0.0f; }
    bool IsAttacking() const { return attacking; }
    bool IsAttackActive() const { return attackActive; }
    bool IsMusou() const { return musou; }
    bool CanStartMusou() const { return !IsDead() && !musou && blood >= musouThreshold; }
    float GetAttackDamage() const { return attackDamage; }
    Weapon GetWeapon() const { return weapon; }
    int GetBloodLevel() const { return musou ? 1 : 0; } // å›ä∑ópÅBí~êœÇæÇØÇ≈ÇÕã≠âªÇµÇ»Ç¢ÅB
    float GetWeaponGauge(Weapon target) const;
    const WeaponData& GetWeaponData() const;
    const AnimationClips& GetAnimationClips() const { return clips; }
    DirectX::XMFLOAT3 GetHitCenter() const;
    float GetHitRadius() const { return bodyRadius; }

protected:
    Object player;

private:
    enum class State { Idle, Run, Attack, Damage, Dead };
    void FindAnimationClips();
    void UpdateAnimation(float elapsedTime, bool moved);
    void UpdateVisualTransform();
    int SelectClip(State next) const;
    bool ValidClip(int index) const;

    static constexpr float maxHealth = 100.0f;
    static constexpr float maxBlood = 100.0f;
    static constexpr float maxGauge = 100.0f;
    static constexpr float musouThreshold = 80.0f;
    static constexpr float musouDrain = 10.0f;
    static const std::array<WeaponData, 3> weaponData;
    float health = maxHealth, blood = 0.0f;
    float moveSpeed = 1000.0f;
    float bodyRadius = 40.0f, bodyHeight = 80.0f;
    float attackHeight = 20.0f; // å≥ÇÃìG(0.1î{ÉÇÉfÉã)ÇÃçÇÇ≥Ç…çáÇÌÇπÇΩâºê›íË
    float invincibleTime = 0.0f, hurtTime = 0.0f;
    Weapon weapon = Weapon::Sword;
    std::array<float, 3> gauges = {};
    bool attacking = false, attackActive = false;
    float attackTime = 0.0f, attackDuration = 0.0f;
    float attackDamage = 0.0f, attackReach = 0.0f, attackRadius = 0.0f;
    float attackMoveRate = 1.0f;
    AttackType attackType = AttackType::Normal;
    std::unordered_set<std::uint64_t> hitEnemies;
    std::array<bool, 7> previousKeys = {};
    bool musou = false;
    float swayTime = 0.0f;
    float visualSway = 0.0f, visualPitch = 0.0f;
    float movementYaw = 0.0f;
    int previousMoveMask = 0;
    // model Ç™îjä¸Ç≥ÇÍÇÈëOÇ… animation Çîjä¸Ç∑ÇÈÅB
    std::unique_ptr<PlayerAnimation> animation;
    AnimationClips clips;
    State state = State::Idle;
    int currentClip = -1;
};

