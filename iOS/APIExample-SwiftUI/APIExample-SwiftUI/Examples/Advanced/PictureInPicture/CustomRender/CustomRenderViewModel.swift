//
//  CustomRenderExampleViewModel.swift
//  APIExample-SwiftUI
//
//  Created by qinhui on 2024/8/13.
//

import Foundation

class CustomRenderViewModel: NSObject, ObservableObject {
    private var isJoined = false
    var agoraRtcKit: CustomRenderRTC
    var configs: [String: Any]
    
    private lazy var remoteRenderViews: NSHashTable = {
        let table = NSHashTable<PixelBufferRenderView>(options: .weakMemory)
        return table
    }()
    
    init(configs: [String: Any]) {
        self.agoraRtcKit = CustomRenderRTC()
        self.configs = configs
        super.init()
        
        self.agoraRtcKit.rtcEngineDelegate = self
    }
    
    func setupRtcEngine(localView: PixelBufferRenderView) {
        guard let channelName = configs["channelName"] as? String else { return }
        let uid = UInt.random(in: 1..<100000)
        
        NetworkManager.shared.generateToken(channelName: channelName, success: { [weak self] token in
            guard let self = self else { return }
            self.agoraRtcKit.setupRtcEngine(token: token ?? "",
                                            channelName: channelName,
                                            userId: uid,
                                            clientRoleType: .broadcaster,
                                            localView: localView)
        })
    }
    
    func cleanRtc() {
        agoraRtcKit.disable()
        if isJoined {
            agoraRtcKit.leave()
        }
    }
    
    func addRenderView(renderView: PixelBufferRenderView) {
        remoteRenderViews.add(renderView)
        
        agoraRtcKit.addRenderView(renderView: renderView)
    }
    
    func removeRenderView(renderView: PixelBufferRenderView) {
        remoteRenderViews.remove(renderView)
        
        agoraRtcKit.removeRenderView(renderView: renderView)
    }
}

extension CustomRenderViewModel: AgoraRtcEngineDelegate {
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinChannel channel: String, withUid uid: UInt, elapsed: Int) {
        self.isJoined = true
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinedOfUid uid: UInt, elapsed: Int) {
        if let view = remoteRenderViews.allObjects.first(where: { $0.uid == 0 }) {
            view.uid = uid
        }
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOfflineOfUid uid: UInt, reason: AgoraUserOfflineReason) {
        if let view = remoteRenderViews.allObjects.first(where: { $0.uid == uid }) {
            self.removeRenderView(renderView: view)
        }
    }
}
