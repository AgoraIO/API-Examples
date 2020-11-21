#include "agorascreendlg.h"
#include "ui_agorascreendlg.h"
#include "config.h"
#include "language.h"
#include <QScreen>
#include <QDesktopWidget>
#include <QMessageBox>
class CScreenCaputreEventHandler : public agora::rtc::IRtcEngineEventHandler
{
	AgoraScreenDlg& m_pDlgScreen;
public:
	//set the message notify window handler
	CScreenCaputreEventHandler(AgoraScreenDlg& pDlgScreen)
		: m_pDlgScreen(pDlgScreen)
	{

	}

	/*
	note:
		Join the channel callback.This callback method indicates that the client
		successfully joined the specified channel.Channel ids are assigned based
		on the channel name specified in the joinChannel. If IRtcEngine::joinChannel
		is called without a user ID specified. The server will automatically assign one
	parameters:
		channel:channel name.
		uid: user ID¡£If the UID is specified in the joinChannel, that ID is returned here;
		Otherwise, use the ID automatically assigned by the Agora server.
		elapsed: The Time from the joinChannel until this event occurred (ms).
	*/
	virtual void onJoinChannelSuccess(const char* channel, agora::rtc::uid_t uid, int elapsed)
	{
		emit m_pDlgScreen.onJoinChannelSuccess(channel, (unsigned int)uid, elapsed);
	}

	/*
	note:
		When the App calls the leaveChannel method, the SDK indicates that the App
		has successfully left the channel. In this callback method, the App can get
		the total call time, the data traffic sent and received by THE SDK and other
		information. The App obtains the call duration and data statistics received
		or sent by the SDK through this callback.
	parameters:
		stats: Call statistics.
	*/
	//virtual void onLeaveChannel(const agora::rtc::RtcStats& stats) override;

private:
	HWND m_hMsgHanlder;
};


AgoraScreenDlg::AgoraScreenDlg(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AgoraScreenDlg)
{
    ui->setupUi(this);
    setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    hide();
	ui->labelWnd->hide();
	ui->cmbHwnd->hide();
	ui->btnWindow->hide();
	ui->labelChannel_3->hide();
	ui->cmbScreen->hide();
	ui->btnScreen->hide();
	ui->chkCursor->hide();

	ui->labelChannel->setText(commonCtrlChannel);
	ui->btnJoin->setText(commonCtrlJoinChannel);
	ui->labelFPS->setText(screenShareCtrlFPS);
	ui->labelBirate->setText(screenShareCtrlBitrate);
	ui->chkCursor->setText(screenShareCtrlShareCursor);
	ui->btnUpdateCapParam->setText(screenShareCtrlUpdateCaptureParam);
	connect(this, &AgoraScreenDlg::onJoinChannelSuccess, this, &AgoraScreenDlg::onJoinChannelSuccess_slot);
}

AgoraScreenDlg::~AgoraScreenDlg()
{
    delete ui;
}


bool AgoraScreenDlg::InitAgora()
{
	//create Agora RTC engine
	m_rtcEngine = createAgoraRtcEngine();
	if (!m_rtcEngine) {
		ui->listWidgetInfo->insertItem(ui->listWidgetInfo->count(), "createAgoraRtcEngine failed");
		return false;
	}
	//set message notify receiver window
	//m_eventHandler.SetMsgReceiver(m_hWnd);

	agora::rtc::RtcEngineContext context;
	std::string strAppID = GET_APP_ID;
	context.appId = strAppID.c_str();
	m_eventHandler = new CScreenCaputreEventHandler(*this);
	context.eventHandler = m_eventHandler;
	//initialize the Agora RTC engine context.  
	int ret = m_rtcEngine->initialize(context);
	if (ret != 0) {
		m_initialize = false;
		QString strInfo = QString("initialize failed: %d").arg(ret);
		ui->listWidgetInfo->insertItem(ui->listWidgetInfo->count(), strInfo);
		return false;
	}
	else
		m_initialize = true;
	ui->listWidgetInfo->insertItem(ui->listWidgetInfo->count(), "initialize success");

	//disable video in the engine.
	m_rtcEngine->enableLocalVideo(false);
	ui->listWidgetInfo->insertItem(ui->listWidgetInfo->count(), "enable video");
	//set channel profile in the engine to the CHANNEL_PROFILE_LIVE_BROADCASTING.
	m_rtcEngine->setChannelProfile(agora::CHANNEL_PROFILE_LIVE_BROADCASTING);
	ui->listWidgetInfo->insertItem(ui->listWidgetInfo->count(), "live broadcasting");
	//set client role in the engine to the CLIENT_ROLE_BROADCASTER.
	m_rtcEngine->setClientRole(agora::rtc::CLIENT_ROLE_BROADCASTER);
	ui->listWidgetInfo->insertItem(ui->listWidgetInfo->count(), "setClientRole broadcaster");

	//m_btnJoinChannel.EnableWindow(TRUE);

	QDesktopWidget *desktop = QApplication::desktop();
	int screen_count = desktop->screenCount();
	for (int i = 0; i < screen_count; ++i) {
		ui->cmbScreen->insertItem(i, QString("screen %1").arg(i));
	}

	ui->cmbScreen->setCurrentIndex(0);


	//return QGuiApplication::screens().size();
	return true;
}

void AgoraScreenDlg::UnInitAgora()
{
	if (m_rtcEngine) {
		m_rtcEngine->leaveChannel();
		//stop preview in the engine.
		m_rtcEngine->stopPreview();
		ui->listWidgetInfo->insertItem(ui->listWidgetInfo->count(), "stopPreview");
		
		//disable video in the engine.
		m_rtcEngine->disableVideo();
		ui->listWidgetInfo->insertItem(ui->listWidgetInfo->count(), "disableVideo");
		//release engine.
		m_rtcEngine->release(true);
		ui->listWidgetInfo->insertItem(ui->listWidgetInfo->count(), "release rtc engine");
		m_rtcEngine = NULL;
	}
}

void AgoraScreenDlg::onJoinChannelSuccess_slot(const char* channel, agora::rtc::uid_t uid, int elapsed)
{
	RenderLocalVideo();

	m_rtcEngine->startPreview();
	agora::rtc::ChannelMediaOptions op;
	op.publishAudioTrack = true;
	op.publishCameraTrack = false;
	op.publishCustomVideoTrack = false;
	op.publishScreenTrack = true;
	op.autoSubscribeAudio = false;
	op.autoSubscribeVideo = false;
	op.clientRoleType = agora::rtc::CLIENT_ROLE_TYPE::CLIENT_ROLE_BROADCASTER;
	op.channelProfile = agora::CHANNEL_PROFILE_LIVE_BROADCASTING;
	m_rtcEngine->updateChannelMediaOptions(op);
	m_rtcEngine->enableLocalVideo(false);
}

void AgoraScreenDlg::on_btnJoin_clicked()
{
	QString channelName = ui->lineEditChannel->text();
	if (channelName.isEmpty()) {
		QMessageBox::about(nullptr, "Screen Share", emptyChannelName);
		return;
	}

	on_btnScreen_clicked();
	if (!m_joinChannel) {
		m_rtcEngine->joinChannel("", channelName.toUtf8(), nullptr, 0);
		ui->btnJoin->setText(commonCtrlLeaveChannel);
	}
	else {
		m_rtcEngine->leaveChannel();
		ui->btnJoin->setText(commonCtrlJoinChannel);
	}
	m_joinChannel = !m_joinChannel;
}
void AgoraScreenDlg::on_btnWindow_clicked()
{

}
void AgoraScreenDlg::on_btnScreen_clicked()
{
	if (!m_bShareScreen) {
		agora::rtc::ScreenCaptureParameters capParam;
		QScreen* screen = QGuiApplication::screens().at(ui->cmbScreen->currentIndex());
		QRect rc = screen->geometry();
		agora::rtc::Rectangle screenRegion = { rc.x(), rc.y(), rc.width(), rc.height() };
		agora::rtc::Rectangle regionRect = screenRegion;
		capParam.dimensions.width = rc.width();
		capParam.dimensions.height = rc.height();
		int ret = m_rtcEngine->startScreenCaptureByScreenRect(screenRegion, regionRect, capParam);
		m_rtcEngine->startScreenCaptureByScreenRect(screenRegion, regionRect, capParam);
	}
	else {
		m_rtcEngine->stopScreenCapture();
	}
	m_bShareScreen = !m_bShareScreen;
}
void AgoraScreenDlg::on_btnUpdateCapParam_clicked()
{
	QScreen* screen = QGuiApplication::screens().at(ui->cmbScreen->currentIndex());
	QRect rc = screen->geometry();
	
	agora::rtc::ScreenCaptureParameters capParam;
	capParam.bitrate = ui->lineEditBirate->text().toInt();
	capParam.frameRate = ui->lineEditFPS->text().toInt();
	capParam.dimensions.width = rc.width();
	capParam.dimensions.height = rc.height();
	m_rtcEngine->updateScreenCaptureParameters(capParam);

}

void AgoraScreenDlg::showEvent(QShowEvent *event)
{
	
}

//render local video from SDK local capture.
void AgoraScreenDlg::RenderLocalVideo()
{
	if (m_rtcEngine) {

		agora::rtc::VideoCanvas canvas;
		canvas.renderMode = agora::media::base::RENDER_MODE_FIT;
		canvas.uid = 0;
		canvas.sourceType = agora::rtc::VIDEO_SOURCE_SCREEN;
		canvas.view = (void*)ui->labPreview->winId();
		//setup local video in the engine to canvas.
		m_rtcEngine->setupLocalVideo(canvas);
		ui->listWidgetInfo->insertItem(ui->listWidgetInfo->count(), "setupLocalVideo");
	}
}