#include "PlayerAnimation.h"
#include <algorithm>
#include <cmath>

PlayerAnimation::PlayerAnimation(Model& source) : model(&source) {}

float PlayerAnimation::GetDuration(int index) const
{
    const auto& clips = model->GetResource()->GetAnimations();
    if (index < 0 || static_cast<std::size_t>(index) >= clips.size()) return 0.0f;
    return clips[index].secondsLength;
}

void PlayerAnimation::PlayAnimation(int index, bool loop)
{
    const auto& clips = model->GetResource()->GetAnimations();
    if (index < 0 || static_cast<std::size_t>(index) >= clips.size()
        || clips[index].keyframes.empty()) return;
    // 切替時点の姿勢を保存してから、次のクリップへブレンドする。
    blendSource.clear();
    for (const auto& node : model->GetNodes())
    {
        ModelResource::NodeKeyData pose{};
        pose.scale = node.scale;
        pose.rotate = node.rotate;
        pose.translate = node.translate;
        blendSource.push_back(pose);
    }
    animationIndex = index;
    animationLoop = loop;
    animationSeconds = blendSeconds = 0.0f;
    animationPlaying = true;
    UpdateAnimation(0.0f);
}

void PlayerAnimation::PlayAnimation(const char* name, bool loop)
{
    if (!name) return;
    const auto& clips = model->GetResource()->GetAnimations();
    for (std::size_t i = 0; i < clips.size(); ++i)
        if (clips[i].name == name) { PlayAnimation(static_cast<int>(i), loop); return; }
}

void PlayerAnimation::Update(float elapsedTime)
{
    if (!std::isfinite(elapsedTime) || elapsedTime < 0.0f) return;
    UpdateAnimation(elapsedTime);
}

void PlayerAnimation::UpdateAnimation(float elapsedTime)
{
    const auto& clips = model->GetResource()->GetAnimations();
    if (!animationPlaying || animationIndex < 0
        || static_cast<std::size_t>(animationIndex) >= clips.size())
    {
        model->UpdateTransform();
        return;
    }
    const auto& clip = clips[animationIndex];
    const auto& frames = clip.keyframes;
    if (frames.empty()) { animationPlaying = false; model->UpdateTransform(); return; }
    animationSeconds += elapsedTime;
    blendSeconds += elapsedTime;
    if (clip.secondsLength > 0.0f && animationSeconds >= clip.secondsLength)
    {
        if (animationLoop) animationSeconds = std::fmod(animationSeconds, clip.secondsLength);
        else { animationSeconds = clip.secondsLength; animationPlaying = false; }
    }
    else if (clip.secondsLength <= 0.0f) animationPlaying = false;

    // 1フレームしかないアニメ、終端、先頭にも対応する。

    std::size_t a = 0;
    std::size_t b = 0;

    if (animationSeconds >= frames.back().seconds)
    {
        a = frames.size() - 1;
        b = a;
    }
    else if (animationSeconds > frames.front().seconds)
    {
        for (std::size_t i = 0; i + 1 < frames.size(); ++i)
        {
            if (animationSeconds < frames[i + 1].seconds)
            {
                a = i;
                b = i + 1;
                break;
            }
        }
    }

    const float span = frames[b].seconds - frames[a].seconds;

    const float rate = span > 0.00001f
        ? (animationSeconds - frames[a].seconds) / span
        : 0.0f;
    const float blend = !animationPlaying || animationBlendSecondsLength <= 0.0f
        ? 1.0f : (std::min)(blendSeconds / animationBlendSecondsLength, 1.0f);
    auto& nodes = model->GetNodes();
    const std::size_t count = (std::min)(nodes.size(),
        (std::min)(frames[a].nodeKeys.size(), frames[b].nodeKeys.size()));
    using namespace DirectX;
    for (std::size_t i = 0; i < count; ++i)
    {
        const auto& k0 = frames[a].nodeKeys[i];
        const auto& k1 = frames[b].nodeKeys[i];
        XMVECTOR scale = XMVectorLerp(XMLoadFloat3(&k0.scale), XMLoadFloat3(&k1.scale), rate);
        XMVECTOR rotate = XMQuaternionSlerp(XMLoadFloat4(&k0.rotate), XMLoadFloat4(&k1.rotate), rate);
        XMVECTOR translate = XMVectorLerp(XMLoadFloat3(&k0.translate), XMLoadFloat3(&k1.translate), rate);
        if (blend < 1.0f && i < blendSource.size())
        {
            scale = XMVectorLerp(XMLoadFloat3(&blendSource[i].scale), scale, blend);
            rotate = XMQuaternionSlerp(XMLoadFloat4(&blendSource[i].rotate), rotate, blend);
            translate = XMVectorLerp(XMLoadFloat3(&blendSource[i].translate), translate, blend);
        }
        XMStoreFloat3(&nodes[i].scale, scale);
        XMStoreFloat4(&nodes[i].rotate, rotate);
        XMStoreFloat3(&nodes[i].translate, translate);
    }
    // 非ループ再生でも最終姿勢を適用してから停止する。
    model->UpdateTransform();
}
