#pragma once
#include "AGVideoWnd.h"
#include <list>
#include <string>
#include "stdafx.h"
#include "rte_cpp.h"
using namespace rte;
#define WM_PLAYER_INIT (WM_USER + 10001)
#define WM_PLAYER_PREPARED (WM_USER + 10002)
#define WM_PLAYER_STATE (WM_USER + 10003)
#define WM_PLAYER_EVENT (WM_USER + 1004)
class PlayerObserverAdapter:public PlayerObserver
{
public:
	PlayerObserverAdapter();
	virtual~PlayerObserverAdapter();
	void onStateChanged(rte::PlayerState old_state, rte::PlayerState new_state,
		rte::Error* err) override;
	void onPositionChanged(uint64_t curr_time,
		uint64_t utc_time) override;
	void onResolutionChanged(int width, int height) override;
	void onEvent(rte::PlayerEvent event) override;
	void onMetadata(rte::PlayerMetadataType type,
		const uint8_t* data, size_t length) override;
	void onPlayerInfoUpdated(const rte::PlayerInfo* info) override;
	void onAudioVolumeIndication(int32_t volume) override;

};


class CRtePlayerDlg : public CDialogEx,public PlayerObserverAdapter
{
	DECLARE_DYNAMIC(CRtePlayerDlg)

public:
	CRtePlayerDlg(CWnd* pParent = nullptr); // standard constructor
	virtual ~CRtePlayerDlg();
	bool InitAgora();
	void UnInitAgora();
	void onStateChanged(rte::PlayerState old_state, rte::PlayerState new_state,rte::Error* err) override;
	void onEvent(rte::PlayerEvent event) override;
	enum
	{
		IDD = IDD_DIALOG_RTE_PLAYER
	};

protected:
	virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV support
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

private:
	void InitCtrlText();
	int JoinChannel(const char* channel);
	int LeaveChannel();
	HWND hWnd = NULL;
	rte::Rte *mRte = nullptr;
	rte::Player *mRtePlayer = nullptr;
	rte::Canvas *mCanvas = nullptr;
	std::string mRteUrl;

	bool isInitSuccess = false;
	bool isPrepareSuccess = false;
	
	void start();
	void stop();
	void play();
	void pause();
	void updateUiState();
	CEdit mRteUrlEc;
	CButton mBtStart;
	CButton mBtStop;
	CButton mBtPlay;
	CButton mBtPause;
	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnBnClickedButtonStop();
	afx_msg void OnBnClickedButtonPlay();
	afx_msg void OnBnClickedButtonPause();
	afx_msg LRESULT OnInit(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnPrepared(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnPlayerStateChanged(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnPlayerOnEvent(WPARAM wParam, LPARAM lParam);
	CListBox mListInfo;
	CStatic mVideoWnd;
};
