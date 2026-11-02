#pragma once

#include <array>
#include <cstdint>
#include <memory>
#include <unordered_set>
#include "Graphics.h"
#include "Camera.h"
#include "Object.h"
#include "Animation.h"
#include "Sprite.h"

class Player
{
public:


    Player();
    ~Player();
    void Initialize();
    void Update(float elapsedTime, Object& stage, const Camera& camera);
    void Render(float elapsedTime, RenderContext rc);
    void UIRender(float elapsedTime, ID3D11DeviceContext* dc);
    void DrawGUI();
  
    DirectX::XMFLOAT3 GetVelocity() { return player.velocity; }
    DirectX::XMFLOAT3 GetPosition() { return player.position; }
    DirectX::XMFLOAT3 GetScale() { return player.scale; }
    DirectX::XMFLOAT3 GetAngle() { return player.angle; }
    float GetPAtkDmg() { return pAtkDmg; }
    float GetPlayerHP() { return playerHP; }

    bool CheckAttackHit(
        size_t enemyIndex,
        DirectX::XMFLOAT3 enemyPosition,
        float enemyRadius);

    void takeDamage(float damage);

    bool CheckTakeAttack(size_t enemyIndex,
        DirectX::XMFLOAT3 enemyPosition,
        float enemyRadius);

protected:
    //プレイヤー
    Object player;

    float playerHP = 100.0f;
    float playerRadius = 5.0f;
    float pAtkDmg = 200.0f;
    bool IsDead = false;
    float invincibleTime = 0.5f;

    //
    float enemyHP = 100.0f;
    //

    //攻�?
    //仮の値
    float attackStartTime = 0.4f;
    float attackEndTime = 1.0f;

    DirectX::XMFLOAT3 attackPosition = { 0,0,0 };//攻�?��定�?中�?
    float attackRadius = 2.0f;//�??半�?
    float attackDistance = 2.5f;//プレイヤーから前方への距離
    float attackHeight = 3.0f;//地面からの高さ

    float attackTime = 0;//攻�?��始めてからの経過時間
    float attackDuration = 2.4f;//攻�?��終わるまでの時間
    bool isAttacking = false;

    //現在攻�?��定が有効�?
    bool IsAttackActive()const;


    //敵
    std::unordered_set<size_t> hitEnemies;


    
    void StartAttack();//攻�??開始�?�?
    void UpdateAttack(float elapsedTime);

    bool InputMove(float elapsedTime,Camera camera);

    Animation* animation = nullptr;
    void pAnimation(float elapsedTime,bool isMoving);

private:
    //アニメーションState
    enum class State
    {
        Idle,
        Run,
        Dead,
        SwordAttack1,
        SwordAttack2,
    };
    State state = State::Idle;

    enum class Weapon
    {
        Sword,
        Fist,
        Scythe,
    };
    Weapon weapon = Weapon::Sword;


//	std::unique_ptr<Sprite> spriteGauge = nullptr;

//	std::unique_ptr<Sprite> spriteHPGauge = nullptr;
//	std::unique_ptr<Sprite> spriteGauge = nullptr;

//	std::unique_ptr<Sprite> spriteHP = nullptr;
//	std::unique_ptr<Sprite> spriteWP = nullptr;
//	std::unique_ptr<Sprite> spriteW[3];

std::unique_ptr<Sprite> spriteGauge = nullptr;
std::unique_ptr<Sprite> spriteHPGauge = nullptr;
std::unique_ptr<Sprite> spriteHP = nullptr;
std::unique_ptr<Sprite> spriteWP = nullptr;
std::unique_ptr<Sprite> spriteW[3];


	int wt=0;





    //debug
    bool showAttackCollision = true;
    bool previewState = false;

};
