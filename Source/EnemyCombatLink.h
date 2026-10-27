#pragma once
#include <cstdint>
#include <vector>
#include <cmath>
#include "Enemy.h"
#include "Player.h"
#include "GameplayMath.h"

// Enemy / EnemyManager を変更せず、戦闘に足りない情報を外側に保持する。
// EnemyはEnemyManagerが所有する。このクラスは削除もUpdateも行わない。
// Manager.Clear()の前には必ずこのクラスのClear()を呼ぶこと。
class EnemyCombatLink
{
public:
    void Register(Enemy& enemy)
    {
        for (const auto& record : records) if (record.enemy == &enemy) return;
        Record record;
        record.enemy = &enemy;
        record.id = nextId++;
        record.wasAlive = enemy.Alive();
        records.push_back(record);
    }
    void Clear() { records.clear(); } // 攻撃IDを使い回さないためnextIdは維持
    std::size_t GetAliveCount() const
    {
        std::size_t count = 0;
        for (const auto& record : records) if (record.enemy->Alive()) ++count;
        return count;
    }
    void Update(float dt, Player& player)
    {
        if (!std::isfinite(dt) || dt <= 0.0f) return;
        // 先にプレイヤーの攻撃を全敵へ適用する。
        for (auto& record : records)
        {
            Enemy& enemy = *record.enemy;
            if (!enemy.Alive()) { record.wasAlive = false; continue; }
            if (!record.wasAlive)
            {
                // 元のenemy_set(true)による再有効化にも対応。
                record.health = 60.0f;
                record.cooldown = record.windup = 0.0f;
                record.windingUp = false;
                record.id = nextId++;
            }
            record.wasAlive = true;
            auto center = enemy.GetPos();
            center.y += 10.0f; // 元の敵0.1倍スケールに合わせる
            if (player.TryHitEnemy(record.id, center, 10.0f))
            {
                record.health = GameplayMath::Clamp(record.health - player.GetAttackDamage(), 0.0f, 60.0f);
                if (record.health <= 0.0f)
                {
                    enemy.enemy_set(false);
                    record.wasAlive = false;
                    record.windingUp = false;
                    player.OnEnemyDefeated(15.0f);
                }
            }
        }
        if (player.IsDead()) return;
        // 敵自身の移動AIは変更せず、近接攻撃だけ外側から接続する。
        for (auto& record : records)
        {
            if (!record.enemy->Alive()) continue;
            record.cooldown = GameplayMath::Clamp(record.cooldown - dt, 0.0f, 1.2f);
            const auto pos = record.enemy->GetPos();
            const auto target = player.GetHitCenter();
            const float dx = target.x - pos.x, dz = target.z - pos.z;
            const float reach = 70.0f + player.GetHitRadius();
            const bool inRange = dx * dx + dz * dz <= reach * reach
                && std::fabs(target.y - pos.y) <= 160.0f;
            if (record.windingUp)
            {
                record.windup -= dt;
                if (record.windup <= 0.0f)
                {
                    record.windingUp = false;
                    if (inRange) player.TakeDamage(10.0f);
                }
            }
            else if (inRange && record.cooldown <= 0.0f)
            {
                record.windingUp = true;
                record.windup = 0.35f;
                record.cooldown = 1.2f;
            }
        }
    }
private:
    struct Record
    {
        Enemy* enemy = nullptr;
        std::uint64_t id = 0;
        float health = 60.0f, cooldown = 0.0f, windup = 0.0f;
        bool wasAlive = true, windingUp = false;
    };
    std::vector<Record> records;
    std::uint64_t nextId = 1;
};
