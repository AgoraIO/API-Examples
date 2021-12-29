#pragma once
#include <windows.h>
#include <dsound.h>
#include "tchar.h"
#include <mutex>

#define MAX_AUDIO_BUF 4 
#define BUFFERNOTIFYSIZE 192000 


class DSoundRender
{
public:
	DSoundRender()
	{
		for (int i = 0; i < MAX_AUDIO_BUF; i++)
		{
			m_event[i] = 0;
		}
	}
	~DSoundRender() {
		Close();
	}
	BOOL Init(HWND hWnd, int sample_rate, int channels, int bits_per_sample);
	void Render(BYTE * buffer, int buffer_len);
	void Close();

private:
	IDirectSound8 *m_pDS = NULL;
	IDirectSoundBuffer8 *m_pDSBuffer8 = NULL;
	IDirectSoundBuffer *m_pDSBuffer = NULL;
	IDirectSoundNotify8 *m_pDSNotify = NULL;
	DSBPOSITIONNOTIFY m_pDSPosNotify[MAX_AUDIO_BUF];

	HANDLE m_event[MAX_AUDIO_BUF];
	DWORD res = WAIT_OBJECT_0;
	DWORD offset = 0;

	int m_sample_rate = 44100;
	int m_channels = 2;
	int m_bits_per_sample = 16;
	std::mutex m_mutex;
};