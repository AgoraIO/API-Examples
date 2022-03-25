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

class SpatialAudioMain: BaseViewController {
    @IBOutlet weak var infoLabel: UILabel!
    @IBOutlet weak var peopleView: UIImageView!
    @IBOutlet weak var soundSourceView: UIImageView!
    
    var agoraKit: AgoraRtcEngineKit!
    var mediaPlayer: AgoraRtcMediaPlayerProtocol!
    var localSpatial: AgoraLocalSpatialAudioKit!
    
    var currentAngle = 0.0
    var currentDistance = 0.0
    var downTimer: Timer?
    
    override func viewDidLoad() {
        super.viewDidLoad()
        self.setupUI()
        
        agoraKit = AgoraRtcEngineKit.sharedEngine(withAppId: KeyCenter.AppId, delegate: self)
        agoraKit.setChannelProfile(.liveBroadcasting)
        agoraKit.setClientRole(.broadcaster)
        
        agoraKit.setAudioProfile(.default, scenario: .gameStreaming)
        agoraKit.enableSpatialAudio(true)
        
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
            let pos = [NSNumber(0.0), NSNumber(0), NSNumber(0.0)]
            let forward = [NSNumber(1.0), NSNumber(0.0), NSNumber(0.0)]
            let right = [NSNumber(0.0), NSNumber(1.0), NSNumber(0.0)]
            let up = [NSNumber(0.0), NSNumber(0.0), NSNumber(1.0)]
            
            self.localSpatial.updateSelfPosition(pos, axisForward: forward, axisRight: right, axisUp: up)
        }
        
        updatePosition(objectCenter: soundSourceView.center)
    }

    override func willMove(toParent parent: UIViewController?) {
        if parent == nil {
            downTimer?.invalidate()
            downTimer = nil
            agoraKit = nil
            AgoraLocalSpatialAudioKit.destroy()
            AgoraRtcEngineKit.destroy()
        }
    }
    
    func setupUI() {
        infoLabel.text = "Please insert headphones to experience the spatial audio effect.Now you can move the speaker icon to experience the spatial audio effect".localized
        
        let panGesture = UIPanGestureRecognizer(target: self, action: #selector(panGestureChanged))
        soundSourceView.addGestureRecognizer(panGesture)
    }
        
    @objc func panGestureChanged(gesture: UIPanGestureRecognizer) {
        let move = gesture.translation(in: self.view)
        var objectCenter = gesture.view!.center
        objectCenter = CGPoint(x: objectCenter.x + move.x, y: objectCenter.y + move.y)
        soundSourceView.center = objectCenter
        gesture.setTranslation(.zero, in: self.view)
  
        if gesture.state == .ended {
            updatePosition(objectCenter: objectCenter)
        }
    }
    
    func updatePosition(objectCenter: CGPoint) {
        let circleCenter = peopleView.center
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
        updateRemoteUserSpatialAudioPositon()
    }
    
    func updateRemoteUserSpatialAudioPositon() {
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

// MARK: -
class SpatialAudioEntry: UIViewController {
    @IBOutlet weak var channelTextField: UITextField!
          
      @IBAction func joinBtnClick(sender: UIButton) {
          guard let channelName = channelTextField.text,
                    channelName.lengthOfBytes(using: .utf8) > 0 else {return}
          channelTextField.resignFirstResponder()
          
          let identifier = "SpatialAudio"
          let storyBoard: UIStoryboard = UIStoryboard(name: identifier, bundle: nil)
          guard let newViewController = storyBoard.instantiateViewController(withIdentifier: identifier) as? BaseViewController else {return}
          newViewController.title = channelName
          newViewController.configs = ["channelName": channelName]
          self.navigationController?.pushViewController(newViewController, animated: true)
      }
}
