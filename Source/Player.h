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
    ~Player() = default;
    void Initialize();
    void Update(float elapsedTime, Object& stage, const Camera& camera);
    void Render(float elapsedTime, RenderContext rc);
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
    Object player;
    Object Weapon[2];

    float playerHP = 100.0f;
    float playerRadius = 5.0f;
    float pAtkDmg = 200.0f;
    bool IsDead = false;
    float invincibleTime = 0.5f;

    //
    float enemyHP = 100.0f;
    //

    //仮の値
    float attackStartTime = 0.4f;
    float attackEndTime = 1.0f;

    //現在攻撃判定が有効か
    bool IsAttackActive()const;

    std::unordered_set<size_t> hitEnemies;

    DirectX::XMFLOAT3 attackPosition = { 0,0,0 };//攻撃判定の中心
    float attackRadius = 2.0f;//球の半径
    float attackDistance = 2.5f;//プレイヤーから前方への距離
    float attackHeight = 3.0f;//地面からの高さ

    float attackTime=0;//攻撃を始めてからの経過時間
    float attackDuration=2.4f;//攻撃が終わるまでの時間
    bool isAttacking=false;
    void StartAttack();//攻撃の開始処理
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

    enum class WEAPON
    {
        Sword=0,
        Scythe,
        Fist,
    };
    WEAPON weapon = WEAPON::Sword;
    int weapon_num = 0;;

//	std::unique_ptr<Sprite> spriteGauge = nullptr;
////=======
//	std::unique_ptr<Sprite> spriteHPGauge = nullptr;
//	std::unique_ptr<Sprite> spriteGauge = nullptr;
////>>>>>>> 302028f4317cdc9fb9b45fcff7960d7030713289
//	std::unique_ptr<Sprite> spriteHP = nullptr;
//	std::unique_ptr<Sprite> spriteWP = nullptr;
//	std::unique_ptr<Sprite> spriteW[3];

	int wt=0;


//public:
//	Player();
//	~Player() {};
//
//	void Initialize();
//
//
//	// �X�V����
//	void Update(float elapsedTime, Object& S, const Camera& C);
//	// �`�揈��
//	void Render(float elapsedTime, RenderContext rc,bool Pause);
//	void UIRender(float elapsedTime, ID3D11DeviceContext* dc);


    //debug
    bool showAttackCollision = true;
    bool previewState = false;

};
