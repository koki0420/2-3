#pragma once
#include<memory>

//前方宣言
class audio_buffer;

//シーンごとに呼び出す
static struct AudioResource //音源を種類ごとに管理(保存)
{
	//変数につくinlineは「複数の翻訳単位に同じ定義が存在してもリンカは1つのオブジェクトとして扱う」という意味であり、
	// cppファイルで実体を作る必要がなくなる
	inline static std::shared_ptr<audio_buffer> audio_buffers[8]; //要素数は、何種類の音源を使うか
};