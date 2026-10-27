#include "Player.h"
#include "GameplayMath.h"
#include <Windows.h>
#include <imgui.h>
#include <algorithm>
#include <cctype>
#include <initializer_list>
#include <string>

using namespace GameplayMath;

const std::array<Player::WeaponData, 3> Player::weaponData = { {
    { "Sword", 20.0f, 140.0f, 100.0f, 0.50f, 10.0f },
    { "Fist", 12.0f, 80.0f, 65.0f, 0.28f, 6.0f },
    { "Scythe", 30.0f, 180.0f, 160.0f, 0.80f, 15.0f }
} };
namespace
{
    bool ValidWeapon(Player::Weapon w)
    {
        return w == Player::Weapon::Sword || w == Player::Weapon::Fist || w == Player::Weapon::Scythe;
    }
    std::string LowerName(const std::string& source)
    {
        std::string result;
        for (unsigned char c : source)
            if (std::isalnum(c)) result += static_cast<char>(std::tolower(c));
        return result;
    }
}

Player::Player() { Initialize(); }

void Player::Initialize()
{
    animation.reset();
    health = maxHealth;
    blood = 0.0f;
    gauges.fill(0.0f);
    weapon = Weapon::Sword;
    invincibleTime = hurtTime = 0.0f;
    attacking = attackActive = musou = false;
    attackTime = attackDuration = attackDamage = attackReach = attackRadius = 0.0f;
    attackMoveRate = 1.0f;
    swayTime = visualSway = visualPitch = movementYaw = 0.0f;
    previousMoveMask = 0;
    hitEnemies.clear();
    previousKeys.fill(false);
    state = State::Idle;
    currentClip = -1;
    player.position = { 13, 0, 16 };
    player.angle = { 0, 0, 0 };
    player.scale = { 1, 1, 1 };
    player.velocity = { 0, 0, 0 };
    player.model = std::make_unique<Model>("Data/Model/Player/player.fbx");
    animation = std::make_unique<PlayerAnimation>(*player.model);
    FindAnimationClips();
    UpdateAnimation(0.0f, false);
    UpdateVisualTransform();
}

float Player::GetWeaponGauge(Weapon target) const
{
    return ValidWeapon(target) ? gauges[static_cast<std::size_t>(target)] : 0.0f;
}
const Player::WeaponData& Player::GetWeaponData() const
{
    return weaponData[static_cast<std::size_t>(weapon)];
}

bool Player::SetWeapon(Weapon next)
{
    if (IsDead() || attacking || attackActive || hurtTime > 0.0f || !ValidWeapon(next)) return false;
    weapon = next;
    return true;
}

void Player::TakeDamage(float damage)
{
    if (!std::isfinite(damage) || damage <= 0.0f || IsDead() || invincibleTime > 0.0f) return;
    health = Clamp(health - damage, 0.0f, maxHealth);
    invincibleTime = 0.5f;
    hurtTime = 0.25f;
    attacking = attackActive = false;
    attackDamage = 0.0f;
    if (IsDead()) musou = false;
    UpdateAnimation(0.0f, false);
}
void Player::Heal(float amount)
{
    if (std::isfinite(amount) && amount > 0.0f && !IsDead()) health = Clamp(health + amount, 0.0f, maxHealth);
}
void Player::AddBlood(float amount)
{
    if (std::isfinite(amount) && amount > 0.0f && !IsDead()) blood = Clamp(blood + amount, 0.0f, maxBlood);
}
void Player::OnEnemyDefeated(float amount) { AddBlood(amount); }

bool Player::TryStartMusou()
{
    if (!CanStartMusou()) return false;
    musou = true;
    swayTime = 0.0f;
    return true;
}

bool Player::TryAttack(AttackType type)
{
    if (IsDead() || attacking || attackActive || hurtTime > 0.0f) return false;
    if (type != AttackType::Normal && type != AttackType::Blood && type != AttackType::Special) return false;
    const auto& data = GetWeaponData();
    float& gauge = gauges[static_cast<std::size_t>(weapon)];
    if (type == AttackType::Blood && blood < data.bloodCost) return false;
    if (type == AttackType::Special && gauge < maxGauge) return false;
    // 蓄積量だけでは強化しない。開始時の無双状態で一撃の性能を確定。
    attackDamage = data.damage * (musou ? 2.0f : 1.0f);
    attackReach = data.reach;
    attackRadius = data.radius * (musou ? 1.25f : 1.0f);
    attackDuration = data.duration / (musou ? 1.5f : 1.0f);
    attackMoveRate = weapon == Weapon::Fist ? 0.8f : 0.45f;
    if (type == AttackType::Blood)
    {
        blood = Clamp(blood - data.bloodCost, 0.0f, maxBlood);
        attackDamage *= 2.0f;
        attackRadius *= 1.3f;
        if (blood <= 0.0f) musou = false;
    }
    else if (type == AttackType::Special)
    {
        gauge = 0.0f;
        attackDamage *= 4.0f;
        attackRadius *= 2.0f;
        attackDuration *= 1.5f;
    }
    attackType = type;
    attackTime = 0.0f;
    attacking = true;
    attackActive = false;
    hitEnemies.clear();
    state = State::Attack;
    currentClip = SelectClip(state);
    if (ValidClip(currentClip)) animation->PlayAnimation(currentClip, false);
    return true;
}

void Player::Update(float elapsedTime, Object& stage, const Camera& camera)
{
    (void)stage;
    attackActive = false;
    if (!std::isfinite(elapsedTime) || elapsedTime <= 0.0f) return;
    invincibleTime = Clamp(invincibleTime - elapsedTime, 0.0f, 0.5f);
    hurtTime = Clamp(hurtTime - elapsedTime, 0.0f, 0.25f);
    if (musou)
    {
        blood = Clamp(blood - musouDrain * elapsedTime, 0.0f, maxBlood);
        if (blood <= 0.0f || IsDead()) musou = false;
    }
    if (musou) swayTime = std::fmod(swayTime + elapsedTime, 1000.0f);
    const float targetRoll = musou ? std::sin(swayTime * 3.1f) * 0.14f : 0.0f;
    const float targetPitch = musou ? std::sin(swayTime * 2.3f) * 0.045f : 0.0f;
    visualSway += (targetRoll - visualSway) * FollowRate(10.0f, elapsedTime);
    visualPitch += (targetPitch - visualPitch) * FollowRate(10.0f, elapsedTime);

    const int keys[] = { '1', '2', '3', 'J', 'K', 'L', 'R' };
    bool pressed[7] = {};
    const bool capture = ImGui::GetCurrentContext() && ImGui::GetIO().WantCaptureKeyboard;
    for (std::size_t i = 0; i < previousKeys.size(); ++i)
    {
        const bool down = (GetAsyncKeyState(keys[i]) & 0x8000) != 0;
        pressed[i] = down && !previousKeys[i] && !capture;
        previousKeys[i] = down;
    }
    if (IsDead())
    {
        previousMoveMask = 0;
        UpdateAnimation(elapsedTime, false);
        UpdateVisualTransform();
        return;
    }
    if (pressed[0]) SetWeapon(Weapon::Sword);
    else if (pressed[1]) SetWeapon(Weapon::Fist);
    else if (pressed[2]) SetWeapon(Weapon::Scythe);
    if (pressed[6]) TryStartMusou();
    if (pressed[5]) TryAttack(AttackType::Special);
    else if (pressed[4]) TryAttack(AttackType::Blood);
    else if (pressed[3]) TryAttack(AttackType::Normal);

    const bool movingAttack = attacking;
    if (attacking)
    {
        const float before = attackTime;
        attackTime += elapsedTime;
        attackActive = before < attackDuration * 0.65f && attackTime >= attackDuration * 0.2f;
        if (attackTime >= attackDuration) attacking = false;
    }
    float axisX = 0.0f, axisY = 0.0f;
    int moveMask = 0;
    if (!capture && hurtTime <= 0.0f)
    {
        if (GetAsyncKeyState('W') & 0x8000) { axisY += 1; moveMask |= 1; }
        if (GetAsyncKeyState('S') & 0x8000) { axisY -= 1; moveMask |= 2; }
        if (GetAsyncKeyState('D') & 0x8000) { axisX += 1; moveMask |= 4; }
        if (GetAsyncKeyState('A') & 0x8000) { axisX -= 1; moveMask |= 8; }
    }
    const bool moved = axisX != 0.0f || axisY != 0.0f;
    if (moved)
    {
        // 同じキーを押している間は移動基準を固定する。
        // 背後へ回るカメラに移動が引かれて、意図せず円を描くのを防ぐ。
        if (moveMask != previousMoveMask)
        {
            const auto& front = camera.GetFront();
            if (front.x * front.x + front.z * front.z > 0.00001f)
                movementYaw = std::atan2(front.x, front.z);
            else movementYaw = player.angle.y;
        }
        const float inputYaw = movementYaw + std::atan2(axisX, axisY);
        // 蛇行は移動方向だけに加算。論理的な向きとカメラには揺れを渡さない。
        const float stumble = musou
            ? std::sin(swayTime * 3.0f) * 0.18f + std::sin(swayTime * 5.3f) * 0.07f : 0.0f;
        const float speed = moveSpeed * (musou ? 1.2f : 1.0f)
            * (movingAttack ? attackMoveRate : 1.0f);
        player.position.x += std::sin(inputYaw + stumble) * speed * elapsedTime;
        player.position.z += std::cos(inputYaw + stumble) * speed * elapsedTime;
        if (!movingAttack) player.angle.y = WrapAngle(inputYaw);
    }
    previousMoveMask = moved ? moveMask : 0;
    UpdateAnimation(elapsedTime, moved);
    UpdateVisualTransform();
}

DirectX::XMFLOAT3 Player::GetHitCenter() const
{
    return { player.position.x, player.position.y + bodyHeight, player.position.z };
}

bool Player::TryHitEnemy(std::uint64_t id, const DirectX::XMFLOAT3& center, float radius)
{
    if (IsDead() || !attackActive || radius < 0.0f || hitEnemies.count(id)) return false;
    // 前方へ伸びるカプセル判定。懐に入った敵にも命中させる。
    const float fx = std::sin(player.angle.y), fz = std::cos(player.angle.y);
    const float dx = center.x - player.position.x, dz = center.z - player.position.z;
    const float projection = Clamp(dx * fx + dz * fz, 0.0f, attackReach);
    const float x = player.position.x + fx * projection;
    const float y = player.position.y + attackHeight;
    const float z = player.position.z + fz * projection;
    if (!SphereHit(center.x - x, center.y - y, center.z - z, attackRadius + radius)) return false;
    hitEnemies.insert(id);
    if (attackType != AttackType::Special)
    {
        auto& gauge = gauges[static_cast<std::size_t>(weapon)];
        gauge = Clamp(gauge + 8.0f, 0.0f, maxGauge);
    }
    return true;
}

bool Player::ValidClip(int index) const
{
    if (!player.model || index < 0) return false;
    const auto& list = player.model->GetResource()->GetAnimations();
    return static_cast<std::size_t>(index) < list.size() && !list[index].keyframes.empty();
}

void Player::FindAnimationClips()
{
    clips = AnimationClips{};
    const auto& list = player.model->GetResource()->GetAnimations();
    auto find = [&](std::initializer_list<const char*> candidates)
        {
            for (const char* word : candidates)
                for (std::size_t i = 0; i < list.size(); ++i)
                    if (ValidClip(static_cast<int>(i)) && LowerName(list[i].name).find(word) != std::string::npos)
                        return static_cast<int>(i);
            return -1;
        };
    clips.idle = find({ "idle", "standing", "breathing" });
    clips.run = find({ "running", "run", "jog", "walk" });
    clips.damage = find({ "hitreaction", "gethit", "takinghit", "damage", "hurt" });
    clips.death = find({ "death", "dying", "die" });
    clips.weaponIdle[0] = find({ "swordidle", "swordandshieldidle" });
    clips.weaponIdle[1] = find({ "boxingidle", "fistidle", "fightingidle" });
    clips.weaponIdle[2] = find({ "scytheidle" });
    const int genericAttack = find({ "attack", "slash", "punch", "kick" });
    clips.normal[0] = find({ "swordattack", "swordslash", "swordandshieldslash", "slash" });
    clips.normal[1] = find({ "fistattack", "punch", "boxing", "kick" });
    clips.normal[2] = find({ "scytheattack", "scytheslash", "reap" });
    for (std::size_t w = 0; w < 3; ++w)
        if (clips.normal[w] < 0) clips.normal[w] = genericAttack;
    clips.blood[0] = find({ "swordblood", "swordheavy" });
    clips.blood[1] = find({ "fistblood", "uppercut" });
    clips.blood[2] = find({ "scytheblood", "scytheheavy" });
    clips.special[0] = find({ "swordspecial", "swordspin" });
    clips.special[1] = find({ "fistspecial", "combopunch" });
    clips.special[2] = find({ "scythespecial", "scythespin" });
}

void Player::SetAnimationClips(const AnimationClips& value)
{
    clips = value;
    currentClip = -1;
}

int Player::SelectClip(State next) const
{
    const std::size_t w = static_cast<std::size_t>(weapon);
    switch (next)
    {
    case State::Dead: return clips.death;
    case State::Damage: return clips.damage;
    case State::Attack:
        if (attackType == AttackType::Special && ValidClip(clips.special[w])) return clips.special[w];
        if (attackType == AttackType::Blood && ValidClip(clips.blood[w])) return clips.blood[w];
        return clips.normal[w];
    case State::Run: if (ValidClip(clips.run)) return clips.run; break;
    default: break;
    }
    return ValidClip(clips.weaponIdle[w]) ? clips.weaponIdle[w] : clips.idle;
}

void Player::UpdateAnimation(float elapsedTime, bool moved)
{
    if (!animation) return;
    const State next = IsDead() ? State::Dead : (hurtTime > 0.0f ? State::Damage
        : ((attacking || attackActive) ? State::Attack : (moved ? State::Run : State::Idle)));
    const int index = SelectClip(next);
    const bool loop = next == State::Idle || next == State::Run;
    if (next != state || index != currentClip)
    {
        state = next;
        currentClip = index;
        if (ValidClip(index)) animation->PlayAnimation(index, loop);
        else animation->Stop();
    }
    float rate = 1.0f;
    if (next == State::Attack && attackDuration > 0.0f && ValidClip(index))
        rate = animation->GetDuration(index) / attackDuration;
    else if (next == State::Damage && ValidClip(index))
        rate = animation->GetDuration(index) / 0.25f;
    else if (next == State::Run && musou) rate = 1.2f;
    animation->Update(elapsedTime * rate);
}

void Player::UpdateVisualTransform()
{
    // ふらつく姿勢は描画行列だけに反映。GetAngle() は安定した向きを返す。
    using namespace DirectX;
    float pitch = player.angle.x + visualPitch;
    float roll = player.angle.z + visualSway;
    float yaw = player.angle.y;
    // 攻撃モーションがない場合も最低限の振りを表示。ボーンの代用品に限る。
    if ((attacking || attackActive) && !ValidClip(SelectClip(State::Attack)) && attackDuration > 0.0f)
    {
        const float t = Clamp(attackTime / attackDuration, 0.0f, 1.0f);
        yaw += std::sin(t * Pi) * (weapon == Weapon::Fist ? 0.2f : 0.65f);
        pitch += std::sin(t * Pi) * 0.08f;
    }
    const XMMATRIX s = XMMatrixScaling(player.scale.x, player.scale.y, player.scale.z);
    const XMMATRIX r = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);
    const XMMATRIX t = XMMatrixTranslation(player.position.x, player.position.y, player.position.z);
    XMStoreFloat4x4(&player.transform, s * r * t);
}

void Player::Render(float elapsedTime, RenderContext rc, bool pause)
{
    (void)elapsedTime;
    (void)pause;
    if (player.model) Graphics::Instance().GetModelRenderer()->Render(
        rc, player.transform, player.model.get(), ShaderId::Lambert);
}

void Player::DrawGUI()
{
    if (!ImGui::CollapsingHeader("Player", ImGuiTreeNodeFlags_DefaultOpen)) return;
    ImGui::DragFloat3("Position", &player.position.x, 0.1f);
    ImGui::DragFloat3("Angle", &player.angle.x, 0.01f);
    ImGui::DragFloat3("Scale", &player.scale.x, 0.01f);
    ImGui::DragFloat("Move speed", &moveSpeed, 10.0f, 0.0f, 5000.0f);
    ImGui::DragFloat("Player hit radius", &bodyRadius, 1.0f, 1.0f, 200.0f);
    ImGui::DragFloat("Player hit height", &bodyHeight, 1.0f, 0.0f, 300.0f);
    ImGui::DragFloat("Attack height", &attackHeight, 1.0f, 0.0f, 300.0f);
    ImGui::Text("HP %.0f / 100", health); ImGui::ProgressBar(health / maxHealth);
    ImGui::Text("Blood %.0f / 100", blood); ImGui::ProgressBar(blood / maxBlood);
    ImGui::Text("Musou: %s", musou ? "ACTIVE (-10/sec)" : (CanStartMusou() ? "READY [R]" : "Need 80 blood"));
    ImGui::Text("Weapon: %s", GetWeaponData().name);
    for (std::size_t w = 0; w < gauges.size(); ++w)
    {
        ImGui::Text("%s gauge", weaponData[w].name);
        ImGui::ProgressBar(gauges[w] / maxGauge);
    }
    ImGui::Text("WASD Move / 1-3 Weapon / J Attack / K Blood / L Special / R Musou");
    if (IsDead()) ImGui::Text("DEAD");
    if (ImGui::Button("Debug: Blood +15")) AddBlood(15.0f);
    if (ImGui::Button("Debug: Damage 10")) TakeDamage(10.0f);
    if (ImGui::Button("Debug: Fill current gauge")) gauges[static_cast<std::size_t>(weapon)] = maxGauge;
    if (ImGui::Button("Debug: Respawn")) Initialize();
    if (ImGui::TreeNode("Animation mapping"))
    {
        const auto& list = player.model->GetResource()->GetAnimations();
        auto select = [&](const char* label, int& index)
            {
                const char* preview = ValidClip(index) ? list[index].name.c_str() : "(none)";
                if (ImGui::BeginCombo(label, preview))
                {
                    if (ImGui::Selectable("(none)", index < 0)) index = -1;
                    for (std::size_t i = 0; i < list.size(); ++i)
                    {
                        ImGui::PushID(static_cast<int>(i));
                        if (ImGui::Selectable(list[i].name.c_str(), index == static_cast<int>(i))) index = static_cast<int>(i);
                        ImGui::PopID();
                    }
                    ImGui::EndCombo();
                }
            };
        select("Idle", clips.idle); select("Run", clips.run);
        select("Damage", clips.damage); select("Death", clips.death);
        for (std::size_t w = 0; w < 3; ++w)
        {
            ImGui::PushID(static_cast<int>(w));
            ImGui::Text("%s", weaponData[w].name);
            select("Weapon idle", clips.weaponIdle[w]);
            select("Normal", clips.normal[w]); select("Blood", clips.blood[w]); select("Special", clips.special[w]);
            ImGui::PopID();
        }
        if (list.empty()) ImGui::Text("No animation clips in this model.");
        ImGui::Text("Runtime selections reset on Initialize(). Save indexes in code if needed.");
        ImGui::TreePop();
    }
}
