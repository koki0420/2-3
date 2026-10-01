#pragma once

// ItemとGimmickの基底クラスのようなイメージ
namespace Interact
{
	enum InteractionType //アイテムとギミックで共通する種類
	{
		None = -1,
		Bomb = 0,	//爆破
		Slash,		//斬撃
		Smash,		//吹き飛ばし
		Flash,		//閃光
		// dummy2,
	};
}