#pragma once
#include <d3d9.h>
/**
 * D3DRender
 * You'll need to call the Init function to pass in an HWND and window size 
 * that supports YUV data and RGB data.The incoming data can then be called to Render.
 * 
 */

class D3DRender {

public:
	D3DRender();
	~D3DRender();
public:
	//initialize window
	//hwnd is render to window.nWidth is buffer width not window width,nHeight is buffer height not window height,
	//isYuv to identify yuv
	int Init(HWND hwnd, unsigned int nWidth, unsigned int nHeight, bool isYuv);
	//release d3d handle
	void Close();
	//accept buffer data to render window.
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
