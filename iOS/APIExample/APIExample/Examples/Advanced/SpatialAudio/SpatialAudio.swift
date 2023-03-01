//
//  SpatialAudio.swift
//  APIExample
//
//  Created by Arlin on 2022/3/1.
//  Copyright © 2022 Agora Corp. All rights reserved.
//

import UIKit
import Foundation
import AgoraRtcKit

class SpatialAudioEntry : UIViewController
{
    @IBOutlet weak var channelTextField: UITextField!

    @IBAction func joinBtnClicked(sender: UIButton) {
        guard let channelName = channelTextField.text,
              channelName.lengthOfBytes(using: .utf8) > 0 else {return}
        channelTextField.resignFirstResponder()
        
        let identifier = "SpatialAudio"
        let storyBoard: UIStoryboard = UIStoryboard(name: identifier, bundle: nil)
        guard let newViewController = storyBoard.instantiateViewController(withIdentifier: identifier) as? BaseViewController else {return}
        newViewController.title = channelName
        newViewController.configs = ["channelName": channelName]
        navigationController?.pushViewController(newViewController, animated: true)
    }
}

let SCREENSIZE: CGSize = UIScreen.main.bounds.size
class SpatialAudioMain: BaseViewController {
    @IBOutlet weak var infoLabel: UILabel!
    @IBOutlet weak var selfPostionView: UIImageView!
    @IBOutlet weak var voiceContainerView1: UIView!
    @IBOutlet weak var voiceContainerView2: UIView!
    @IBOutlet weak var voiceButton1: UIButton!
    @IBOutlet weak var voiceButton2: UIButton!
    @IBOutlet weak var remoteUserButton1: UIButton!
    @IBOutlet weak var remoteUserButton2: UIButton!
    
    private var isJoined: Bool = false
    private lazy var actionView1 = SpatialAudioActionSheet()
    private lazy var actionView2 = SpatialAudioActionSheet()
    var agoraKit: AgoraRtcEngineKit!
    var mediaPlayer1: AgoraRtcMediaPlayerProtocol!
    var mediaPlayer2: AgoraRtcMediaPlayerProtocol!
    var localSpatial: AgoraLocalSpatialAudioKit!
    
    var currentAngle = 0.0
    var currentDistance = 0.0
    var maxDistance: CGFloat = 10
    let forward = [NSNumber(1.0), NSNumber(0.0), NSNumber(0.0)]
    let right = [NSNumber(0.0), NSNumber(1.0), NSNumber(0.0)]
    let up = [NSNumber(0.0), NSNumber(0.0), NSNumber(1.0)]
    
    override func viewDidLoad() {
        super.viewDidLoad()
        self.setupUI()
        
        agoraKit = AgoraRtcEngineKit.sharedEngine(withAppId: KeyCenter.AppId, delegate: self)
        // Configuring Privatization Parameters
        Util.configPrivatization(agoraKit: agoraKit)
        agoraKit.setChannelProfile(.liveBroadcasting)
        agoraKit.setClientRole(GlobalSettings.shared.getUserRole())
        agoraKit.muteAllRemoteAudioStreams(true)
        agoraKit.setParameters("{\"rtc.enable_debug_log\":true}")
        agoraKit.setLogFile(LogUtils.sdkLogPath())
        agoraKit.enableAudio()
        agoraKit.setEnableSpeakerphone(true)
        
        let localSpatialConfig = AgoraLocalSpatialAudioConfig()
        localSpatialConfig.rtcEngine = agoraKit
        localSpatial = AgoraLocalSpatialAudioKit.sharedLocalSpatialAudio(with: localSpatialConfig)
        localSpatial.muteLocalAudioStream(false)
        localSpatial.muteAllRemoteAudioStreams(false)
        localSpatial.setAudioRecvRange(Float(SCREENSIZE.height))
        localSpatial.setMaxAudioRecvCount(2)
        localSpatial.setDistanceUnit(1)
    }
    
    override func viewDidAppear(_ animated: Bool) {
        super.viewDidAppear(animated)
        guard isJoined == false else { return }
        mediaPlayer1 = agoraKit.createMediaPlayer(with: self)
        mediaPlayer1.setLoopCount(10000)
        mediaPlayer1.open("https://webdemo.agora.io/audiomixing.mp3", startPos: 0)
        localSpatial.updatePlayerPositionInfo(Int(mediaPlayer1.getMediaPlayerId()), positionInfo: getPlayerPostion(view: voiceButton1))
        localSpatial.setPlayerAttenuation(0.2, playerId: UInt(mediaPlayer1.getMediaPlayerId()), forceSet: false)
        
        mediaPlayer2 = agoraKit.createMediaPlayer(with: self)
        mediaPlayer2.setLoopCount(10000)
        mediaPlayer2.open("https://webdemo.agora.io/dang.mp3", startPos: 0)
        localSpatial.updatePlayerPositionInfo(Int(mediaPlayer2.getMediaPlayerId()), positionInfo: getPlayerPostion(view: voiceButton2))
        localSpatial.setPlayerAttenuation(0.2, playerId: UInt(mediaPlayer2.getMediaPlayerId()), forceSet: false)
        
        updatePosition()
        
        joinChannel()
    }
    
    override func viewDidDisappear(_ animated: Bool) {
        super.viewDidDisappear(animated)
        agoraKit = nil
        AgoraLocalSpatialAudioKit.destroy()
        AgoraRtcEngineKit.destroy()
        isJoined = false
    }
    
    private func joinChannel() {
        guard let channelName = configs["channelName"] as? String else {return}
        let option = AgoraRtcChannelMediaOptions()
        option.publishMicrophoneTrack = true
        option.autoSubscribeAudio = true
        NetworkManager.shared.generateToken(channelName: channelName, success: { token in
            let result = self.agoraKit.joinChannel(byToken: token, channelId: channelName, uid: 0, mediaOptions: option, joinSuccess: nil)
            if result != 0 {
                print("join channel fail")
            }
            self.isJoined = true
        })
    }
    
    func setupUI() {
        infoLabel.text = "Please move the red icon to experience the 3D audio effect".localized
        voiceButton1.setTitle("", for: .normal)
        voiceButton2.setTitle("", for: .normal)
        voiceButton1.setImage(UIImage(named: "spatial_sound2")?.withRenderingMode(.alwaysOriginal), for: .normal)
        voiceButton2.setImage(UIImage(named: "spatial_sound2")?.withRenderingMode(.alwaysOriginal), for: .normal)
        voiceContainerView2.isHidden = true
        voiceContainerView1.isHidden = true
        remoteUserButton1.isHidden = true
        remoteUserButton2.isHidden = true
        let panGesture = UIPanGestureRecognizer(target: self, action: #selector(panGestureChanged))
        selfPostionView.addGestureRecognizer(panGesture)
    }
        
    @objc func panGestureChanged(gesture: UIPanGestureRecognizer) {
        let move = gesture.translation(in: self.view)
        var objectCenter = gesture.view!.center
        objectCenter = CGPoint(x: objectCenter.x + move.x, y: objectCenter.y + move.y)
        selfPostionView.center = objectCenter
        gesture.setTranslation(.zero, in: self.view)
        if gesture.state == .ended {
            updatePosition()
        }
    }
    @IBAction func onTapVoice1Button(_ sender: Any) {
        updateMediaPlayerParams(mediaPlayer: mediaPlayer1, actionView: actionView1)
    }
    @IBAction func onTapVoice2Button(_ sender: Any) {
        updateMediaPlayerParams(mediaPlayer: mediaPlayer2, actionView: actionView2)
    }
    @IBAction func onTapRemoteUser1Button(_ sender: Any) {
    }
    @IBAction func onTapRemoteUser2Button(_ sender: Any) {
    }
    @IBAction func onTapMicSwitch(_ sender: UISwitch) {
        let option = AgoraRtcChannelMediaOptions()
        option.publishMicrophoneTrack = sender.isOn
        agoraKit.updateChannel(with: option)
    }
    @IBAction func onTapAudioAttenuationSwitch(_ sender: UISwitch) {
        voiceContainerView1.isHidden = !sender.isOn
        if sender.isOn {
            let audioZone = AgoraSpatialAudioZone()
            audioZone.forwardLength = Float(voiceContainerView1.frame.width)
            audioZone.rightLength = Float(voiceContainerView1.frame.height)
            audioZone.upLength = Float(maxDistance)
            audioZone.zoneSetId = 1
            audioZone.audioAttenuation = 1
            audioZone.forward = forward
            audioZone.right = right
            audioZone.up = up
            audioZone.position = getViewCenterPostion(view: voiceContainerView1)
            localSpatial.setZones([audioZone])
        } else {
            let audioZone = AgoraSpatialAudioZone()
            audioZone.forwardLength = Float(SCREENSIZE.height)
            audioZone.rightLength = Float(SCREENSIZE.width)
            audioZone.upLength = Float(maxDistance)
            localSpatial.setZones([audioZone])
        }
        let pos = getViewCenterPostion(view: selfPostionView)
        localSpatial.updateSelfPosition(pos, axisForward: forward, axisRight: right, axisUp: up)
    }
    
    private func updateMediaPlayerParams(mediaPlayer: AgoraRtcMediaPlayerProtocol,
                                         actionView: SpatialAudioActionSheet) {
        actionView.onTapMuteSwitchClosure = { isPause in
            _ = isPause ? mediaPlayer.pause() : mediaPlayer.resume()
        }
        actionView.onTapVoiceBlurSwitchClosure = { isOn in
            let params = AgoraSpatialAudioParams()
            params.enable_blur = isOn
            mediaPlayer.setSpatialAudioParams(params)
        }
        actionView.onTapAirborneSwitchClosure = { isOn in
            let params = AgoraSpatialAudioParams()
            params.enable_air_absorb = isOn
            mediaPlayer.setSpatialAudioParams(params)
        }
        actionView.onTapAttenuationSilderClosure = { [weak self] value in
            guard let self = self else { return }
            self.localSpatial.setPlayerAttenuation(Double(value), playerId: UInt(mediaPlayer.getMediaPlayerId()), forceSet: false)
        }
        AlertManager.show(view: actionView, alertPostion: .bottom, didCoverDismiss: true)
    }
    
    func updatePosition() {
        let pos = getViewCenterPostion(view: selfPostionView)
        localSpatial.updateSelfPosition(pos, axisForward: forward, axisRight: right, axisUp: up)
    }
    
    private func getPlayerPostion(view: UIView) -> AgoraRemoteVoicePositionInfo {
        let position = getViewCenterPostion(view: view)
        print("player postion == \(position)")
        let positionInfo = AgoraRemoteVoicePositionInfo()
        positionInfo.position = position
        positionInfo.forward = forward
        return positionInfo
    }
    private func getViewCenterPostion(view: UIView) -> [NSNumber] {
        [NSNumber(value: Double(view.center.x)), NSNumber(value: Double(view.center.y)), NSNumber(0.0)]
    }
    
    private func updateSpatialAngle(objectCenter: CGPoint) -> AgoraRemoteVoicePositionInfo {
        let circleCenter = view.center
        let deltaX = objectCenter.x - circleCenter.x
        let deltaY = circleCenter.y - objectCenter.y
        let R = sqrt(deltaX * deltaX + deltaY * deltaY)
        
        // In spatial audio, it is start at Y direction with clockwise
        let TwoPI = Double.pi * 2.0
        let cosAngle = acos(deltaX / R)
        let mathAngle = deltaY > 0 ? cosAngle : (TwoPI - cosAngle)
        var spatialAngle = TwoPI + TwoPI / 4.0 - mathAngle
        if spatialAngle > TwoPI {
            spatialAngle = spatialAngle - TwoPI
        }
        
        currentAngle = spatialAngle
        currentDistance = R
        
        let maxR = UIScreen.main.bounds.height / 2.0
        let maxSpatailDistance = 30.0
        let spatialDistance = currentDistance * maxSpatailDistance / maxR

        let posForward = spatialDistance * cos(currentAngle);
        let posRight = spatialDistance * sin(currentAngle);
        let position = [NSNumber(value: posForward), NSNumber(value: posRight), NSNumber(0.0)]
        let forward = [NSNumber(1.0), NSNumber(0.0), NSNumber(0.0)]
        
        let positionInfo = AgoraRemoteVoicePositionInfo()
        positionInfo.position = position
        positionInfo.forward = forward
        
        return positionInfo
    }
}
    
extension SpatialAudioMain: AgoraRtcEngineDelegate {
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinedOfUid uid: UInt, elapsed: Int) {
        LogUtils.log(message: "remote user join: \(uid) \(elapsed)ms", level: .info)
        if remoteUserButton1.tag <= 0 {
            remoteUserButton1.setTitle("\(uid)", for: .normal)
            remoteUserButton1.tag = Int(uid)
            remoteUserButton1.isHidden = false
        } else if remoteUserButton2.tag <= 0 {
            remoteUserButton2.setTitle("\(uid)", for: .normal)
            remoteUserButton2.tag = Int(uid)
            remoteUserButton2.isHidden = false
        }
    }
   
    /// callback when a remote user is leaving the channel, note audience in live broadcast mode will NOT trigger this event
    /// @param uid uid of remote joined user
    /// @param reason reason why this user left, note this event may be triggered when the remote user
    /// become an audience in live broadcasting profile
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOfflineOfUid uid: UInt, reason: AgoraUserOfflineReason) {
        LogUtils.log(message: "remote user left: \(uid) reason \(reason)", level: .info)
        
        if remoteUserButton1.tag == uid {
            remoteUserButton1.isHidden = true
            remoteUserButton1.tag = 0
        } else if remoteUserButton2.tag == uid {
            remoteUserButton2.isHidden = true
            remoteUserButton2.tag = 0
        }
    }
}

extension SpatialAudioMain: AgoraRtcMediaPlayerDelegate {
    func AgoraRtcMediaPlayer(_ playerKit: AgoraRtcMediaPlayerProtocol, didChangedTo state: AgoraMediaPlayerState, error: AgoraMediaPlayerError) {
        print("didChangedTo: \(state.rawValue), \(error.rawValue)")
        if state == .openCompleted || state == .playBackAllLoopsCompleted || state == .playBackCompleted {
            playerKit.play()
        }
    }
}


class SpatialAudioActionSheet: UIView {
    var onTapMuteSwitchClosure: ((Bool) -> Void)?
    var onTapVoiceBlurSwitchClosure: ((Bool) -> Void)?
    var onTapAirborneSwitchClosure: ((Bool) -> Void)?
    var onTapAttenuationSilderClosure: ((Float) -> Void)?
    private lazy var muteLabel: UILabel = {
        let label = UILabel()
        label.text = "静音".localized
        return label
    }()
    private lazy var voiceBlurLabel: UILabel = {
        let label = UILabel()
        label.text = "Voice Blur".localized
        return label
    }()
    private lazy var airborneLabel: UILabel = {
        let label = UILabel()
        label.text = "Airborne Simulation".localized
        return label
    }()
    private lazy var attenuationLabel: UILabel = {
        let label = UILabel()
        label.text = "Attenuation".localized
        return label
    }()
    private lazy var muteSwitch: UISwitch = {
        let view = UISwitch()
        view.addTarget(self, action: #selector(onTapMuteSwitch(sender:)), for: .valueChanged)
        return view
    }()
    private lazy var voiceBlurSwitch: UISwitch = {
        let view = UISwitch()
        view.addTarget(self, action: #selector(onTapVoiceBlurSwitch(sender:)), for: .valueChanged)
        return view
    }()
    private lazy var airborneSwitch: UISwitch = {
        let view = UISwitch()
        view.addTarget(self, action: #selector(onTapAirborneSwitch(sender:)), for: .valueChanged)
        return view
    }()
    private lazy var attenuationSilder: UISlider = {
        let view = UISlider()
        view.value = 0.2
        view.addTarget(self, action: #selector(onTapAttenuationSlider(sender:)), for: .valueChanged)
        return view
    }()
    override init(frame: CGRect) {
        super.init(frame: frame)
        setupUI()
    }
    
    required init?(coder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
    
    private func setupUI() {
        if #available(iOS 13.0, *) {
            backgroundColor = .tertiarySystemBackground
        } else {
            backgroundColor = .white
        }
        addSubview(muteLabel)
        addSubview(voiceBlurLabel)
        addSubview(airborneLabel)
        addSubview(attenuationLabel)
        addSubview(muteSwitch)
        addSubview(voiceBlurSwitch)
        addSubview(airborneSwitch)
        addSubview(attenuationSilder)
        muteLabel.translatesAutoresizingMaskIntoConstraints = false
        voiceBlurLabel.translatesAutoresizingMaskIntoConstraints = false
        airborneLabel.translatesAutoresizingMaskIntoConstraints = false
        attenuationLabel.translatesAutoresizingMaskIntoConstraints = false
        muteSwitch.translatesAutoresizingMaskIntoConstraints = false
        voiceBlurSwitch.translatesAutoresizingMaskIntoConstraints = false
        airborneSwitch.translatesAutoresizingMaskIntoConstraints = false
        attenuationSilder.translatesAutoresizingMaskIntoConstraints = false
        
        widthAnchor.constraint(equalToConstant: SCREENSIZE.width).isActive = true
        
        muteLabel.leadingAnchor.constraint(equalTo: leadingAnchor, constant: 15).isActive = true
        muteLabel.topAnchor.constraint(equalTo: topAnchor, constant: 20).isActive = true
        voiceBlurLabel.leadingAnchor.constraint(equalTo: muteLabel.leadingAnchor).isActive = true
        voiceBlurLabel.topAnchor.constraint(equalTo: muteLabel.bottomAnchor, constant: 15).isActive = true
        airborneLabel.leadingAnchor.constraint(equalTo: voiceBlurLabel.leadingAnchor).isActive = true
        airborneLabel.topAnchor.constraint(equalTo: voiceBlurLabel.bottomAnchor, constant: 15).isActive = true
        attenuationLabel.leadingAnchor.constraint(equalTo: airborneLabel.leadingAnchor).isActive = true
        attenuationLabel.topAnchor.constraint(equalTo: airborneLabel.bottomAnchor, constant: 15).isActive = true
        attenuationLabel.bottomAnchor.constraint(equalTo: bottomAnchor, constant: -25).isActive = true
        muteSwitch.trailingAnchor.constraint(equalTo: trailingAnchor, constant: -15).isActive = true
        muteSwitch.centerYAnchor.constraint(equalTo: muteLabel.centerYAnchor).isActive = true
        voiceBlurSwitch.trailingAnchor.constraint(equalTo: trailingAnchor, constant: -15).isActive = true
        voiceBlurSwitch.centerYAnchor.constraint(equalTo: voiceBlurLabel.centerYAnchor).isActive = true
        airborneSwitch.trailingAnchor.constraint(equalTo: trailingAnchor, constant: -15).isActive = true
        airborneSwitch.centerYAnchor.constraint(equalTo: airborneLabel.centerYAnchor).isActive = true
        attenuationSilder.trailingAnchor.constraint(equalTo: trailingAnchor, constant: -15).isActive = true
        attenuationSilder.centerYAnchor.constraint(equalTo: attenuationLabel.centerYAnchor).isActive = true
        attenuationSilder.leadingAnchor.constraint(equalTo: attenuationLabel.trailingAnchor, constant: 15).isActive = true
    }
    
    
    @objc
    private func onTapMuteSwitch(sender: UISwitch) {
        onTapMuteSwitchClosure?(sender.isOn)
    }
    @objc
    private func onTapVoiceBlurSwitch(sender: UISwitch) {
        onTapVoiceBlurSwitchClosure?(sender.isOn)
    }
    @objc
    private func onTapAirborneSwitch(sender: UISwitch) {
        onTapAirborneSwitchClosure?(sender.isOn)
    }
    @objc
    private func onTapAttenuationSlider(sender: UISlider) {
        onTapAttenuationSilderClosure?(sender.value)
    }
}
