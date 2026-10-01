// XAUDIO2

#include "audio.h"

#include <x3daudio.h>

#include <windows.h>
#include <winerror.h>

IXAudio2* audio_device::xaudio2 = NULL;
IXAudio2MasteringVoice* audio_device::master_voice = NULL;


HRESULT find_chunk(HANDLE hfile, DWORD fourcc, DWORD& chunk_size, DWORD& chunk_data_position)
{

	HRESULT hr = S_OK;

	if (INVALID_SET_FILE_POINTER == SetFilePointer(hfile, 0, NULL, FILE_BEGIN))
	{
		return HRESULT_FROM_WIN32(GetLastError());
	}

	DWORD chunk_type;
	DWORD chunk_data_size;
	DWORD riff_data_size = 0;
	DWORD file_type;
	DWORD bytes_read = 0;
	DWORD offset = 0;

	while (hr == S_OK)
	{
		DWORD number_of_bytes_read;
		if (0 == ReadFile(hfile, &chunk_type, sizeof(DWORD), &number_of_bytes_read, NULL))
		{
			hr = HRESULT_FROM_WIN32(GetLastError());
		}

		if (0 == ReadFile(hfile, &chunk_data_size, sizeof(DWORD), &number_of_bytes_read, NULL))
		{
			hr = HRESULT_FROM_WIN32(GetLastError());
		}

		switch (chunk_type)
		{
		case 'FFIR'/*RIFF*/:
			riff_data_size = chunk_data_size;
			chunk_data_size = 4;
			if (0 == ReadFile(hfile, &file_type, sizeof(DWORD), &number_of_bytes_read, NULL))
			{
				hr = HRESULT_FROM_WIN32(GetLastError());
			}
			break;

		default:
			if (INVALID_SET_FILE_POINTER == SetFilePointer(hfile, chunk_data_size, NULL, FILE_CURRENT))
			{
				return HRESULT_FROM_WIN32(GetLastError());
			}
		}

		offset += sizeof(DWORD) * 2;

		if (chunk_type == fourcc)
		{
			chunk_size = chunk_data_size;
			chunk_data_position = offset;
			return S_OK;
		}

		offset += chunk_data_size;

		if (bytes_read >= riff_data_size)
		{
			return S_FALSE;
		}
	}

	return S_OK;

}

HRESULT read_chunk_data(HANDLE hFile, LPVOID buffer, DWORD buffer_size, DWORD buffer_offset)
{
	HRESULT hr = S_OK;
	if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, buffer_offset, NULL, FILE_BEGIN))
	{
		return HRESULT_FROM_WIN32(GetLastError());
	}
	DWORD number_of_bytes_read;
	if (0 == ReadFile(hFile, buffer, buffer_size, &number_of_bytes_read, NULL))
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
	}
	return hr;
}

audio_buffer::audio_buffer(const wchar_t* filename)
{
	HRESULT hr;

	// Open the file
	HANDLE hfile = CreateFileW(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (INVALID_HANDLE_VALUE == hfile)
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	if (INVALID_SET_FILE_POINTER == SetFilePointer(hfile, 0, NULL, FILE_BEGIN))
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	DWORD chunk_size;
	DWORD chunk_position;
	//check the file type, should be 'WAVE' or 'XWMA'
	find_chunk(hfile, 'FFIR'/*RIFF*/, chunk_size, chunk_position);
	DWORD filetype;
	read_chunk_data(hfile, &filetype, sizeof(DWORD), chunk_position);
	_ASSERT_EXPR(filetype == 'EVAW'/*WAVE*/, L"Only support 'WAVE'");

	find_chunk(hfile, ' tmf'/*FMT*/, chunk_size, chunk_position);
	read_chunk_data(hfile, &wfx_, chunk_size, chunk_position);

	//fill out the audio data buffer with the contents of the fourccDATA chunk
	find_chunk(hfile, 'atad'/*DATA*/, chunk_size, chunk_position);
	BYTE* data = new BYTE[chunk_size];
	read_chunk_data(hfile, data, chunk_size, chunk_position);

	buffer_.AudioBytes = chunk_size;  //size of the audio buffer in bytes
	buffer_.pAudioData = data;  //buffer containing audio data
	buffer_.Flags = XAUDIO2_END_OF_STREAM; // tell the source voice not to expect any data after this buffer
}

audio_buffer::~audio_buffer()
{
	delete[] buffer_.pAudioData;
}

audio_source_voice::audio_source_voice(std::shared_ptr<audio_buffer>& audio_buffer)
	: audio_buffer_(audio_buffer)
{
	HRESULT hr;

	hr = audio_device::xaudio2->CreateSourceVoice(&source_voice_, (WAVEFORMATEX*)&audio_buffer->wfx_);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
}

audio_source_voice::~audio_source_voice()
{
	source_voice_->DestroyVoice();
}
void audio_source_voice::play(int loop_count)
{
	HRESULT hr;

	XAUDIO2_VOICE_STATE voice_state = {};
	source_voice_->GetState(&voice_state);

	if (voice_state.BuffersQueued)
	{
		//stop(false, 0);
		return;
	}

	audio_buffer_->buffer_.LoopCount = loop_count;
	hr = source_voice_->SubmitSourceBuffer(&audio_buffer_->buffer_);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	hr = source_voice_->Start(0);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
}

void audio_source_voice::stop(bool play_tails/*Continue emitting effect output after the voice is stopped. */)
{
	XAUDIO2_VOICE_STATE voice_state = {};
	source_voice_->GetState(&voice_state);
	if (!voice_state.BuffersQueued)
	{
		return;
	}

	HRESULT hr;
	hr = source_voice_->Stop(play_tails ? XAUDIO2_PLAY_TAILS : 0);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	hr = source_voice_->FlushSourceBuffers();
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

}

void audio_source_voice::volume(float volume)
{
	HRESULT hr = source_voice_->SetVolume(volume);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
}

bool audio_source_voice::queuing(int handle)
{
	XAUDIO2_VOICE_STATE voice_state = {};
	source_voice_->GetState(&voice_state);
	return voice_state.BuffersQueued;
}

//#define SPEAKER_MONO (0x0)
//#define SPEAKER_FRONT_LEFT (0x1)
//#define SPEAKER_FRONT_RIGHT (0x2)
//#define SPEAKER_STEREO SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT
//#define SPEAKER_FRONT_CENTER (0x4)
//#define SPEAKER_LOW_FREQUENCY (0x8)
//#define SPEAKER_BACK_LEFT (0x10)
//#define SPEAKER_BACK_RIGHT (0x20)
//#define SPEAKER_5POINT1 SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_FRONT_CENTER | SPEAKER_LOW_FREQUENCY | SPEAKER_BACK_LEFT | SPEAKER_BACK_RIGHT

void audio_source_voice::pan(float pan_value/*pan_value of -1.0 indicates all left speaker, 1.0 is all right speaker, 0.0 is split between left and right*/)
{
	// https://learn.microsoft.com/en-us/windows/win32/xaudio2/how-to--pan-a-sound
	float output_matrix[8] = {};

	float left = 0.5f - pan_value / 2;
	float right = 0.5f + pan_value / 2;

	DWORD channel_mask;
	audio_device::master_voice->GetChannelMask(&channel_mask);
	switch (channel_mask)
	{
	case SPEAKER_MONO:
		output_matrix[0] = 1.0;
		break;
	case SPEAKER_STEREO:
	case SPEAKER_2POINT1:
	case SPEAKER_SURROUND:
		output_matrix[0] = left;
		output_matrix[1] = right;
		break;
	case SPEAKER_QUAD:
		output_matrix[0] = output_matrix[2] = left;
		output_matrix[1] = output_matrix[3] = right;
		break;
	case SPEAKER_4POINT1:
		output_matrix[0] = output_matrix[3] = left;
		output_matrix[1] = output_matrix[4] = right;
		break;
	case SPEAKER_5POINT1:
	case SPEAKER_7POINT1:
	case SPEAKER_5POINT1_SURROUND:
		output_matrix[0] = output_matrix[4] = left;
		output_matrix[1] = output_matrix[5] = right;
		break;
	case SPEAKER_7POINT1_SURROUND:
		output_matrix[0] = output_matrix[4] = output_matrix[6] = left;
		output_matrix[1] = output_matrix[5] = output_matrix[7] = right;
		break;
	}

	XAUDIO2_VOICE_DETAILS voice_details;
	source_voice_->GetVoiceDetails(&voice_details);

	XAUDIO2_VOICE_DETAILS master_voice_details;
	audio_device::master_voice->GetVoiceDetails(&master_voice_details);

	HRESULT hr = source_voice_->SetOutputMatrix(NULL, voice_details.InputChannels, 2/*master_voice_details.InputChannels*/, output_matrix);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
}
