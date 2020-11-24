#ifndef AGORASCREENDLG_H
#define AGORASCREENDLG_H

#include <QWidget>
#include <IAgoraRtcEngine.h>
namespace Ui {
class AgoraScreenDlg;
}
class CScreenCaputreEventHandler;

class AgoraScreenDlg : public QWidget
{
    Q_OBJECT

public:
    explicit AgoraScreenDlg(QWidget *parent = nullptr);
    ~AgoraScreenDlg();

	bool InitAgora();
	void UnInitAgora();

	virtual void showEvent(QShowEvent *event) override;
public slots:
	void on_btnJoin_clicked();
	void on_btnWindow_clicked();
	void on_btnScreen_clicked();
	void on_btnUpdateCapParam_clicked();
	void onJoinChannelSuccess_slot(const char* channel, agora::rtc::uid_t uid, int elapsed);
signals:
	void onJoinChannelSuccess(const char* channel,unsigned int uid, int elapsed);
private:
    Ui::AgoraScreenDlg *ui;

	agora::rtc::IRtcEngine* m_rtcEngine = nullptr;
	bool m_initialize = false;
	bool m_bShareScreen = false;
	bool m_joinChannel = false;
	CScreenCaputreEventHandler* m_eventHandler;
	//render local video from SDK local capture.
	void RenderLocalVideo();
};

#endif // AGORASCREENDLG_H
