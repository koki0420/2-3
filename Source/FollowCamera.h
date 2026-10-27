#pragma once
#include "Camera.h"
#include "GameplayMath.h"

// 論理的なプレイヤーの向きに追従。描画用の酔いの揺れは渡さない。
class FollowCamera
{
public:
    float distance = 1000.0f;
    float focusHeight = 80.0f;
    float basePitch = 0.60f;
    float turnSpeed = 4.0f;
    float positionSpeed = 12.0f;
    float mouseSensitivity = 0.003f;
    float yawLimit = 25.0f * GameplayMath::Pi / 180.0f;
    float pitchLimit = 12.0f * GameplayMath::Pi / 180.0f;

    void Reset(Camera& camera, const DirectX::XMFLOAT3& position, float playerYaw)
    {
        yaw = GameplayMath::WrapAngle(playerYaw);
        manualYaw = manualPitch = 0.0f;
        focus = { position.x, position.y + focusHeight, position.z };
        Apply(camera);
    }
    void Update(Camera& camera, const DirectX::XMFLOAT3& position,
        float playerYaw, float dt, bool manual, float mouseX, float mouseY)
    {
        using namespace GameplayMath;
        if (!std::isfinite(dt) || dt <= 0.0f) return;
        yaw = FollowAngle(yaw, playerYaw, turnSpeed, dt);
        if (manual)
        {
            manualYaw = Clamp(manualYaw + mouseX * mouseSensitivity, -yawLimit, yawLimit);
            manualPitch = Clamp(manualPitch + mouseY * mouseSensitivity, -pitchLimit, pitchLimit);
        }
        else
        {
            const float rate = FollowRate(2.5f, dt);
            manualYaw += (0.0f - manualYaw) * rate;
            manualPitch += (0.0f - manualPitch) * rate;
        }
        const float rate = FollowRate(positionSpeed, dt);
        focus.x += (position.x - focus.x) * rate;
        focus.y += (position.y + focusHeight - focus.y) * rate;
        focus.z += (position.z - focus.z) * rate;
        Apply(camera);
    }
    float GetYaw() const { return yaw; }
    float GetManualYaw() const { return manualYaw; }
    float GetManualPitch() const { return manualPitch; }
private:
    void Apply(Camera& camera)
    {
        const float pitch = GameplayMath::Clamp(basePitch + manualPitch, 0.1f, 1.3f);
        const float heading = yaw + manualYaw;
        const float horizontal = distance * std::cos(pitch);
        const DirectX::XMFLOAT3 eye = {
            focus.x - std::sin(heading) * horizontal,
            focus.y + std::sin(pitch) * distance,
            focus.z - std::cos(heading) * horizontal
        };
        camera.SetLookAt(eye, focus, { 0, 1, 0 });
    }
    float yaw = 0.0f, manualYaw = 0.0f, manualPitch = 0.0f;
    DirectX::XMFLOAT3 focus = { 0, 0, 0 };
};
