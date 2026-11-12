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
#define PLAYER_ATTACK_DAMAGE 34//‰Šú’l
#define PLAYER_ATTACK_RADIUS 10//‰Šú’l
#define PLAYER_ATTACK_DISTANCE 10//‰Šú’l

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

    //UŒ‚‚ğ—^‚¦‚Ä‚¢‚é‚©
    bool CheckAttackHit(
        size_t enemyIndex,
        DirectX::XMFLOAT3 enemyPosition,
        float enemyRadius);

    //UŒ‚‚ğó‚¯‚é
    void takeDamage(float damage);

    //UŒ‚‚ğó‚¯‚Ä‚¢‚é‚©
    bool CheckTakeAttack(DirectX::XMFLOAT3 enemyPosition,
        float enemyRadius);

    //•ŠíØ‚è‘Ö‚¦
    void WeaponChange();

    //ŒŒ‰tŠl“¾
    void AddBlood(float amount);

    //ŒŒ‰tUŒ‚
    bool BloodAttack();

protected:
    //ƒvƒŒƒCƒ„[
    Object player;
    Object Weapon[2];

    float playerHP = PLAYER_HP_MAX;
    float playerMoveSpeed = 40.0f;
    float playerRadius = 5.0f;
    float playerRadiusHeight = 15.0f;

    float PAD = PLAYER_ATTACK_DAMAGE;//GUI‚Å’l‚ğ•ÏX‚·‚é—p_DEBUG
    float pAtkDmg = PLAYER_ATTACK_DAMAGE;
    bool IsDead = false;
    float invincibleTime = 0.5f;

    //
    float enemyHP = 100.0f;
    //

    //UŒ‚
    //‰¼’l
    float attackStartTime = 0.4f;
    float attackEndTime = 1.0f;

    DirectX::XMFLOAT3 attackPosition = { 0,0,0 };//UŒ‚”»’è‚Ì’†S
    float attackRadius = PLAYER_ATTACK_RADIUS;//‹…‚Ì”¼Œa
    //•Ší‚ÌŒŒ‰tUŒ‚”ÍˆÍ
    float WeaponAtkRadius[3] = { 15.0f,35.0f,10.0f };

    float attackDistance = PLAYER_ATTACK_DISTANCE;//ƒvƒŒƒCƒ„[‚©‚ç‘O•û‚Ö‚Ì‹——£
    //ŒŒ‰tUŒ‚‹——£
    float bloodAtkDistance[3] = { 20.0f,0.0f,10.0f };

    float attackHeight = 15.0f;//’n–Ê‚©‚ç‚Ì‚‚³
    float attackTime = 0;//UŒ‚‚ğn‚ß‚Ä‚©‚ç‚ÌŒo‰ßŠÔ
    float attackDuration = 2.4f;//UŒ‚‚ªI‚í‚é‚Ü‚Å‚ÌŠÔ
    bool isAttacking = false;

    //ŒŒ‰tUŒ‚ƒRƒXƒg
    float BloodCosts[3] = { 5.0f,10.0f,2.5f};
    //ŒŒ‰tUŒ‚”{—¦
    float BloodMultipliers[3] = { 1.8f,2.0f,5.0f };
    //–³‘oƒ‚[ƒh‚Å‚ÌŒŒ‰tÁ”ï—Ê
    float BloodGaugeConsumption[3] = { 10.0f,12.0f,2.5f };

    //UŒ‚‚µ‚Ä‚¢‚é‚©
    bool IsAttackActive()const;

    //“G
    std::unordered_set<size_t> hitEnemies;
    
    void StartAttack();//UŒ‚ŠJnˆ—
    void UpdateAttack(float elapsedTime);
   
    //ŒŒƒQ[ƒW‚Ì”Ô†‚¾‚¯‚ğ‹‚ß‚éŠÖ”
    int GetBloodGaugeIndex() const;
    //–³‘oó‘Ô‚Ö
    void ChangeMusouMode();
    void UpdateMusouMode(float elapsedTime);
    //“ü—ÍAˆÚ“®ˆ—(ƒJƒƒ‰ŠÜ‚Ş
    bool InputMove(float elapsedTime,Camera camera);
    bool wasMoving = false;
    DirectX::XMFLOAT3 moveFront = { 0,0,1 };
    DirectX::XMFLOAT3 moveRight = { 1,0,0 };

    std::unique_ptr<Animation> animation;
    void pAnimation(float elapsedTime,bool isMoving);

private:
    //ƒAƒjƒ[ƒVƒ‡ƒ“state
    enum class State
    {
        Idle,
        Death,
        Drunk_Idle,//Drunk=Œ‚Á•¥‚¢
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
    WEAPON weapon = WEAPON::Sword;//Å‰‚ÍŒ•
    int weapon_num = 0;
    float bloodGauge[3] = {};//0:Sword,1:Scythe,2:Fist
    //–³‘oƒ‚[ƒh
    bool IsMusou = false;
    //UŒ‚”{—¦
    float bloodAttackMultiplier = 1.0f;
    //UŒ‚‚ÌÄ¶”{—¦
    float attackSpeedMultiplier = 1.0f;
    //duration
    float WeaponDuration[3] = { 2.4f,1.1f,0.87f };

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
 

    //ƒGƒtƒFƒNƒg—p
    Effect* Sword_Effect = nullptr;
    Effect* Fight_Effect = nullptr;
    Effect* Slash_Effect = nullptr;

    bool IsSwrod;
    bool IsFight;
    bool IsSlash;

    bool Blad = false;

    //
    float niseHP = 10000.0f;
    float nisehptime = 1.0f;

};
