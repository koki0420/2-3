#pragma once
// XAUDIO2

#include <xaudio2.h>
#include <wrl.h>

#include <mmreg.h>
#include <memory>

#include <vector>

#include "misc.h"

class audio_device
{
public:
	static IXAudio2* xaudio2;
	static IXAudio2MasteringVoice* master_voice;

	static bool initialize()
	{
		HRESULT hr = S_OK;

		hr = XAudio2Create(&xaudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		hr = xaudio2->CreateMasteringVoice(&master_voice);
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		return hr == S_OK;
	}
	static void uninitialize()
	{
		master_voice->DestroyVoice();
		xaudio2->Release();
	}
};

class audio_source_voice;
class audio_buffer
{
	WAVEFORMATEXTENSIBLE wfx_ = { 0 };
	XAUDIO2_BUFFER buffer_ = { 0 };
public:
	audio_buffer(const wchar_t* filename);
	virtual ~audio_buffer();
	audio_buffer(const audio_buffer& rhs) = delete;
	audio_buffer& operator=(const audio_buffer& rhs) = delete;
	audio_buffer(audio_buffer&&) noexcept = delete;
	audio_buffer& operator=(audio_buffer&&) noexcept = delete;

	friend class audio_source_voice;
};

class audio_source_voice
{
	IXAudio2SourceVoice* source_voice_;
	size_t currentVoiceIndex = 0;
	std::shared_ptr<audio_buffer> audio_buffer_;

public:
	audio_source_voice(std::shared_ptr<audio_buffer>& audio_buffer);
	virtual ~audio_source_voice();
	audio_source_voice(const audio_source_voice& rhs) = delete;
	audio_source_voice& operator=(const audio_source_voice& rhs) = delete;
	audio_source_voice(audio_source_voice&&) noexcept = delete;
	audio_source_voice& operator=(audio_source_voice&&) noexcept = delete;

	void play(int loop_count = 0/*255 : XAUDIO2_LOOP_INFINITE*/);
	void stop(bool play_tails = true);
	void volume(float volume);
	void pan(
		float pan_value/*pan_value of -1.0 indicates all left speaker, 1.0 is all right speaker, 0.0 is split between left and right*/);
	bool queuing(int handle);
};

