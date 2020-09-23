#include "AgVideoBuffer.h"
#include <mutex>

std::mutex buf_mutex;
BYTE CAgVideoBuffer::videoBuffer[VIDEO_BUF_SIZE] = { 0 };

CAgVideoBuffer* CAgVideoBuffer::GetInstance()
{
    static CAgVideoBuffer agVideoBuffer;
    return &agVideoBuffer;
}

CAgVideoBuffer::CAgVideoBuffer()
{

}

CAgVideoBuffer::~CAgVideoBuffer()
{

}


void CAgVideoBuffer::SetVideoFormat(const BITMAPINFOHEADER *lpInfoHeader)
{
    memcpy_s(&m_bmiHeader, sizeof(BITMAPINFOHEADER), lpInfoHeader, sizeof(BITMAPINFOHEADER));
    m_nPackageSize = m_bmiHeader.biWidth*m_bmiHeader.biWidth * 3 / 2;
    _ASSERT(m_nPackageSize <= VIDEO_BUF_SIZE);
}

void CAgVideoBuffer::GetVideoFormat(BITMAPINFOHEADER *lpInfoHeader)
{
    memcpy_s(lpInfoHeader, sizeof(BITMAPINFOHEADER), &m_bmiHeader, sizeof(BITMAPINFOHEADER));
}

bool CAgVideoBuffer::writeBuffer(BYTE* buffer, int bufsize, int ts)
{
    if ((size_t)bufsize < m_nPackageSize)
        return false;
    std::lock_guard<std::mutex> buf_lock(buf_mutex);
    memcpy_s(videoBuffer, bufsize, buffer, bufsize);
    timestamp = ts;
    return true;
}
bool CAgVideoBuffer::readBuffer(BYTE* buffer, int bufsize, int& ts)
{
    if ((size_t)bufsize < m_nPackageSize)
        return false;
    std::lock_guard<std::mutex> buf_lock(buf_mutex);
    memcpy_s(buffer, bufsize, videoBuffer, bufsize);
    ts = timestamp;
    return true;
}