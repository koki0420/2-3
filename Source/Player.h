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

#define PLAYER_HP_MAX 10000//
#define PLAYER_ATTACK_DAMAGE 34//初期値
#define PLAYER_ATTACK_RADIUS 10//初期値
#define PLAYER_ATTACK_DISTANCE 10//初期値

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

    //攻撃を与えているか
    bool CheckAttackHit(
        size_t enemyIndex,
        DirectX::XMFLOAT3 enemyPosition,
        float enemyRadius);

    //攻撃を受ける
    void takeDamage(float damage);

    //攻撃を受けているか
    bool CheckTakeAttack(DirectX::XMFLOAT3 enemyPosition,
        float enemyRadius);

    //武器切り替え
    void WeaponChange();

    //血液獲得
    void AddBlood(float amount);

    bool GetBlood() { return Blood_Attack; }

    float GetBloddCosts(int weapon) { return bloodCosts[weapon]; }

protected:
    //プレイヤー
    Object player;
    Object Weapon[2];

    float playerHP = PLAYER_HP_MAX;
    float playerMoveSpeed = 40.0f;
    float playerRadius = 5.0f;
    float playerRadiusHeight = 15.0f;

    float PAD = PLAYER_ATTACK_DAMAGE;//GUIで値を変更する用_DEBUG
    float pAtkDmg = PLAYER_ATTACK_DAMAGE;
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
    float attackRadius = PLAYER_ATTACK_RADIUS;//球の半径
    float attackDistance = PLAYER_ATTACK_DISTANCE;//プレイヤーから前方への距離
    float attackHeight = 15.0f;//地面からの高さ
    float attackTime = 0;//攻撃を始めてからの経過時間
    float attackDuration = 2.4f;//攻撃が終わるまでの時間
    bool isAttacking = false;


    //攻撃しているか
    bool IsAttackActive()const;

    //敵
    std::unordered_set<size_t> hitEnemies;

    //血液攻撃
    bool BloodAttack();
    
    void StartAttack();//攻撃開始処理
    void UpdateAttack(float elapsedTime);
   
    //血ゲージの番号だけを求める関数
    int GetBloodGaugeIndex() const;
    //無双状態へ
    void ChangeMusouMode();
    void UpdateMusouMode(float elapsedTime);
    //入力、移動処理(カメラ含む
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
        Fight_kick,
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
    WEAPON weapon = WEAPON::Sword;//最初は剣
    int weapon_num = 0;
    //血液ゲージ
    float bloodGauge[3] = {};//0:Sword,1:Scythe,2:Fist
    //血液攻撃コスト
    float bloodCosts[3] = { 5.0f,10.0f,2.5f };
    //無双モードでの血液消費量
    float bloodGaugeConsumption[3] = { 10.0f,12.0f,2.5f };

    struct HitTiming//攻撃判定を出すタイミング
    {
        float start;
        float end;
    };

    struct AttackSetting
    {
        int hitCount;//攻撃判定を出す回数
        HitTiming timing[5];

        float radius;//判定球の半径
        float distance;//前方への距離
        float animationSpeed;//1.0が通常の再生速度
    };

    //[武器][種類]
    //武器:0=剣、1=鎌、2=拳
    //攻撃種類:0=通常、1=血攻撃
    AttackSetting attackSetting[3][2] =
    {
        //剣
        {
            {3,{{0.3f,0.6f},{0.9f,1.2f}, {1.5f,1.9f}},
                10.0f,10.0f,1.0f},
            //3回攻撃、0.3f毎に10.0fの攻撃判定球を前方10.0fへ出す
            {3,{{0.3f,0.6f},{0.9f,1.2f}, {1.5f,1.9f}},
                15.0f,20.0f,1.3f}
        },

        //鎌
        {
            {1,{{0.3f,0.7f}},
                10.0f,10.0f,1.0f},
            {1,{{0.3f,0.7f}},
                35.0f,0.0f,1.2f}
        },

        //拳
        {
            {1,{{0.15f,0.3f}},
                10.0f,10.0f,1.0f},
            {2,{{0.15f,0.3f},{0.5f,0.7f}},
                10.0f,10.0f,1.5f}
        }
    };
    //今回の一撃に使う設定
    AttackSetting currentAttackSetting{};
    int currentHitIndex = -1;
    //血液攻撃倍率
    float bloodMultipliers[3] = { 1.8f,2.0f,5.0f };
    //duration
    float weaponDuration[3] = { 2.4f,1.1f,0.87f };
    //無双モード
    bool IsMusou = false;
    //攻撃倍率
    float bloodAttackMultiplier = 1.0f;
    //攻撃の再生倍率
    float attackSpeedMultiplier = 1.0f;
    
    bool Blood_Attack = false;

    //UI
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

    bool Blad = false;


    //残像用
   

    //
    float niseHP = 10000.0f;
    float nisehptime = 1.0f;


};
