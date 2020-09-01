#pragma once
#include <stdio.h>
#include <tchar.h>
#include <d3d9.h>

class D3DRender {

public:
	D3DRender();
	~D3DRender();

public:
	int Init(HWND hwnd, unsigned int nWidth, unsigned int nHeight, bool isYuv);
	void Cleanup();
	bool Render(char *buffer);

private:
	bool                    m_bIsYuv;
	int                     m_nWidth;
	int                     m_nHeight;
	RECT                    m_rtViewport;
	CRITICAL_SECTION        m_critial;
	IDirect3D9              *m_pDirect3D9;
	IDirect3DDevice9        *m_pDirect3DDevice;
	IDirect3DSurface9       *m_pDirect3DSurfaceRender;
};
