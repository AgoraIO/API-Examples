#include "D3DRender.h"

D3DRender::D3DRender()
{
	InitializeCriticalSection(&m_critial);
	m_pDirect3D9 = NULL;
	m_pDirect3DDevice = NULL;
	m_pDirect3DSurfaceRender = NULL;
}
D3DRender::~D3DRender()
{
	Close();
	DeleteCriticalSection(&m_critial);
}

//init render hwnd and set width and height.
int D3DRender::Init(HWND hwnd, unsigned int nWidth, unsigned int nHeight, bool isYuv) {

	HRESULT lRet;
	
	Close();

	m_pDirect3D9 = Direct3DCreate9(D3D_SDK_VERSION);
	if (m_pDirect3D9 == NULL)
		return -1;

	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

	GetClientRect(hwnd, &m_rtViewport);

	lRet = m_pDirect3D9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &m_pDirect3DDevice);
	if (FAILED(lRet))
		return -1;

	D3DFORMAT format = isYuv ? (D3DFORMAT)'21VY' : D3DFMT_X8R8G8B8;
	lRet = m_pDirect3DDevice->CreateOffscreenPlainSurface(nWidth, nHeight, format, D3DPOOL_DEFAULT, &m_pDirect3DSurfaceRender, NULL);
	if (FAILED(lRet))
		return -1;
	
	m_nWidth = nWidth;
	m_nHeight = nHeight;
	m_bIsYuv = isYuv;

	return 0;
}

void D3DRender::Close()
{
	EnterCriticalSection(&m_critial);
	if (m_pDirect3DSurfaceRender)
	{
		m_pDirect3DSurfaceRender->Release();
		m_pDirect3DSurfaceRender = NULL;
	}
	if (m_pDirect3DDevice)
	{
		m_pDirect3DDevice->Release();
		m_pDirect3DDevice = NULL;
	}
	if (m_pDirect3D9)
	{
		m_pDirect3D9->Release();
		m_pDirect3D9 = NULL;
	}
	LeaveCriticalSection(&m_critial);
}


bool D3DRender::Render(char *buffer) {

	if (!m_pDirect3DSurfaceRender || !buffer)
		return false;

	HRESULT lRet;
	D3DLOCKED_RECT d3d_rect;
	lRet = m_pDirect3DSurfaceRender->LockRect(&d3d_rect, NULL, D3DLOCK_DONOTWAIT);
	if (FAILED(lRet))
		return false;

	byte *pSrc = (byte *)buffer;
	byte * pDest = (BYTE *)d3d_rect.pBits;
	int stride = d3d_rect.Pitch;

	if (m_bIsYuv) {  
		for (int i = 0; i < m_nHeight; i++) {
			memcpy(pDest + i * stride, pSrc + i * m_nWidth, m_nWidth);
		}
		for (int i = 0; i < m_nHeight / 2; i++) {
			memcpy(pDest + stride * m_nHeight + i * stride / 2, pSrc + m_nWidth * m_nHeight * 5 / 4 + i * m_nWidth / 2, m_nWidth / 2);
		}
		for (int i = 0; i < m_nHeight / 2; i++) {
			memcpy(pDest + stride * m_nHeight + stride * m_nHeight / 4 + i * stride / 2, pSrc + m_nWidth * m_nHeight + i * m_nWidth / 2, m_nWidth / 2);
		}
	}
	else {
		int pixel_w_size = m_nWidth * 4;
		for (int i = 0; i < m_nHeight; i++) {
			memcpy(pDest, pSrc, pixel_w_size);
			pDest += stride;
			pSrc += pixel_w_size;
		}
	}

	lRet = m_pDirect3DSurfaceRender->UnlockRect();
	if (FAILED(lRet))
		return false;

	if (m_pDirect3DDevice == NULL)
		return false;

	m_pDirect3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	m_pDirect3DDevice->BeginScene();
	IDirect3DSurface9 * pBackBuffer = NULL;

	m_pDirect3DDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);
	m_pDirect3DDevice->StretchRect(m_pDirect3DSurfaceRender, NULL, pBackBuffer, &m_rtViewport, D3DTEXF_LINEAR);
	m_pDirect3DDevice->EndScene();
	m_pDirect3DDevice->Present(NULL, NULL, NULL, NULL);
	pBackBuffer->Release();

	return true;
}
