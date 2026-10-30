#pragma once


#include "Graphics.h"
#include"Interaction.h"
#include <Sprite.h>
#include <EffectManager.h>
#include"Object.h"
#include "audio.h"


//‘O•ûéŒ¾
class audio_source_voice;


class Player
{
private:

	std::unique_ptr<Sprite> spriteHPGauge = nullptr;
	std::unique_ptr<Sprite> spriteGauge = nullptr;
	std::unique_ptr<Sprite> spriteHP = nullptr;
	std::unique_ptr<Sprite> spriteWP = nullptr;
	std::unique_ptr<Sprite> spriteW[3];

	int wt=0;
public:
	Player();
	~Player() {};

	void Initialize();


	// XVˆ—
	void Update(float elapsedTime, Object& S, const Camera& C);
	// •`‰æˆ—
	void Render(float elapsedTime, RenderContext rc,bool Pause);
	void UIRender(float elapsedTime, ID3D11DeviceContext* dc);


};

