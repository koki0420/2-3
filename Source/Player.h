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

    bool CheckTakeAttack(size_t enemyIndex,
        DirectX::XMFLOAT3 enemyPosition,
        float enemyRadius);

    //•ŠíØ‚è‘Ö‚¦
    void WeaponChange();

    void AddBlood(float amount);

protected:
    //ƒvƒŒƒCƒ„[
    Object player;
    Object Weapon[2];

    float playerHP = 100.0f;
    float playerMoveSpeed = 10.0f;
    float playerRadius = 5.0f;
    float playerRadiusHeight = 15.0f;
    float pAtkDmg = 20.0f;
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
    float attackRadius = 10.0f;//‹…‚Ì”¼Œa
    float attackDistance = 10.0f;//ƒvƒŒƒCƒ„[‚©‚ç‘O•û‚Ö‚Ì‹——£
    float attackHeight = 15.0f;//’n–Ê‚©‚ç‚Ì‚‚³

    float attackTime = 0;//UŒ‚‚ğn‚ß‚Ä‚©‚ç‚ÌŒo‰ßŠÔ
    float attackDuration = 2.4f;//UŒ‚‚ªI‚í‚é‚Ü‚Å‚ÌŠÔ
    bool isAttacking = false;

    //UŒ‚‚µ‚Ä‚¢‚é‚©
    bool IsAttackActive()const;


    //“G
    std::unordered_set<size_t> hitEnemies;


    
    void StartAttack();//UŒ‚ŠJnˆ—
    void UpdateAttack(float elapsedTime);
    //ŒŒ‰tUŒ‚
    bool BloodAttack();
    //ŒŒƒQ[ƒW‚Ì”Ô†‚¾‚¯‚ğ‹‚ß‚éŠÖ”
    int GetBloodGaugeIndex() const;
    //–³‘oó‘Ô‚Ö
    void ChangeMusouMode();
    void UpdateMusouMode(float elapsedTime);

    bool InputMove(float elapsedTime,Camera camera);

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
    //UŒ‚‚ÌÄ¶”{—¦
    float attackSpeedMultiplier = 1.0f;

    std::unique_ptr<Sprite> spriteGauge = nullptr;
    std::unique_ptr<Sprite> spriteHPGauge = nullptr;
    std::unique_ptr<Sprite> spriteHP = nullptr;
    std::unique_ptr<Sprite> spriteWP = nullptr;
    std::unique_ptr<Sprite> spriteW[3];

	int wt=0;

    //debug
    bool showAttackCollision = true;
    bool previewState = false;


	Sword_Line sword_Line;
 

};
