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


public:
	Player();
	~Player() {};

	void Initialize();


	// XVˆ—
	void Update(float elapsedTime, Object& S, const Camera& C);
	// •`‰æˆ—
	void Render(float elapsedTime, RenderContext rc,bool Pause);


};

