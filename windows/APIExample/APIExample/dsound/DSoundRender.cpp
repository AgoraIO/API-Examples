#include "dsound/DSoundRender.h"

BOOL DSoundRender::Init(HWND hWnd, int sample_rate, int channels, int bits_per_sample) {
	Close();
	std::lock_guard<std::mutex> _(m_mutex);
	if (FAILED(DirectSoundCreate8(NULL, &m_pDS, NULL)))
	{
#ifdef _DEBUG
		OutputDebugString(_T("DirectSoundCreate8 error!\n"));
#endif
		return FALSE;
	}
	if (FAILED(m_pDS->SetCooperativeLevel(hWnd, DSSCL_NORMAL)))
	{
#ifdef _DEBUG
		OutputDebugString(_T("SetCooperativeLevel error!\n"));
#endif
		return FALSE;
	}

	m_channels = channels;
	m_sample_rate = sample_rate;
	m_bits_per_sample = bits_per_sample;

	DSBUFFERDESC dsbd;
	memset(&dsbd, 0, sizeof(dsbd));
	dsbd.dwSize = sizeof(dsbd);
	dsbd.dwFlags = DSBCAPS_GLOBALFOCUS | DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_GETCURRENTPOSITION2;
	dsbd.dwBufferBytes = MAX_AUDIO_BUF * BUFFERNOTIFYSIZE;
	dsbd.lpwfxFormat = (WAVEFORMATEX*)new WAVEFORMATEX;
	dsbd.lpwfxFormat->wFormatTag = WAVE_FORMAT_PCM;
	dsbd.lpwfxFormat->nChannels = channels;
	dsbd.lpwfxFormat->nSamplesPerSec = sample_rate;
	dsbd.lpwfxFormat->nAvgBytesPerSec = sample_rate * (bits_per_sample / 8)*channels;
	dsbd.lpwfxFormat->nBlockAlign = (bits_per_sample / 8)*channels;
	dsbd.lpwfxFormat->wBitsPerSample = bits_per_sample;
	dsbd.lpwfxFormat->cbSize = 0;

	if (FAILED(m_pDS->CreateSoundBuffer(&dsbd, &m_pDSBuffer, NULL))) {
#ifdef _DEBUG
		OutputDebugString(_T("SetCooperativeLevel error!\n"));
#endif
		return FALSE;
	}
	if (FAILED(m_pDSBuffer->QueryInterface(IID_IDirectSoundBuffer8, (LPVOID*)&m_pDSBuffer8))) {
#ifdef _DEBUG
		OutputDebugString(_T("SetCooperativeLevel error!\n"));
#endif
		return FALSE;
	}
	if (FAILED(m_pDSBuffer8->QueryInterface(IID_IDirectSoundNotify, (LPVOID*)&m_pDSNotify))) {
#ifdef _DEBUG
		OutputDebugString(_T("SetCooperativeLevel error!\n"));
#endif
		return FALSE;
	}
	for (int i = 0; i < MAX_AUDIO_BUF; i++) {
		m_pDSPosNotify[i].dwOffset = i * BUFFERNOTIFYSIZE;
		m_event[i] = ::CreateEvent(NULL, FALSE, FALSE, NULL);
		m_pDSPosNotify[i].hEventNotify = m_event[i];
	}
	m_pDSNotify->SetNotificationPositions(MAX_AUDIO_BUF, m_pDSPosNotify);
	m_pDSNotify->Release();
	m_pDSBuffer8->SetCurrentPosition(0);
	m_pDSBuffer8->Play(0, 0, DSBPLAY_LOOPING);
	return TRUE;
}

void DSoundRender::Render(BYTE * buffer, int buffer_len)
{
	LPVOID buf = NULL;
	if ((res >= WAIT_OBJECT_0) && (res <= WAIT_OBJECT_0 + 3))
	{
		std::lock_guard<std::mutex> _(m_mutex);
		m_pDSBuffer8->Lock(offset, buffer_len, &buf, (DWORD*)&buffer_len, NULL, NULL, 0);
		memcpy(buf, buffer, buffer_len);
		offset += BUFFERNOTIFYSIZE;
		offset %= (BUFFERNOTIFYSIZE * MAX_AUDIO_BUF);
#ifdef _DEBUG
		TCHAR buffer[1024];
#ifdef _UNICODE
		swprintf_s(buffer, 1024, _T("offset:%d ,data_len:%d\n"), offset, buffer_len);
#else
		sprintf_s(buffer, 1024, _T("offset:%d ,data_len:%d\n"), offset, buffer_len);
#endif // _UNICODE
		OutputDebugString(buffer);
#endif 
		m_pDSBuffer8->Unlock(buf, buffer_len, NULL, 0);
	}
	res = WaitForMultipleObjects(MAX_AUDIO_BUF, m_event, FALSE, INFINITE);
}

void DSoundRender::Close()
{
	std::lock_guard<std::mutex> _(m_mutex);
	if (m_pDSNotify)
	{
		m_pDSNotify->Release();
		m_pDSNotify = nullptr;
	}
	if (m_pDSBuffer8)
	{
		m_pDSBuffer8->Release();
		m_pDSBuffer8 = nullptr;
		m_pDSBuffer = nullptr;
	}
	if (m_pDS)
	{
		m_pDS->Release();
		m_pDS = nullptr;
	}
	for (int i = 0; i < MAX_AUDIO_BUF; i++) {
		if (m_event[i])
			CloseHandle(m_event[i]);
	}
}


