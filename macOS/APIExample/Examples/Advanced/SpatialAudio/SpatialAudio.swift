//
//  SpatialAudio.swift
//  APIExample
//
//  Created by Arlin on 2022/3/23.
//  Copyright © 2022 Agora Corp. All rights reserved.
//

import Foundation
import AgoraRtcKit

class SpatialAudioMain: BaseViewController {
    @IBOutlet weak var infoLabel: NSTextField!
    @IBOutlet weak var selfPostionView: NSImageView!
    
    var agoraKit: AgoraRtcEngineKit?
    var mediaPlayer1: AgoraRtcMediaPlayerProtocol?
    var mediaPlayer2: AgoraRtcMediaPlayerProtocol?
    var localSpatial: AgoraLocalSpatialAudioKit?
    @IBOutlet weak var voice1Button: NSButton!
    @IBOutlet weak var voice2Button: NSButton!
    @IBOutlet weak var voice1ContainerView: NSBox!
    @IBOutlet weak var remoteUser1: NSButton!
    @IBOutlet weak var remoteUser2: NSButton!
    @IBOutlet weak var inputChannelTextField: NSTextField!
    @IBOutlet weak var audioSettingView1: NSBox!
    @IBOutlet weak var audioSettingView2: NSBox!
    @IBOutlet weak var audioAttenuationSwitch: NSSwitch!
    @IBOutlet weak var micSwitch: NSSwitch!
    @IBOutlet weak var joinChannelButton: NSButton!
    
    var currentAngle = 0.0
    var currentDistance = 0.0
    var maxDistance: CGFloat = 10
    let forward = simd_float3(1.0, 0.0, 0.0)
    let right = simd_float3(0.0, 1.0, 0.0)
    let up = simd_float3(0.0, 0.0, 1.0)
    var isJoind: Bool = false {
        didSet {
            voice1Button.isEnabled = isJoind
            voice2Button.isEnabled = isJoind
            audioAttenuationSwitch.isEnabled = isJoind
            micSwitch.isEnabled = isJoind
            if audioSettingView1.isHidden == false {
                audioSettingView1.isHidden = true
            }
            if audioSettingView2.isHidden == false {
                audioSettingView2.isHidden = true
            }
            isJoind ? (joinChannelButton.title = "离开频道") : (joinChannelButton.title = "加入频道")
        }
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        self.setupUI()
    }
    
    override func viewWillBeRemovedFromSplitView() {
        AgoraLocalSpatialAudioKit.destroy()
        AgoraRtcEngineKit.destroy()
    }

    func setupUI() {
        infoLabel.stringValue = "Please move the red icon to experience the 3D audio effect".localized
        voice1ContainerView.isHidden = true
        remoteUser1.isHidden = true
        remoteUser2.isHidden = true
        audioSettingView1.isHidden = true
        audioSettingView2.isHidden = true
        let panGesture = NSPanGestureRecognizer(target: self, action: #selector(panGestureChanged))
        selfPostionView.addGestureRecognizer(panGesture)
    }
    
    
    @IBAction func startBtnClicked(_ sender: NSButton) {
        if isJoind {
            isJoind = false
            mediaPlayer1?.stop()
            mediaPlayer2?.stop()
            AgoraLocalSpatialAudioKit.destroy()
            agoraKit?.leaveChannel(nil)
            return
        }
        agoraKit = AgoraRtcEngineKit.sharedEngine(withAppId: KeyCenter.AppId, delegate: self)
        // Configuring Privatization Parameters
        Util.configPrivatization(agoraKit: agoraKit)
        agoraKit?.setChannelProfile(.liveBroadcasting)
        agoraKit?.setClientRole(.broadcaster)

        agoraKit?.setAudioProfile(.default)
        agoraKit?.setAudioScenario(.gameStreaming)
        
        let localSpatialConfig = AgoraLocalSpatialAudioConfig()
        localSpatialConfig.rtcEngine = agoraKit
        localSpatial = AgoraLocalSpatialAudioKit.sharedLocalSpatialAudio(with: localSpatialConfig)
        localSpatial?.muteLocalAudioStream(false)
        localSpatial?.muteAllRemoteAudioStreams(false)
        localSpatial?.setAudioRecvRange(Float(view.frame.height))
        localSpatial?.setMaxAudioRecvCount(2)
        localSpatial?.setDistanceUnit(1)
                
        let channelName = inputChannelTextField.stringValue
        let option = AgoraRtcChannelMediaOptions()
        option.publishMicrophoneTrack = true
        option.autoSubscribeAudio = true
        NetworkManager.shared.generateToken(channelName: channelName, success: { token in
            let result = self.agoraKit?.joinChannel(byToken: token,
                                                    channelId: channelName,
                                                    uid: 0,
                                                    mediaOptions: option,
                                                    joinSuccess: nil)
            if result != 0 {
                print("join channel fail")
            }
            self.isJoind = true
            DispatchQueue.main.asyncAfter(deadline: .now() + 0.25) {
                self.openMusic()
                self.updatePosition()
            }
        })
    }
    
    private func openMusic() {
        mediaPlayer1 = agoraKit?.createMediaPlayer(with: self)
        mediaPlayer1?.setLoopCount(10000)
        mediaPlayer1?.open("https://webdemo.agora.io/audiomixing.mp3", startPos: 0)
        localSpatial?.updatePlayerPositionInfo(Int(mediaPlayer1?.getMediaPlayerId() ?? 0), positionInfo: getPlayerPostion(view: voice1Button))
        localSpatial?.setPlayerAttenuation(0.1, playerId: UInt(mediaPlayer1?.getMediaPlayerId() ?? 0), forceSet: false)
        
        mediaPlayer2 = agoraKit?.createMediaPlayer(with: self)
        mediaPlayer2?.setLoopCount(10000)
        mediaPlayer2?.open("https://webdemo.agora.io/dang.mp3", startPos: 0)
        localSpatial?.updatePlayerPositionInfo(Int(mediaPlayer2?.getMediaPlayerId() ?? 0), positionInfo: getPlayerPostion(view: voice2Button))
        localSpatial?.setPlayerAttenuation(0.1, playerId: UInt(mediaPlayer2?.getMediaPlayerId() ?? 0), forceSet: false)
    }
    
    @objc func panGestureChanged(gesture: NSPanGestureRecognizer) {
        let move = gesture.translation(in: self.view)
        var objectCenter = CGPoint(x: NSMidX(gesture.view!.frame), y: NSMidY(gesture.view!.frame))
        objectCenter = CGPoint(x: objectCenter.x + move.x, y: objectCenter.y + move.y)
        
        let width = selfPostionView.frame.size.width
        selfPostionView.frame = CGRect(origin: CGPoint(x: objectCenter.x - width / 2.0, y: objectCenter.y - width / 2.0), size: CGSize(width: width, height: width))
        gesture.setTranslation(.zero, in: self.view)
  
        if gesture.state == .ended {
            updatePosition()
        }
    }
    
    func updatePosition() {
        let pos = getViewCenterPostion(view: selfPostionView)
        localSpatial?.updateSelfPosition(pos, axisForward: forward, axisRight: right, axisUp: up)
    }
    private func getPlayerPostion(view: NSView) -> AgoraRemoteVoicePositionInfo {
        let position = getViewCenterPostion(view: view)
        let positionInfo = AgoraRemoteVoicePositionInfo()
        positionInfo.position = position
        positionInfo.forward = forward
        return positionInfo
    }
    private func getViewCenterPostion(view: NSView) -> simd_float3 {
        let centerX = view.frame.origin.x + view.frame.width * 0.5
        let centerY = view.frame.origin.y + view.frame.height * 0.5
        return simd_float3(Float(centerX), Float(centerY), 0.0)
    }
    
    @IBAction func onTapVoice1Button(_ sender: Any) {
        audioSettingView2.isHidden = true
        audioSettingView1.isHidden = false
    }
    @IBAction func onTapVoice2Button(_ sender: Any) {
        audioSettingView2.isHidden = false
        audioSettingView1.isHidden = true
    }
    @IBAction func onTapMicSwitch(_ sender: NSSwitch) {
        let option = AgoraRtcChannelMediaOptions()
        option.publishMicrophoneTrack = sender.state == .on
        agoraKit?.updateChannel(with: option)
    }
    @IBAction func onTapAudioAttenuationSwitch(_ sender: NSSwitch) {
        voice1ContainerView.isHidden = sender.state != .on
        if sender.state == .on {
            let audioZone = AgoraSpatialAudioZone()
            audioZone.forwardLength = Float(voice1ContainerView.frame.width)
            audioZone.rightLength = Float(voice1ContainerView.frame.height)
            audioZone.upLength = Float(maxDistance)
            audioZone.zoneSetId = 1
            audioZone.audioAttenuation = 1
            audioZone.forward = forward
            audioZone.right = right
            audioZone.up = up
            audioZone.position = getViewCenterPostion(view: voice1ContainerView)
            localSpatial?.setZones([audioZone])
        } else {
            let audioZone = AgoraSpatialAudioZone()
            audioZone.forwardLength = Float(view.frame.height)
            audioZone.rightLength = Float(view.frame.width)
            audioZone.upLength = Float(maxDistance)
            localSpatial?.setZones([audioZone])
        }
        let pos = getViewCenterPostion(view: selfPostionView)
        localSpatial?.updateSelfPosition(pos, axisForward: forward, axisRight: right, axisUp: up)
    }
    @IBAction func onTapMuteSwitch(_ sender: NSSwitch) {
        if audioSettingView1.isHidden == false {
            _ = sender.state == .on ? mediaPlayer1?.pause() : mediaPlayer1?.resume()
        } else {
            _ = sender.state == .on ? mediaPlayer2?.pause() : mediaPlayer2?.resume()
        }
    }
    @IBAction func onTapVoiceBlurSwitch(_ sender: NSSwitch) {
        let params = AgoraSpatialAudioParams()
        params.enable_blur = sender.state == .on
        if audioSettingView1.isHidden == false {
            mediaPlayer1?.setSpatialAudioParams(params)
        } else {
            mediaPlayer2?.setSpatialAudioParams(params)
        }
    }
    @IBAction func onTapAirborneSwitch(_ sender: NSSwitch) {
        let params = AgoraSpatialAudioParams()
        params.enable_air_absorb = sender.state == .on
        if audioSettingView1.isHidden == false {
            mediaPlayer1?.setSpatialAudioParams(params)
        } else {
            mediaPlayer2?.setSpatialAudioParams(params)
        }
    }
    @IBAction func onTapAttenuationSlider(_ sender: NSSlider) {
        if audioSettingView1.isHidden == false {
            localSpatial?.setPlayerAttenuation(sender.doubleValue, playerId: UInt(mediaPlayer1?.getMediaPlayerId() ?? 0), forceSet: false)
        } else {
            localSpatial?.setPlayerAttenuation(sender.doubleValue, playerId: UInt(mediaPlayer2?.getMediaPlayerId() ?? 0), forceSet: false)
        }
    }
    
}

extension SpatialAudioMain: AgoraRtcEngineDelegate {
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinedOfUid uid: UInt, elapsed: Int) {
        LogUtils.log(message: "remote user join: \(uid) \(elapsed)ms", level: .info)
        if remoteUser1.tag <= 0 {
            remoteUser1.title = "\(uid)"
            remoteUser1.tag = Int(uid)
            remoteUser1.isHidden = false
        } else if remoteUser2.tag <= 0 {
            remoteUser2.title = "\(uid)"
            remoteUser2.tag = Int(uid)
            remoteUser2.isHidden = false
        }
    }
    /// callback when a remote user is leaving the channel, note audience in live broadcast mode will NOT trigger this event
    /// @param uid uid of remote joined user
    /// @param reason reason why this user left, note this event may be triggered when the remote user
    /// become an audience in live broadcasting profile
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOfflineOfUid uid: UInt, reason: AgoraUserOfflineReason) {
        LogUtils.log(message: "remote user left: \(uid) reason \(reason)", level: .info)
        
        if remoteUser1.tag == uid {
            remoteUser1.isHidden = true
            remoteUser1.tag = 0
        } else if remoteUser2.tag == uid {
            remoteUser2.isHidden = true
            remoteUser2.tag = 0
        }
    }
}

extension SpatialAudioMain: AgoraRtcMediaPlayerDelegate {
    func AgoraRtcMediaPlayer(_ playerKit: AgoraRtcMediaPlayerProtocol, didChangedTo state: AgoraMediaPlayerState, reason: AgoraMediaPlayerReason) {
        print("didChangedTo: \(state.rawValue), \(reason.rawValue)")
        if state == .openCompleted || state == .playBackAllLoopsCompleted || state == .playBackCompleted {
            playerKit.play()
        }
    }
}


