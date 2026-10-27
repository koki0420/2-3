#pragma once
#include <vector>
#include "Model.h"

// Model は所有しない。Model より先にこのインスタンスを破棄する。
class PlayerAnimation
{
public:
    explicit PlayerAnimation(Model& model);
    void Update(float elapsedTime);
    void PlayAnimation(int index, bool loop);
    void PlayAnimation(const char* name, bool loop);
    bool IsPlaying() const { return animationPlaying; }
    int GetIndex() const { return animationIndex; }
    float GetDuration(int index) const;
    void Stop() { animationPlaying = false; }

private:
    void UpdateAnimation(float elapsedTime);
    Model* model;
    int animationIndex = -1;
    float animationSeconds = 0.0f;
    bool animationLoop = false;
    bool animationPlaying = false;
    float animationBlendSecondsLength = 0.12f;
    float blendSeconds = 0.0f;
    std::vector<ModelResource::NodeKeyData> blendSource;
};
