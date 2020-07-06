#pragma once
#include <windows.h>
#include <queue>
#define CIC_WAITTIMEOUT		0
#define AUDIO_CALLBACK_TIMES  100
#define MAX_AUDIO_SAMPLE_SIZE (48000*2*2/AUDIO_CALLBACK_TIMES)//sampleRate*sizeof(16bit)*channel+ AUDIO_CALLBACK_TIMES*sizeof(timestamp)=max_s
class CircleBuffer
{
private:
	BYTE* m_pBuffer;
	unsigned int m_iBufferSize;
	unsigned int m_iInternalReadCursor;
	unsigned int m_iInternalWriteCursor;
	HANDLE m_evtDataAvailable;
	CRITICAL_SECTION m_csCircleBuffer;
	BOOL m_bComplete;
	int wait_timeout;
    std::queue<int> audioTimeQueue;
    static CircleBuffer* m_pCircleBuffer;
public:
    CircleBuffer(const unsigned int iBufferSize,int waittimeout);
	~CircleBuffer(void);
	BOOL IsComplete();
	void SetComplete();
	unsigned int getFreeSize();
	unsigned int getUsedSize();
	void writeBuffer(const void* pSourceBuffer, const unsigned int iNumBytes, int audioTime);
	BOOL readBuffer(void* pDestBuffer, const unsigned int iBytesToRead, unsigned int* pbBytesRead, int& audioTime);
	void flushBuffer();

    static CircleBuffer* GetInstance();
    static void CloseInstance();
};

