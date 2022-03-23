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
    @IBOutlet weak var startButton: UIButton!
    @IBOutlet weak var peopleView: UIImageView!
    @IBOutlet weak var soundSourceView: UIImageView!
    
    var agoraKit: AgoraRtcEngineKit!
    var remoteUid: UInt = 0
    var currentAngle = 0.0
    var currentDistance = 0.0
    var downCount = 0
    var downTimer: Timer?
    
    override func viewDidLoad() {
        super.viewDidLoad()
        self.setupUI()
                
        agoraKit = AgoraRtcEngineKit.sharedEngine(withAppId: KeyCenter.AppId, delegate: self)
        agoraKit.setChannelProfile(.liveBroadcasting)
        agoraKit.setClientRole(.broadcaster)
        agoraKit.setAudioProfile(.default, scenario: .gameStreaming)
    }
    
    override func willMove(toParent parent: UIViewController?) {
        if parent == nil {
            downTimer?.invalidate()
            downTimer = nil
            agoraKit = nil
            AgoraRtcEngineKit.destroy()
        }
    }
        
    func setupUI() {
        infoLabel.text = "请插入耳机体验空间音效效果".localized
        
        let panGesture = UIPanGestureRecognizer(target: self, action: #selector(panGestureChanged))
        self.soundSourceView.addGestureRecognizer(panGesture)
    }
    
    @IBAction func startBtnClicked(_ sender: Any) {
        guard let filePath = Bundle.main.path(forResource: "audiomixing", ofType: "mp3") else {return}
        let timeout = 10
        agoraKit.startEchoTest(withInterval: timeout)
        agoraKit.startAudioMixing(filePath, loopback: false, replace: true, cycle: 1, startPos: 0)
        
        startButton.isHidden = true
        downCount  = timeout * 2
        downTimer = Timer.scheduledTimer(withTimeInterval: 1, repeats: true) { _ in
            self.downCount -= 1
            if self.downCount >= timeout {
                if self.downCount == timeout {
                    self.agoraKit.enableSpatialAudio(true)
                    self.agoraKit.stopAudioMixing()
                    self.peopleView.isHidden = false
                    self.soundSourceView.isHidden = false
                } else {
                    self.infoLabel.text = "你会听到一段音乐, \(self.downCount - timeout)秒后这段音乐会通过空间音效的方式播放"
                }
            } else {
                self.infoLabel.text = "现在您可以移动喇叭图标到不同的位置, 体验空间音效效果(\(self.downCount))".localized
                if self.downCount == 0 {
                    self.agoraKit.stopEchoTest()
                    self.agoraKit.enableSpatialAudio(false)
                    self.downTimer?.invalidate()
                    self.downTimer = nil
                    self.peopleView.isHidden = true
                    self.soundSourceView.isHidden = true
                    self.startButton.isHidden = false
                    self.infoLabel.text = "请插入耳机体验空间音效效果".localized
                }
            }
        }
    }
    
    @objc func panGestureChanged(gesture: UIPanGestureRecognizer) {
        let circleCenter = self.peopleView.center
        let move = gesture.translation(in: self.view)
        var newCenter = gesture.view!.center
        newCenter = CGPoint(x: newCenter.x + move.x, y: newCenter.y + move.y)
        
        let deltaX = newCenter.x - circleCenter.x
        let deltaY = circleCenter.y - newCenter.y
        if deltaX == 0 {return}
        
        // In spatial audio, angle is range [0, 360], it is angle 0 when at Y direction.
        let tanValue = abs(deltaY) / abs(deltaX)
        let tanAngle = atan(tanValue) * 180.0 / Double.pi
        var spatialAngle = 0.0
        if deltaX > 0 && deltaY > 0 { // scope I
            spatialAngle = 270.0 + tanAngle
        } else if deltaX < 0 && deltaY > 0 { // scope II
            spatialAngle = 90.0 - tanAngle
        } else if deltaX < 0 && deltaY < 0 { // scope III
            spatialAngle = 90.0 + tanAngle
        } else if deltaX > 0 && deltaY < 0 { // scope IV
            spatialAngle = 270.0 - tanAngle
        }
        
        if gesture.state == .ended {
            let L = sqrt(deltaX * deltaX + deltaY * deltaY)
            let maxL = UIScreen.main.bounds.height / 2.0
            let maxSpatailDistance = 30.0
            let spatialDistance = L * maxSpatailDistance / maxL
            currentAngle = spatialAngle
            currentDistance = spatialDistance
            
            self.updateRemoteUserSpatialAudioPositon()
        }
        
        self.soundSourceView.center = newCenter
        gesture.setTranslation(.zero, in: self.view)
    }
    
    func updateRemoteUserSpatialAudioPositon() {
        let spatialParams = AgoraSpatialAudioParams()
        spatialParams.speaker_azimuth = .of(Double(currentAngle))
        spatialParams.speaker_distance = .of(currentDistance)
        spatialParams.speaker_elevation = .of(0)
        spatialParams.enable_blur = .of(false)
        spatialParams.enable_air_absorb = .of(true)
        agoraKit.setRemoteUserSpatialAudioParams(remoteUid, param: spatialParams)
    }
}
    
extension SpatialAudioMain: AgoraRtcEngineDelegate {
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinedOfUid uid: UInt, elapsed: Int) {
        print("didJoinedOfUid:\(uid)")
        remoteUid = uid
        self.updateRemoteUserSpatialAudioPositon()
    }
}
