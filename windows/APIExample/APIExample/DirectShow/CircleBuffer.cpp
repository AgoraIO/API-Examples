
#include "CircleBuffer.hpp"

CircleBuffer* CircleBuffer::m_pCircleBuffer = nullptr;

CircleBuffer* CircleBuffer::GetInstance()
{
    if (m_pCircleBuffer) {
        return m_pCircleBuffer;
    }
    m_pCircleBuffer = new CircleBuffer(MAX_AUDIO_SAMPLE_SIZE, 0);
    return m_pCircleBuffer;
}

void CircleBuffer::CloseInstance()
{
    if (m_pCircleBuffer) {
        delete m_pCircleBuffer;
        m_pCircleBuffer = nullptr;
    }
}

CircleBuffer::CircleBuffer(const unsigned int iBufferSize,int waittimeout)
{
	this->m_iBufferSize = iBufferSize;
	this->m_pBuffer = (BYTE*)malloc(iBufferSize);
	this->m_iInternalReadCursor  = 0;
	this->m_iInternalWriteCursor = 0;
	this->m_bComplete            = FALSE;
	this->m_evtDataAvailable     = CreateEvent(NULL, FALSE, FALSE, NULL);
	InitializeCriticalSection(&this->m_csCircleBuffer);
	this->wait_timeout=waittimeout;
}

CircleBuffer::~CircleBuffer(void)
{
	DeleteCriticalSection(&this->m_csCircleBuffer);
	CloseHandle(this->m_evtDataAvailable);
	free(this->m_pBuffer);
}

BOOL CircleBuffer::IsComplete()
{
	return this->m_bComplete;
}

void CircleBuffer::SetComplete()
{
	EnterCriticalSection(&this->m_csCircleBuffer);
	this->m_bComplete = TRUE;
	SetEvent(this->m_evtDataAvailable);
	LeaveCriticalSection(&this->m_csCircleBuffer);
}

unsigned int CircleBuffer::getFreeSize()
{
	unsigned int iNumBytesFree;

	EnterCriticalSection(&this->m_csCircleBuffer);

	if (this->m_iInternalWriteCursor < this->m_iInternalReadCursor)
		iNumBytesFree = (this->m_iInternalReadCursor - 1) - this->m_iInternalWriteCursor;
	else if (this->m_iInternalWriteCursor == this->m_iInternalReadCursor)
		iNumBytesFree = this->m_iBufferSize;
	else
		iNumBytesFree = (this->m_iInternalReadCursor - 1) + (this->m_iBufferSize - this->m_iInternalWriteCursor);

	LeaveCriticalSection(&this->m_csCircleBuffer);
	return iNumBytesFree;
}

unsigned int CircleBuffer::getUsedSize()
{
	return this->m_iBufferSize - this->getFreeSize();
}

void CircleBuffer::flushBuffer()
{
	EnterCriticalSection(&this->m_csCircleBuffer);
	this->m_iInternalReadCursor = 0;
	this->m_iInternalWriteCursor = 0;
	LeaveCriticalSection(&this->m_csCircleBuffer);
}

void CircleBuffer::writeBuffer(const void* pSourceBuffer, const unsigned int iNumBytes, int audioTime)
{
	unsigned int iBytesToWrite = iNumBytes;
	BYTE* pSourceReadCursor = (BYTE*)pSourceBuffer;

	EnterCriticalSection(&this->m_csCircleBuffer);
    //audioTimeQueue.push(audioTime);
//	if (this->m_iInternalWriteCursor >= this->m_iInternalReadCursor)			//2_9 avoid memcpy over
//	{
		unsigned int iChunkSize = this->m_iBufferSize - this->m_iInternalWriteCursor;
		if (iChunkSize > iBytesToWrite)
			iChunkSize = iBytesToWrite;

		memcpy(this->m_pBuffer + this->m_iInternalWriteCursor, pSourceReadCursor, iChunkSize);
		pSourceReadCursor += iChunkSize;
		iBytesToWrite -= iChunkSize;
		this->m_iInternalWriteCursor += iChunkSize;

		if (this->m_iInternalWriteCursor >= this->m_iBufferSize)
			this->m_iInternalWriteCursor -= this->m_iBufferSize;
//	}

	if (iBytesToWrite)
	{
		memcpy(this->m_pBuffer + this->m_iInternalWriteCursor, pSourceReadCursor, iBytesToWrite);
		this->m_iInternalWriteCursor += iBytesToWrite;
	}

	SetEvent(this->m_evtDataAvailable);
	LeaveCriticalSection(&this->m_csCircleBuffer);
}

BOOL CircleBuffer::readBuffer(void* pDestBuffer, const unsigned int _iBytesToRead, unsigned int* pbBytesRead, int& audioTime)
{
	unsigned int iBytesToRead = _iBytesToRead;
	unsigned int iBytesRead = 0;
	DWORD dwWaitResult;
	BOOL bComplete = FALSE;

	while (iBytesToRead > 0 && bComplete == FALSE)
	{
		dwWaitResult = WaitForSingleObject(this->m_evtDataAvailable, this->wait_timeout);
		if (dwWaitResult == WAIT_TIMEOUT)
		{
			*pbBytesRead = iBytesRead;
			return FALSE;
		}

		EnterCriticalSection(&this->m_csCircleBuffer);

        bool bPopTimeStamp = false;
		if (this->m_iInternalReadCursor > this->m_iInternalWriteCursor)
		{
			unsigned int iChunkSize = this->m_iBufferSize - this->m_iInternalReadCursor;
			if (iChunkSize > iBytesToRead)
				iChunkSize = iBytesToRead;

			memcpy((BYTE*)pDestBuffer + iBytesRead, this->m_pBuffer + this->m_iInternalReadCursor, iChunkSize);

			iBytesRead += iChunkSize;
			iBytesToRead -= iChunkSize;

			this->m_iInternalReadCursor += iChunkSize;
			if (this->m_iInternalReadCursor >= this->m_iBufferSize)
				this->m_iInternalReadCursor -= this->m_iBufferSize;
            bPopTimeStamp = true;
		}

		if (iBytesToRead && this->m_iInternalReadCursor < this->m_iInternalWriteCursor)
		{
			unsigned int iChunkSize = this->m_iInternalWriteCursor - this->m_iInternalReadCursor;
			if (iChunkSize > iBytesToRead)
				iChunkSize = iBytesToRead;

			memcpy((BYTE*)pDestBuffer + iBytesRead,
				   this->m_pBuffer + this->m_iInternalReadCursor,
				   iChunkSize);

			iBytesRead += iChunkSize;
			iBytesToRead -= iChunkSize;
			this->m_iInternalReadCursor += iChunkSize;
            bPopTimeStamp = true;
		}

        if(bPopTimeStamp && !audioTimeQueue.empty())  {
            audioTime = audioTimeQueue.front();//audio timestamp
            audioTimeQueue.pop();
        }

		if (this->m_iInternalReadCursor == this->m_iInternalWriteCursor)
		{
			if (this->m_bComplete)
				bComplete = TRUE;
		}
		else
			SetEvent(this->m_evtDataAvailable);

		LeaveCriticalSection(&this->m_csCircleBuffer);
	}

	*pbBytesRead = iBytesRead;
	return bComplete ? FALSE : TRUE;
}

