//
//  MediaPlayer.swift
//  APIExample
//
//  Created by Arlin on 2022/3/15.
//  Copyright © 2022 Agora Corp. All rights reserved.
//

import Foundation
import AgoraRtcKit
import AGEVideoLayout

class MediaPlayer: BaseViewController {
    @IBOutlet weak var channelField: Input!
    @IBOutlet weak var joinChannelButton: NSButton!
    
    @IBOutlet weak var Container: AGEVideoContainer!
    @IBOutlet weak var selectResolutionPicker: Picker!
    @IBOutlet weak var selectFpsPicker: Picker!
    @IBOutlet weak var selectLayoutPicker: Picker!
    @IBOutlet weak var playButton: NSButton!
    @IBOutlet weak var pauseButton: NSButton!
    @IBOutlet weak var stopButton: NSButton!
    @IBOutlet weak var playAudioTrackButton: NSButton!
    @IBOutlet weak var pushAudioTrackButton: NSButton!
    @IBOutlet weak var startPublishButton: NSButton!
    @IBOutlet weak var stopPublishButton: NSButton!
    private var isPublishStream: Bool = false
    
    var videos: [VideoView] = []
    let layouts = [Layout("1v1", 2), Layout("1v3", 4), Layout("1v8", 9), Layout("1v15", 16)]
    
    var agoraKit: AgoraRtcEngineKit!
    var mediaPlayer: AgoraRtcMediaPlayerProtocol!
    let mediaUrl = "https://agora-adc-artifacts.s3.cn-north-1.amazonaws.com.cn/resources/sample.mp4";
    var playAudioTrackId: Int = 0
    var publishTrackIndex: Int = 0
    
    private var trackList: [AgoraRtcMediaStreamInfo]?
    
    var isJoinChannel: Bool = false {
        didSet {
            channelField.isEnabled = !isJoinChannel
            joinChannelButton.title = isJoinChannel ? "Leave Channel".localized : "Join Channel".localized
            playButton.isEnabled = isJoinChannel
            
            if !isJoinChannel {
                pauseButton.isEnabled = false
                stopButton.isEnabled = false
                playAudioTrackButton.isEnabled = false
                pushAudioTrackButton.isEnabled = false
                startPublishButton.isEnabled = false
                stopPublishButton.isEnabled = false
            }
        }
    }
    
    // MARK: - LifeCycle
    override func viewDidLoad() {
        super.viewDidLoad()
        setupUI()
        setupAgoraKit()
    }
    
    override func viewWillBeRemovedFromSplitView() {
        mediaPlayer.stop()
        agoraKit.leaveChannel(nil)
        AgoraRtcEngineKit.destroy()
    }
    
    func setupAgoraKit() {
        agoraKit = AgoraRtcEngineKit.sharedEngine(withAppId: KeyCenter.AppId, delegate: self)
        // Configuring Privatization Parameters
        Util.configPrivatization(agoraKit: agoraKit)
        agoraKit.setChannelProfile(.liveBroadcasting)
        agoraKit.setClientRole(.broadcaster)
        
        // setup media player and open online file
        let localVideo = videos[0]
        mediaPlayer = agoraKit.createMediaPlayer(with: self)
        mediaPlayer.setView(localVideo.videocanvas)
        mediaPlayer.setRenderMode(.fit)
        mediaPlayer.adjustPublishSignalVolume(80)
        mediaPlayer.adjustPlayoutVolume(80)
        let mediaSource = AgoraMediaSource()
        mediaSource.url = mediaUrl
        mediaSource.autoPlay = false
        mediaSource.enableMultiAudioTrack = true
        mediaPlayer.open(with: mediaSource)
    }
    
    // MARK: - UI
    func setupUI() {
        channelField.label.stringValue = "Channel".localized
        channelField.field.placeholderString = "Channel Name".localized
        joinChannelButton.title = isJoinChannel ? "Leave Channel".localized : "Join Channel".localized
        
        initSelectResolutionPicker()
        initSelectFpsPicker()
        initSelectLayoutPicker()
    }
    
    @IBAction func joinButtonPressed(_ sender: NSButton) {
        if !isJoinChannel {
            let channel = channelField.stringValue
            guard let resolution = selectedResolution(),
                  let fps = selectedFps(), !channel.isEmpty else {return}
            
            let videoConfig = AgoraVideoEncoderConfiguration()
            videoConfig.dimensions = resolution.size()
            videoConfig.frameRate = AgoraVideoFrameRate(rawValue: fps) ?? .fps15
            agoraKit.enableVideo()
            agoraKit.setVideoEncoderConfiguration(videoConfig)
 
            NetworkManager.shared.generateToken(channelName: channel, success: { token in
                let result = self.agoraKit.joinChannel(byToken: token, channelId: channel, info: nil, uid: 0)
                if result != 0 {
                    // Usually happens with invalid parameters
                    // Error code description can be found at:
                    // en: https://api-ref.agora.io/en/video-sdk/ios/4.x/documentation/agorartckit/agoraerrorcode
                    // cn: https://doc.shengwang.cn/api-ref/rtc/ios/error-code
                    self.showAlert(title: "Error", message: "joinChannel call failed: \(result), please check your params")
                }
            })
        } else {
            mediaPlayer.stop()
            agoraKit.leaveChannel { (stats:AgoraChannelStats) in
                self.isJoinChannel = false
                self.videos.forEach {
                    $0.uid = nil
                    $0.statsLabel.stringValue = ""
                }
                LogUtils.log(message: "Leave channel", level: .info)
            }
        }
    }
    
    @IBAction func playButtonPressed(_ sender: Any) {
        mediaPlayer.play()
        if mediaPlayer.getStreamCount() > 0 {
            trackList = (0...mediaPlayer.getStreamCount()).filter({
                mediaPlayer.getStreamBy(Int32($0))?.streamType == .audio
            }).compactMap({ mediaPlayer.getStreamBy(Int32($0)) })
            playAudioTrackId = trackList?.first?.streamIndex ?? 0
            publishTrackIndex = trackList?.first?.streamIndex ?? 0
            playAudioTrackButton.title = trackList?.first?.codecName ?? ""
            pushAudioTrackButton.title = playAudioTrackButton.title
        }
        playButton.isEnabled = false
        pauseButton.isEnabled = true
        stopButton.isEnabled = true
        if !isPublishStream {
            startPublishButton.isEnabled = true
            stopPublishButton.isEnabled = false
        }
        playAudioTrackButton.isEnabled = true
        pushAudioTrackButton.isEnabled = true
    }
    
    @IBAction func pauseButtonPressed(_ sender: Any) {
        mediaPlayer.pause()
        playButton.isEnabled = true
    }
    
    @IBAction func stopButtonPressed(_ sender: Any) {
        mediaPlayer.stop()
        mediaPlayer.open(mediaUrl, startPos: 0)
        playAudioTrackButton.isEnabled = false
        pushAudioTrackButton.isEnabled = false
        playButton.isEnabled = true
        pauseButton.isEnabled = false
        stopButton.isEnabled = false
        startPublishButton.isEnabled = false
        stopPublishButton.isEnabled = false
        stopPublishHandler()
        isPublishStream = false
    }
    
    @IBAction func publishButtonPressed(_ sender: Any) {
        if !isJoinChannel {
            self.showAlert(title: "Alert", message: "Join channel first")
            return
        }
        
        let options = AgoraRtcChannelMediaOptions()
        options.publishCameraTrack = false
        options.publishMediaPlayerVideoTrack = true
        options.publishMediaPlayerAudioTrack = true
        options.publishMediaPlayerId = Int(mediaPlayer.getMediaPlayerId())
        agoraKit.updateChannel(with: options)
        startPublishButton.isEnabled = false
        stopPublishButton.isEnabled = true
        isPublishStream = true
    }
    
    @IBAction func unPublishButtonPressed(_ sender: Any) {
        if !isJoinChannel {
            self.showAlert(title: "Alert", message: "Join channel first")
            return
        }
        stopPublishHandler()
        startPublishButton.isEnabled = true
        stopPublishButton.isEnabled = false
        isPublishStream = false
    }
    
    private func stopPublishHandler() {
        let options = AgoraRtcChannelMediaOptions()
        options.publishCameraTrack = true
        options.publishMediaPlayerVideoTrack = false
        options.publishMediaPlayerAudioTrack = false
        agoraKit.updateChannel(with: options)
    }
    
    @IBAction func onClickPlayAudioTrack(_ sender: NSButton) {
        let menu = NSMenu()
        menu.autoenablesItems = false
        trackList?.compactMap({ $0.codecName }).forEach({
            menu.addItem(withTitle: $0, action: #selector(playItemOption(_:)), keyEquivalent: "")
        })
        sender.menu = menu
        menu.popUp(positioning: nil, at: NSPoint(x: sender.bounds.width, y: 0), in: sender)
    }
    
    @IBAction func onClickPushAudioTrack(_ sender: NSButton) {
        let menu = NSMenu()
        menu.autoenablesItems = false
        trackList?.compactMap({ $0.codecName }).forEach({
            menu.addItem(withTitle: $0, action: #selector(pushItemOption(_:)), keyEquivalent: "")
        })
        sender.menu = menu
        menu.popUp(positioning: nil, at: NSPoint(x: sender.bounds.width, y: 0), in: sender)
    }
    @objc func playItemOption(_ sender: NSMenuItem) {
        playAudioTrackButton.title = sender.title
        guard let track = trackList?.filter({ $0.codecName == sender.title }).first else { return }
        mediaPlayer.selectMultiAudioTrack(track.streamIndex, publishTrackIndex: publishTrackIndex)
        playAudioTrackId = track.streamIndex
    }
    
    @objc func pushItemOption(_ sender: NSMenuItem) {
        pushAudioTrackButton.title = sender.title
        guard let track = trackList?.filter({ $0.codecName == sender.title }).first else { return }
        mediaPlayer.selectMultiAudioTrack(playAudioTrackId, publishTrackIndex: track.streamIndex)
        publishTrackIndex = track.streamIndex
    }
}

extension MediaPlayer: AgoraRtcEngineDelegate {
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinChannel channel: String, withUid uid: UInt, elapsed: Int) {
        isJoinChannel = true
        videos[0].uid = uid
        LogUtils.log(message: "Join \(channel) with uid \(uid) elapsed \(elapsed)ms", level: .info)
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinedOfUid uid: UInt, elapsed: Int) {
        LogUtils.log(message: "remote user join: \(uid) \(elapsed)ms", level: .info)
        
        // find a VideoView w/o uid assigned
        if let remoteVideo = videos.first(where: { $0.uid == nil }) {
            remoteVideo.uid = uid
            
            let videoCanvas = AgoraRtcVideoCanvas()
            videoCanvas.uid = uid
            videoCanvas.view = remoteVideo.videocanvas
            agoraKit.setupRemoteVideo(videoCanvas)
        } else {
            LogUtils.log(message: "no video canvas available for \(uid), cancel bind", level: .warning)
        }
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurError errorCode: AgoraErrorCode) {
        LogUtils.log(message: "error: \(errorCode)", level: .error)
        self.showAlert(title: "Error", message: "Error \(errorCode.rawValue) occur")
    }

    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurWarning warningCode: AgoraWarningCode) {
        LogUtils.log(message: "warning: \(warningCode.rawValue)", level: .warning)
    }
}

extension MediaPlayer: AgoraRtcMediaPlayerDelegate {
    func AgoraRtcMediaPlayer(_ playerKit: AgoraRtcMediaPlayerProtocol, didChangedTo state: AgoraMediaPlayerState, reason: AgoraMediaPlayerReason) {
        print("didChangedTo: \(state.rawValue), \(reason.rawValue)")
        switch state {
        case .openCompleted:
            print("OpenCompleted")
        case .playing:
            print("Playing")
        case .playBackAllLoopsCompleted:
            print("PlayBackAllLoopsCompleted")
        case .stopped:
            print("Stopped")
        default: break
        }
    }
}

// MARK: - View Setting
private extension MediaPlayer {
    func initSelectResolutionPicker() {
        selectResolutionPicker.label.stringValue = "Resolution".localized
        selectResolutionPicker.picker.addItems(withTitles: Configs.Resolutions.map { $0.name() })
        selectResolutionPicker.picker.selectItem(at: GlobalSettings.shared.resolutionSetting.selectedOption().value)
        
        selectResolutionPicker.onSelectChanged {
            if !self.isJoinChannel {
                return
            }
            guard let resolution = self.selectedResolution(),
                  let fps = self.selectedFps() else {return}
            let videoConfig = AgoraVideoEncoderConfiguration()
            videoConfig.dimensions = resolution.size()
            videoConfig.frameRate = AgoraVideoFrameRate(rawValue: fps) ?? .fps15
            self.agoraKit.setVideoEncoderConfiguration(videoConfig)
        }
    }
    
    func selectedResolution() -> Resolution? {
        let index = self.selectResolutionPicker.indexOfSelectedItem
        if index >= 0 && index < Configs.Resolutions.count {
            return Configs.Resolutions[index]
        } else {
            return nil
        }
    }
    
    func initSelectFpsPicker() {
        selectFpsPicker.label.stringValue = "Frame Rate".localized
        selectFpsPicker.picker.addItems(withTitles: Configs.Fps.map { "\($0)fps" })
        selectFpsPicker.picker.selectItem(at: GlobalSettings.shared.fpsSetting.selectedOption().value)
        
        selectFpsPicker.onSelectChanged {
            if !self.isJoinChannel {
                return
            }
            guard let resolution = self.selectedResolution(),
                  let fps = self.selectedFps() else {return}
            let videoConfig = AgoraVideoEncoderConfiguration()
            videoConfig.dimensions = resolution.size()
            videoConfig.frameRate = AgoraVideoFrameRate(rawValue: fps) ?? .fps15
            self.agoraKit.setVideoEncoderConfiguration(videoConfig)
        }
    }
    
    func selectedFps() -> Int? {
        let index = self.selectFpsPicker.indexOfSelectedItem
        if index >= 0 && index < Configs.Fps.count {
            return Configs.Fps[index]
        } else {
            return nil
        }
    }
    
    func initSelectLayoutPicker() {
        selectLayoutPicker.isEnabled = false
        layoutVideos(2)
        selectLayoutPicker.label.stringValue = "Layout".localized
        selectLayoutPicker.picker.addItems(withTitles: layouts.map { $0.label })
        selectLayoutPicker.onSelectChanged {
            if self.isJoinChannel {
                return
            }
            guard let layout = self.selectedLayout() else { return }
            self.layoutVideos(layout.value)
        }
    }
    
    func selectedLayout() ->Layout? {
        let index = self.selectLayoutPicker.indexOfSelectedItem
        if index >= 0 && index < layouts.count {
            return layouts[index]
        } else {
            return nil
        }
    }
    
    func layoutVideos(_ count: Int) {
        videos = []
        for i in 0...count - 1 {
            let view = VideoView.createFromNib()!
            if(i == 0) {
                view.placeholder.stringValue = "Local"
                view.type = .local
                view.statsInfo = StatisticsInfo(type: .local(StatisticsInfo.LocalInfo()))
            } else {
                view.placeholder.stringValue = "Remote \(i)"
                view.type = .remote
                view.statsInfo = StatisticsInfo(type: .remote(StatisticsInfo.RemoteInfo()))
            }
            videos.append(view)
        }
        // layout render view
        Container.layoutStream(views: videos)
    }
}
