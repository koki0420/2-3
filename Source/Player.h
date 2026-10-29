#pragma once

#include <array>
#include <cstdint>
#include <memory>
#include <unordered_set>
#include "Graphics.h"
#include "Camera.h"
#include "Object.h"
#include "Animation.h"

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

    float playerHP = 10000.0f;
    float playerRadius = 5.0f;
    float pAtkDmg = 200.0f;
    bool IsDead = false;
    float invincibleTime = 0.5f;

    //
    float enemyHP = 100.0f;
    //

    //莉ｮ縺ｮ蛟､
    float attackStartTime = 0.4f;
    float attackEndTime = 1.0f;

    //迴ｾ蝨ｨ謾ｻ謦・愛螳壹′譛牙柑縺・
    bool IsAttackActive()const;

    std::unordered_set<size_t> hitEnemies;

    DirectX::XMFLOAT3 attackPosition = { 0,0,0 };//謾ｻ謦・愛螳壹・荳ｭ蠢・
    float attackRadius = 2.0f;//逅・・蜊雁ｾ・
    float attackDistance = 2.5f;//繝励Ξ繧､繝､繝ｼ縺九ｉ蜑肴婿縺ｸ縺ｮ霍晞屬
    float attackHeight = 3.0f;//蝨ｰ髱｢縺九ｉ縺ｮ鬮倥＆

    float attackTime=0;//謾ｻ謦・ｒ蟋九ａ縺ｦ縺九ｉ縺ｮ邨碁℃譎る俣
    float attackDuration=2.4f;//謾ｻ謦・′邨ゅｏ繧九∪縺ｧ縺ｮ譎る俣
    bool isAttacking=false;
    void StartAttack();//謾ｻ謦・・髢句ｧ句・逅・
    void UpdateAttack(float elapsedTime);

    bool InputMove(float elapsedTime,Camera camera);

    Animation* animation = nullptr;
    void pAnimation(float elapsedTime,bool isMoving);

private:

<<<<<<< HEAD
    //繧｢繝九Γ繝ｼ繧ｷ繝ｧ繝ｳState
    enum class State
    {
        Idle,
        Run,
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
=======
	std::unique_ptr<Sprite> spriteGauge = nullptr;
	std::unique_ptr<Sprite> spriteHP = nullptr;
	std::unique_ptr<Sprite> spriteWP = nullptr;
	std::unique_ptr<Sprite> spriteW[3];

	int wt=0;
public:
	Player();
	~Player() {};

	void Initialize();


	// 更新処理
	void Update(float elapsedTime, Object& S, const Camera& C);
	// 描画処理
	void Render(float elapsedTime, RenderContext rc,bool Pause);
	void UIRender(float elapsedTime, ID3D11DeviceContext* dc);
>>>>>>> cbe2ca2cbb218cdbd0047b8ebc4895f2b6e16233

    //debug
    bool showAttackCollision = true;
    bool previewState = false;

};
