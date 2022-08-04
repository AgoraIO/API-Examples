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
    @IBOutlet weak var startButton: NSButton!
    @IBOutlet weak var peopleView: NSImageView!
    @IBOutlet weak var soundSourceView: NSImageView!
    
    var agoraKit: AgoraRtcEngineKit!
    var mediaPlayer: AgoraRtcMediaPlayerProtocol!
    var localSpatial: AgoraLocalSpatialAudioKit!

    var currentAngle = 0.0
    var currentDistance = 0.0
    var downTimer: Timer?
    
    override func viewDidLoad() {
        super.viewDidLoad()
        self.setupUI()
    }
    
    override func viewWillBeRemovedFromSplitView() {
        downTimer?.invalidate()
        downTimer = nil
        AgoraLocalSpatialAudioKit.destroy()
        AgoraRtcEngineKit.destroy()
    }

    func setupUI() {
        infoLabel.stringValue = "Please insert headphones to experience the spatial audio effect.Now you can move the speaker icon to experience the spatial audio effect".localized
        startButton.title = "Start".localized

        let panGesture = NSPanGestureRecognizer(target: self, action: #selector(panGestureChanged))
        self.soundSourceView.addGestureRecognizer(panGesture)
    }
    
    
    @IBAction func startBtnClicked(_ sender: Any) {
        startButton.isHidden = true
        peopleView.isHidden = false
        soundSourceView.isHidden = false
        
        agoraKit = AgoraRtcEngineKit.sharedEngine(withAppId: KeyCenter.AppId, delegate: self)
        agoraKit.setChannelProfile(.liveBroadcasting)
        agoraKit.setClientRole(.broadcaster)
        
        agoraKit.setAudioProfile(.default, scenario: .gameStreaming)
        agoraKit.muteAllRemoteAudioStreams(true)
        
        guard let filePath = Bundle.main.path(forResource: "audiomixing", ofType: "mp3") else {return}
        mediaPlayer = agoraKit.createMediaPlayer(with: self)
        mediaPlayer.setLoopCount(10)
        mediaPlayer.open(filePath, startPos: 0)
        
        let localSpatialConfig = AgoraLocalSpatialAudioConfig()
        localSpatialConfig.rtcEngine = agoraKit
        localSpatial = AgoraLocalSpatialAudioKit.sharedLocalSpatialAudio(with: localSpatialConfig)
        localSpatial.muteLocalAudioStream(false)
        localSpatial.muteAllRemoteAudioStreams(false)
        localSpatial.setAudioRecvRange(50)
        localSpatial.setDistanceUnit(1)
                
        downTimer = Timer.scheduledTimer(withTimeInterval: 3, repeats: true) { _ in
            let position = [NSNumber(0.0), NSNumber(0), NSNumber(0.0)]
            let forward = [NSNumber(1.0), NSNumber(0.0), NSNumber(0.0)]
            let right = [NSNumber(0.0), NSNumber(1.0), NSNumber(0.0)]
            let up = [NSNumber(0.0), NSNumber(0.0), NSNumber(1.0)]
            
            self.localSpatial.updateSelfPosition(position, axisForward: forward, axisRight: right, axisUp: up)
        }
        
        let rect = self.soundSourceView.frame
        let center = CGPoint(x: rect.origin.x + rect.size.width / 2.0, y: rect.origin.y + rect.size.height / 2.0)
        updatePosition(objectCenter:center)
    }
    
    @objc func panGestureChanged(gesture: NSPanGestureRecognizer) {
        let move = gesture.translation(in: self.view)
        var objectCenter = CGPoint(x: NSMidX(gesture.view!.frame), y: NSMidY(gesture.view!.frame))
        objectCenter = CGPoint(x: objectCenter.x + move.x, y: objectCenter.y + move.y)
        
        let width = soundSourceView.frame.size.width
        soundSourceView.frame = CGRect(origin: CGPoint(x: objectCenter.x - width / 2.0, y: objectCenter.y - width / 2.0), size: CGSize(width: width, height: width))
        gesture.setTranslation(.zero, in: self.view)
  
        if gesture.state == .ended {
            updatePosition(objectCenter: objectCenter)
        }
    }
    
    func updatePosition(objectCenter: CGPoint) {
        let circleCenter = CGPoint(x: NSMidX(peopleView.frame), y: NSMidY(peopleView.frame))
        let deltaX = objectCenter.x - circleCenter.x
        let deltaY = objectCenter.y - circleCenter.y
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
        self.updateRemoteUserSpatialAudioPositon()
    }
    
    func updateRemoteUserSpatialAudioPositon() {
        let maxR = (NSScreen.main?.frame.height)! / 2.0
        let maxSpatailDistance = 30.0
        let spatialDistance = currentDistance * maxSpatailDistance / maxR

        let posForward = spatialDistance * cos(currentAngle);
        let posRight = spatialDistance * sin(currentAngle);
        let position = [NSNumber(value: posForward), NSNumber(value: posRight), NSNumber(0.0)]
        let forward = [NSNumber(1.0), NSNumber(0.0), NSNumber(0.0)]
        
        let positionInfo = AgoraRemoteVoicePositionInfo()
        positionInfo.position = position
        positionInfo.forward = forward
        localSpatial.updatePlayerPositionInfo(Int(mediaPlayer.getMediaPlayerId()), positionInfo: positionInfo)
    }
}

extension SpatialAudioMain: AgoraRtcEngineDelegate {
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinedOfUid uid: UInt, elapsed: Int) {
        print("didJoinedOfUid:\(uid)")
    }
}

extension SpatialAudioMain: AgoraRtcMediaPlayerDelegate {
    func agoraRtcMediaPlayer(_ playerKit: AgoraRtcMediaPlayerProtocol, didChangedTo state: AgoraMediaPlayerState, error: AgoraMediaPlayerError) {
        print("didChangedTo: \(state.rawValue), \(error.rawValue)")
        if state == .openCompleted {
            playerKit.play()
        }
    }
}


