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
#include "Sword_Line.h"
#include "Effect.h"
#include "EffectManager.h"

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
    float GetPlayerMoveSpeed() { return playerMoveSpeed; }

    bool CheckAttackHit(
        size_t enemyIndex,
        DirectX::XMFLOAT3 enemyPosition,
        float enemyRadius);

    void takeDamage(float damage);

    bool CheckTakeAttack(DirectX::XMFLOAT3 enemyPosition,
        float enemyRadius);

    //武器切り替え
    void WeaponChange();

    void AddBlood(float amount);

protected:
    //プレイヤー
    Object player;
    Object Weapon[2];

    float playerHP = 10000.0f;
    float playerMoveSpeed = 20.0f;
    float playerRadius = 5.0f;
    float playerRadiusHeight = 15.0f;
    //
    float PAD = 34.0f;
    //
    float pAtkDmg = PAD;
    bool IsDead = false;
    float invincibleTime = 0.5f;

    //
    float enemyHP = 100.0f;
    //

    //攻撃
    //仮値
    float attackStartTime = 0.4f;
    float attackEndTime = 1.0f;

    DirectX::XMFLOAT3 attackPosition = { 0,0,0 };//攻撃判定の中心
    float atkR = 10.0f;
    float attackRadius = atkR;//球の半径
    float attackDistance = 10.0f;//プレイヤーから前方への距離
    float attackHeight = 15.0f;//地面からの高さ

    float attackTime = 0;//攻撃を始めてからの経過時間
    float attackDuration = 2.4f;//攻撃が終わるまでの時間
    bool isAttacking = false;

    //攻撃しているか
    bool IsAttackActive()const;


    //敵
    std::unordered_set<size_t> hitEnemies;


    
    void StartAttack();//攻撃開始処理
    void UpdateAttack(float elapsedTime);
    //血液攻撃
    bool BloodAttack();
    //血ゲージの番号だけを求める関数
    int GetBloodGaugeIndex() const;
    //無双状態へ
    void ChangeMusouMode();
    void UpdateMusouMode(float elapsedTime);

    bool InputMove(float elapsedTime,Camera camera);
    bool wasMoving = false;
    DirectX::XMFLOAT3 moveFront = { 0,0,1 };
    DirectX::XMFLOAT3 moveRight = { 1,0,0 };

    std::unique_ptr<Animation> animation;
    void pAnimation(float elapsedTime,bool isMoving);

private:
    //アニメーションstate
    enum class State
    {
        Idle,
        Death,
        Drunk_Idle,//Drunk=酔っ払い
        Drunk_Scythe,
        Drunk_Sword,
        Fight_Idle,
        Fight_Punch1,
        Fight_Punch2,
        Fight_Kick,
        Scythe_Attack,
        Sword_Attack1,
        Sword_Attack2,
        Sword_Walk,

        
    };
    void setState(State next, bool loop);
    static const char* const animationNames[13];
    State state = State::Idle;

    enum class WEAPON
    {
        Sword=0,
        Scythe,
        Fist,
    };
    WEAPON weapon = WEAPON::Sword;
    int weapon_num = 0;;
    float bloodGauge[3] = {};//0:Sword,1:Scythe,2:Fist
    bool IsMusou = false;
    float bloodAttackMultiplier = 1.0f;
    //攻撃の再生倍率
    float attackSpeedMultiplier = 1.0f;

    std::unique_ptr<Sprite> spriteGauge = nullptr;
    std::unique_ptr<Sprite> spriteHPGauge = nullptr;
    std::unique_ptr<Sprite> spriteHP = nullptr;
    std::unique_ptr<Sprite> spriteWP = nullptr;
    std::unique_ptr<Sprite> spriteW[3];

	

    //debug
    bool showAttackCollision = true;
    bool previewState = false;


	Sword_Line sword_Line;
 

    //エフェクト用
    Effect* Sword_Effect = nullptr;
    Effect* Fight_Effect = nullptr;
    Effect* Slash_Effect = nullptr;

    bool IsSwrod;
    bool IsFight;
    bool IsSlash;

};
