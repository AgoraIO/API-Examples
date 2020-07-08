#pragma once
#include <afxwin.h>
#define VIDEO_BUF_SIZE 4*4*1920*1080//
class CAgVideoBuffer
{
public:
    CAgVideoBuffer();
    ~CAgVideoBuffer();

    void SetVideoFormat(const BITMAPINFOHEADER *lpInfoHeader);
    void GetVideoFormat(BITMAPINFOHEADER *lpInfoHeader);

    bool writeBuffer(BYTE* buffer, int bufsize, int ts);
    bool readBuffer(BYTE* buffer, int bufsize, int& ts);

    static CAgVideoBuffer* GetInstance();
private:
    static BYTE videoBuffer[VIDEO_BUF_SIZE];
    BITMAPINFOHEADER	m_bmiHeader;
    SIZE_T				m_nPackageSize;
    int                 timestamp;
};

