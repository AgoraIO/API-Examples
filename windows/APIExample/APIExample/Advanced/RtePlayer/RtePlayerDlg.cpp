#include "stdafx.h"
#include "APIExample.h"
#include "RtePlayerDlg.h"

PlayerObserverAdapter::PlayerObserverAdapter() {}
PlayerObserverAdapter::~PlayerObserverAdapter() {}
void PlayerObserverAdapter::onStateChanged(rte::PlayerState old_state, rte::PlayerState new_state,
	rte::Error* err) {}
void PlayerObserverAdapter::onPositionChanged(uint64_t curr_time,
	uint64_t utc_time) {}
void PlayerObserverAdapter::onResolutionChanged(int width, int height) {}
void PlayerObserverAdapter::onEvent(rte::PlayerEvent event) {}
void PlayerObserverAdapter::onMetadata(rte::PlayerMetadataType type,
	const uint8_t* data, size_t length) {}
void PlayerObserverAdapter::onPlayerInfoUpdated(const rte::PlayerInfo* info) {}
void PlayerObserverAdapter::onAudioVolumeIndication(int32_t volume) {}

IMPLEMENT_DYNAMIC(CRtePlayerDlg, CDialogEx)

CRtePlayerDlg::CRtePlayerDlg(CWnd* pParent /*=nullptr*/) : CDialogEx(IDD_DIALOG_RTE_PLAYER, pParent)
{
}

CRtePlayerDlg::~CRtePlayerDlg()
{
}

void CRtePlayerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_RTE_URL, mRteUrlEc);
	DDX_Control(pDX, IDC_BUTTON_START, mBtStart);
	DDX_Control(pDX, IDC_BUTTON_STOP, mBtStop);
	DDX_Control(pDX, IDC_BUTTON_PLAY, mBtPlay);
	DDX_Control(pDX, IDC_BUTTON_PAUSE, mBtPause);
	DDX_Control(pDX, IDC_LIST_INFO_BROADCASTING, mListInfo);
	DDX_Control(pDX, IDC_STATIC_VIDEO, mVideoWnd);
}

BEGIN_MESSAGE_MAP(CRtePlayerDlg, CDialogEx)
	ON_MESSAGE(WM_MSGID(WM_PLAYER_INIT), &CRtePlayerDlg::OnInit)
	ON_MESSAGE(WM_MSGID(WM_PLAYER_PREPARED), &CRtePlayerDlg::OnPrepared)
	ON_MESSAGE(WM_MSGID(WM_PLAYER_STATE), &CRtePlayerDlg::OnPlayerStateChanged)
	ON_MESSAGE(WM_MSGID(WM_PLAYER_EVENT), &CRtePlayerDlg::OnPlayerOnEvent)
	ON_BN_CLICKED(IDC_BUTTON_START, &CRtePlayerDlg::OnBnClickedButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CRtePlayerDlg::OnBnClickedButtonStop)
	ON_BN_CLICKED(IDC_BUTTON_PLAY, &CRtePlayerDlg::OnBnClickedButtonPlay)
	ON_BN_CLICKED(IDC_BUTTON_PAUSE, &CRtePlayerDlg::OnBnClickedButtonPause)
END_MESSAGE_MAP()

BOOL CRtePlayerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	InitCtrlText();
	return TRUE;
}

// set control text from config.
void CRtePlayerDlg::InitCtrlText()
{
	std::string strAppID = GET_APP_ID;
	std::string str = "ret://" + strAppID + "/";
	mRteUrlEc.SetWindowTextW(CA2W(str.c_str()));
	updateUiState();
}

int CRtePlayerDlg::JoinChannel(const char* channel)
{

	return 0;
}

int CRtePlayerDlg::LeaveChannel()
{

	return 0;
}

void CRtePlayerDlg::start()
{
	if (isInitSuccess && mRtePlayer)
	{
		mRtePlayer->OpenWithUrl(mRteUrl.c_str(), 0, [&](rte::Error* err) {
			ErrorCode code = kRteOk;
			if (err != nullptr && err->Code() != kRteOk)
			{
				code = err->Code();
			}
			::PostMessage(m_hWnd, WM_MSGID(WM_PLAYER_PREPARED), (WPARAM)code, 0);
			});

	}
}

void CRtePlayerDlg::stop()
{
	if (isInitSuccess && mRtePlayer)
	{
		mRtePlayer->Stop();
	}
}

void CRtePlayerDlg::play()
{
	if (isInitSuccess && mRtePlayer)
	{
		mRtePlayer->Play();
	}
}

void CRtePlayerDlg::pause()
{
	if (isInitSuccess && mRtePlayer)
	{
		mRtePlayer->Pause();
	}
}

void CRtePlayerDlg::updateUiState()
{
	if (isInitSuccess)
	{
		if (isPrepareSuccess)
		{
			mBtStart.EnableWindow(FALSE);
			mBtStop.EnableWindow();
			mBtPlay.EnableWindow();
			mBtPause.EnableWindow();
		}
		else
		{
			mBtStart.EnableWindow();
			mBtStop.EnableWindow(FALSE);
			mBtPlay.EnableWindow(FALSE);
			mBtPause.EnableWindow(FALSE);
		}
	}
	else
	{
		mBtStart.EnableWindow();
		mBtStop.EnableWindow(FALSE);
		mBtPlay.EnableWindow(FALSE);
		mBtPause.EnableWindow(FALSE);
	}
}

// Initialize the Agora SDK
bool CRtePlayerDlg::InitAgora()
{
	rte::Config rteConfig;
	rteConfig.SetAppId(GET_APP_ID.c_str());
	mRte = new rte::Rte();
	mRte->SetConfigs(&rteConfig);
	mRte->InitMediaEngine([this](rte::Error* err)
		{
			ErrorCode code = kRteOk;
			if (err != nullptr && err->Code() != kRteOk)
			{
				code = err->Code();
			}
			::PostMessage(m_hWnd, WM_MSGID(WM_PLAYER_INIT), (WPARAM)code, 0);

		});

	return true;
}

// UnInitialize the Agora SDK
void CRtePlayerDlg::UnInitAgora()
{
	if (mCanvas)
	{
		delete mCanvas;
		mCanvas = nullptr;
	}
	if (mRtePlayer)
	{
		delete mRtePlayer;
		mRtePlayer = nullptr;
	}
	if (mRte)
	{
		delete mRte;
		mRte = nullptr;
	}
}

void CRtePlayerDlg::onStateChanged(rte::PlayerState old_state, rte::PlayerState new_state, rte::Error* err)
{

	ErrorCode code = kRteOk;
	if (err != nullptr && err->Code() != kRteOk)
	{
		code = err->Code();
	}
	::PostMessage(m_hWnd, WM_MSGID(WM_PLAYER_STATE), (WPARAM)new_state, (LPARAM)code);

}

void CRtePlayerDlg::onEvent(rte::PlayerEvent event)
{
	::PostMessage(m_hWnd, WM_MSGID(WM_PLAYER_EVENT), (WPARAM)event, 0);
}

void CRtePlayerDlg::OnBnClickedButtonStart()
{
	CString url;
	mRteUrlEc.GetWindowText(url);
	mRteUrl = cs2utf8(url);
	start();
}

void CRtePlayerDlg::OnBnClickedButtonStop()
{
	stop();
}

void CRtePlayerDlg::OnBnClickedButtonPlay()
{
	play();
}

void CRtePlayerDlg::OnBnClickedButtonPause()
{
	pause();
}

LRESULT CRtePlayerDlg::OnInit(WPARAM wParam, LPARAM lParam)
{
	ErrorCode code = (ErrorCode)wParam;
	if (code == kRteOk)
	{
		mRtePlayer = new rte::Player(mRte);
		mCanvas = new rte::Canvas(mRte);
		RteViewConfig viewConfig;
		RteView view = mVideoWnd.GetSafeHwnd();
		mCanvas->AddView(&view, &viewConfig);
		mRtePlayer->RegisterObserver(this);
		mRtePlayer->SetCanvas(mCanvas);
		isInitSuccess = true;
		mListInfo.AddString(_T("InitMediaEngine success"));
	}
	else
	{
		isInitSuccess = false;
		CString str;
		str.Format(_T("InitMediaEngine failed, code:%d"), code);
		mListInfo.AddString(str);
	}
	return 0;
}
LRESULT CRtePlayerDlg::OnPrepared(WPARAM wParam, LPARAM lParam)
{
	ErrorCode code = (ErrorCode)wParam;
	CString str;
	if (code == kRteOk)
	{
		isPrepareSuccess = true;
		mListInfo.AddString(_T("OpenWithUrl success"));
	}
	else
	{
		isPrepareSuccess = false;
		str.Format(_T("OpenWithUrl failed, code:%d"), code);
		mListInfo.AddString(str);
		stop();
	}
	updateUiState();
	return 0;
}
LRESULT CRtePlayerDlg::OnPlayerStateChanged(WPARAM wParam, LPARAM lParam)
{
	rte::PlayerState new_state = (rte::PlayerState)wParam;
	ErrorCode code = (ErrorCode)lParam;
	CString str;
	switch (new_state)
	{
	case kRtePlayerStateIdle:
		mListInfo.AddString(_T("kRtePlayerStateIdle"));
		break;
	case kRtePlayerStateOpening:
		mListInfo.AddString(_T("kRtePlayerStateOpening"));
		break;
	case kRtePlayerStateOpenCompleted:
		mListInfo.AddString(_T("kRtePlayerStateOpenCompleted"));
		break;
	case kRtePlayerStatePlaying:
		mListInfo.AddString(_T("kRtePlayerStatePlaying"));
		break;
	case kRtePlayerStatePaused:
		mListInfo.AddString(_T("kRtePlayerStatePaused"));
		break;
	case kRtePlayerStatePlaybackCompleted:
		mListInfo.AddString(_T("kRtePlayerStatePlaybackCompleted"));
		break;
	case kRtePlayerStateStopped:
		mListInfo.AddString(_T("kRtePlayerStateStopped"));
		isPrepareSuccess = false;
		break;
	case kRtePlayerStateFailed:
		str.Format(_T("kRtePlayerStateFailed, code:%d"), code);
		mListInfo.AddString(str);
		isPrepareSuccess = false;
		stop();
		break;
	default:
		break;
	}
	updateUiState();
	return 0;
}
LRESULT CRtePlayerDlg::OnPlayerOnEvent(WPARAM wParam, LPARAM lParam)
{
	rte::PlayerEvent event = (rte::PlayerEvent)wParam;
	switch (event)
	{
	case kRtePlayerEventSeekBegin:
		mListInfo.AddString(_T("onEvent kRtePlayerEventSeekBegin"));
		break;
	case kRtePlayerEventSeekComplete:
		mListInfo.AddString(_T("onEvent kRtePlayerEventSeekComplete"));
		break;
	case kRtePlayerEventSeekError:
		mListInfo.AddString(_T("onEvent kRtePlayerEventSeekError"));
		break;
	case kRtePlayerEventBufferLow:
		mListInfo.AddString(_T("onEvent kRtePlayerEventBufferLow"));
		break;
	case kRtePlayerEventBufferRecover:
		mListInfo.AddString(_T("onEvent kRtePlayerEventBufferRecover"));
		break;
	case kRtePlayerEventFreezeStart:
		mListInfo.AddString(_T("onEvent kRtePlayerEventFreezeStart"));
		break;
	case kRtePlayerEventFreezeStop:
		mListInfo.AddString(_T("onEvent kRtePlayerEventFreezeStop"));
		break;
	case kRtePlayerEventOneLoopPlaybackCompleted:
		mListInfo.AddString(_T("onEvent kRtePlayerEventOneLoopPlaybackCompleted"));
		break;
	case kRtePlayerEventAuthenticationWillExpire:
		mListInfo.AddString(_T("onEvent kRtePlayerEventAuthenticationWillExpire"));
		break;
	case kRtePlayerEventAbrFallbackToAudioOnlyLayer:
		mListInfo.AddString(_T("onEvent kRtePlayerEventAbrFallbackToAudioOnlyLayer"));
		break;
	case kRtePlayerEventAbrRecoverFromAudioOnlyLayer:
		mListInfo.AddString(_T("onEvent kRtePlayerEventAbrRecoverFromAudioOnlyLayer"));
		break;

	default:
		break;
	}
	return 0;
}
