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
#include <random>

#define PLAYER_HP_MAX 10000//
#define PLAYER_ATTACK_DAMAGE 34//初期値
#define PLAYER_ATTACK_RADIUS 10//初期値
#define PLAYER_ATTACK_DISTANCE 10//初期値
#define BLOOD_GAUGE_MAX 100

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
    //撃破時の血液獲得
    void AddRandomBlood();

    bool GetBlood() { return Blood_Attack; }

    float GetBloddCosts(int weapon) { return bloodCosts[weapon]; }



    // プレイヤーカラー描画
    void RenderColorPass(RenderContext& rc);

    // プレイヤー速度描画
    void RenderVelocityPass(
        const RenderContext& rc,
        const DirectX::XMFLOAT4X4& previousViewProjection);

    const DirectX::XMFLOAT4X4& GetTransform() const{ return player.transform;}

    const Model* GetModel() const{ return player.model.get();}

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
    void StartAttackMotion();//
   
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

    //回避
    void StartDodge();
    void UpdateDodge(float elapsedTime);

    bool isDodging = false;
    bool wasDodgePressed = false;

    float dodgeTime = 0.0f;
    float dodgeDuration = 0.45f;        // 回避にかかる時間
    float dodgeDistance = 50.0f;       // 回避する距離
    float dodgeInvincibleTime = 0.25f;

    // 回避開始時の方向を保存する
    DirectX::XMFLOAT3 dodgeDirection = { 0, 0, 1 };

private:
    //アニメーションstate
    enum class State
    {
        Idle=0,
        Death,
        Drunk_Idle,//Drunk=酔っ払い
        Drunk_Scyth,
        Drunk_Sword,
        Fight_Idle,
        Fight_Punch,
        Fight_Punch2,
        Fight_kick,
        Scythe_Attack,
        Sword_Attack1,
        Sword_Attack2,
        Sword_Walk,
        Walking,
        Dodge,
        Run
        
    };
    void setState(State next, bool loop);
    static const char* const animationNames[16];
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
    float bloodCosts[3] = { 10.0f,15.0f,5.0f };
    //無双モードでの血液消費量
    float bloodGaugeConsumption[3] = { 2.5f,3.3f,2.0f };// { 10.0f,12.0f,2.5f };

    //攻撃判定一つの設定
    struct HitSetting
    {
        float start = 0.15f;
        float end = 0.3f;
        float radius = 10.0f;
        float distance = 10.0f;
        float damage = PLAYER_ATTACK_DAMAGE;

        float sideOffset = 0.0f;//正なら右、負なら左
    };

    //1つのアニメーションに、複数の攻撃判定を含めるように
    struct AttackMotion
    {
        State motion = State::Idle;
        float duration = 1.0f;
        float animationSpeed = 1.0f;
        int hitCount = 1;//攻撃判定を出す回数
        HitSetting hits[10]{};
    };

    //攻撃時、アニメーションが順番に再生される
    struct AttackSetting
    {
        int motionCount = 1;//流すアニメーションの数
        AttackMotion motions[5]{};
    };

    //[武器][種類]
    //武器:0=剣、1=鎌、2=拳
    //攻撃種類:0=通常、1=血攻撃、2=無双攻撃
    AttackSetting attackSetting[3][3] =
    {
        //剣
        {
            //通常攻撃
            {1,/*アニメーション数 */
                {
                    {
                        State::Sword_Attack1,2.2f,1.0f,9,/*motion,duration,motionSpeed,判定回数*/
                        {
                            {0.35f, 0.45f, 10.0f, 8.0f, 20.0f,15.0f},/*start,end,radius,distance,damage,sideOffset*/
                            {0.45f, 0.55f, 12.0f, 10.0f, 20.0f},
                            {0.55f, 0.65f, 10.0f, 8.0f, 20.0f,-15.0f},

                            {0.95f, 1.05f, 10.0f, 8.0f, 20.0f,-15.0f},
                            {1.05f, 1.15f, 12.0f, 10.0f, 20.0f},
                            {1.15f, 1.25f, 10.0f, 8.0f, 20.0f,15.0f},

                            {1.55f, 1.65f, 10.0f, 8.0f, 20.0f,15.0f},
                            {1.65f, 1.75f, 12.0f, 10.0f, 20.0f},
                            {1.75f, 1.85f, 10.0f, 8.0f, 20.0f,-15.0f}
                        }
                    }
                }
            },
            //血液攻撃
            {1, 
                {
                    {
                        State::Sword_Attack1, 2.2f, 1.3f, 9,
                        {
                            {0.35f, 0.45f, 18.0f, 8.0f, 35.0f,20.0f},
                            {0.45f, 0.55f, 20.0f, 10.0f, 35.0f},
                            {0.55f, 0.65f, 18.0f, 8.0f, 35.0f,-20.0f},

                            {0.95f, 1.05f, 18.0f, 8.0f, 35.0f,-20.0f},
                            {1.05f, 1.15f, 20.0f, 10.0f, 35.0f},
                            {1.15f, 1.25f, 18.0f, 8.0f, 35.0f,20.0f},

                            {1.55f, 1.65f, 18.0f, 8.0f, 35.0f,20.0f},
                            {1.65f, 1.75f, 20.0f, 10.0f, 35.0f},
                            {1.75f, 1.85f, 18.0f, 8.0f, 35.0f,-20.0f}
                        }
                    }
                }
            },
            //無双攻撃
            {1, 
                {
                    {
                        State::Drunk_Sword, 3.1f, 1.0f, 3,
                        {
                            {0.3f, 0.8f, 30.0f, 15.0f, 45.0f},
                            {1.0f, 1.5f, 34.0f, 15.0f, 50.0f},
                            {1.8f, 2.5f, 35.0f, 15.0f, 50.0f}
                        }
                    }
                }
            },
        },

        //鎌
        {
            //通常
            {1, 
                {
                    {
                        State::Scythe_Attack, 1.0f, 1.0f, 1,
                        {
                            {0.3f, 0.7f, 20.0f, 5.0f, 30.0f}
                        }
                    }
                }
            },
            //血液
            {1,
                {
                    {
                        State::Scythe_Attack, 1.0f, 1.2f, 1,
                        {
                            {0.4f, 0.7f, 40.0f, 0.0f, 50.0f}
                        }
                    }
                }
            },
            //無双
            {1,
                {
                    {
                        State::Drunk_Scyth, 1.0f, 1.0f, 5,
                        {
                            {0.2f, 0.3f, 15.0f, 0.0f, 20.0f},
                            {0.3f, 0.4f, 25.0f, 0.0f, 20.0f},
                            {0.4f, 0.5f, 35.0f, 0.0f, 20.0f},
                            {0.5f, 0.6f, 45.0f, 0.0f, 20.0f},
                            {0.6f, 0.7f, 55.0f, 0.0f, 20.0f},
                        }
                    }
                }
            },
        },

        //拳
        {
            //通常
            {1, 
                {
                    {
                        State::Fight_Punch, 0.8f, 1.0f, 1,
                        {
                            {0.35f, 0.45f, 15.0f, 10.0f, 35.0f}
                        }
                    }
                }  
            },
            //血液
            {1, 
                {
                    {
                        State::Fight_Punch2, 0.8f, 1.5f, 2,
                        {
                            {0.35f, 0.4f, 15.0f, 5.0f, 30.0f},
                            {0.4f, 0.7f, 15.0f, 20.0f, 35.0f}
                        }
                    }
                }
            },
            //無双
            {2,
                {
                    {
                        State::Fight_Punch,0.85f,1.0f,1,
                        {
                            {0.35f,0.5f,15.0f,10.0f,40.0f}
                        }
                    },
                    {
                        State::Fight_kick,1.2f,1.0f,3,
                        {
                            {0.125f,0.25f,15.0f,10.0f,20.0f},
                            {0.2f,0.35f,15.0f,20.0f,20.0f},
                            {0.3f,0.45f,20.0f,35.0f,80.0f}
                        }
                    },
                },            
            }
        },
    };
    //今回の一撃に使う設定
    AttackSetting currentAttackSetting{};
    int currentMotionIndex = 0;
    int currentHitIndex = -1;
    //float attackDamageMultiplier = 1.0f;
    float attackModeSpeedMultiplier = 1.0f;
    //血液攻撃倍率
    //float bloodMultipliers[3] = { 1.8f,2.0f,5.0f };
    //無双モード
    bool IsMusou = false;
    //攻撃倍率
    // float bloodAttackMultiplier = 1.0f;
    //攻撃の再生倍率
    float attackSpeedMultiplier = 1.0f;
    
    bool Blood_Attack = false;

    //血ゲージ回復量
    int bloodRecoveryMin = 5;//最小
    int bloodRecoveryMax = 15;//最大

    //UI
    std::unique_ptr<Sprite> spriteGauge = nullptr;
    std::unique_ptr<Sprite> spriteHPGauge = nullptr;
    std::unique_ptr<Sprite> spriteHP = nullptr;
    std::unique_ptr<Sprite> spriteWP = nullptr;
    std::unique_ptr<Sprite> spriteW[3];
    std::unique_ptr<Sprite> spriteHITCOUNT = nullptr;
	 std::unique_ptr<Sprite> spriteHIT = nullptr;
	

    //debug
    bool showAttackCollision = true;
    bool previewState = false;


	Sword_Line sword_Line;
 

    //エフェクト用
    Effect* Sword_Effect = nullptr;
    Effect* Fight_Effect = nullptr;
    Effect* Slash_Effect = nullptr;
    Effect* Drunk_Sword = nullptr;

    bool IsSwrod;
    bool IsFight;
    bool IsSlash;

    bool Blad = false;


    //残像用
   

    //
    float niseHP = PLAYER_HP_MAX;
    float nisehptime = 1.0f;

    int hit = 0;
    int nisehit = 0;
    float hittime = 5.0f;
};
