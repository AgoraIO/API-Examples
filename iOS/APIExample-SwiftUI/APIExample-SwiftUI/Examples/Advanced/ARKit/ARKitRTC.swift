//
//  JoinChannelVideoRTC.swift
//  APIExample-SwiftUI
//
//  Created by zhaoyongqiang on 2024/3/19.
//

import AgoraRtcKit
import SwiftUI
import ARKit

class ARKitRTC: NSObject, ObservableObject {
    private var agoraKit: AgoraRtcEngineKit!
    private var isJoined: Bool = false
    private var renderer: ARVideoRenderer!
    
    private var unusedScreenNodes = [SCNNode]()
    private var undisplayedUsers = [UInt]()
    private var activeScreens = [UInt: SCNNode]()
    private var sceneView: ARSCNView?
    
    @Published var planarDetected: Bool = false
    @Published var isSupportedAR: Bool = false
    @Published var isHiddenStatsLabel: Bool = false
    @Published var stats: String = ""
    
    
    func setupRTC(configs: [String: Any], sceneView: ARSCNView) {
        self.sceneView = sceneView
        // set AR Scene delegate
        sceneView.delegate = self
        sceneView.session.delegate = self
        sceneView.showsStatistics = true
        // set up agora instance when view loaded
        let config = AgoraRtcEngineConfig()
        config.appId = KeyCenter.AppId
        config.areaCode = GlobalSettings.shared.area
        config.channelProfile = .liveBroadcasting
        agoraKit = AgoraRtcEngineKit.sharedEngine(with: config, delegate: self)
        // Configuring Privatization Parameters
        Util.configPrivatization(agoraKit: agoraKit)
        
        agoraKit.setLogFile(LogUtils.sdkLogPath())
        // get channel name from configs
        guard let channelName = configs["channelName"] as? String else {return}
        
        if let isFirstFrame = configs["isFirstFrame"] as? Bool, isFirstFrame == true {
            agoraKit.enableInstantMediaRendering()
            agoraKit.startMediaRenderingTracing()
        }
        
        // make myself a broadcaster
        agoraKit.setClientRole(GlobalSettings.shared.getUserRole())
 
        renderer = ARVideoRenderer()
        agoraKit.setVideoFrameDelegate(renderer)
        // start AR Session
        startARSession()
        
        // Set audio route to speaker
        agoraKit.setDefaultAudioRouteToSpeakerphone(true)
        
        // start joining channel
        // 1. Users can only see each other after they join the
        // same channel successfully using the same app id.
        // 2. If app certificate is turned on at dashboard, token is needed
        // when joining channel. The channel name and uid used to calculate
        // the token has to match the ones used for channel join
        let option = AgoraRtcChannelMediaOptions()
        option.publishCameraTrack = GlobalSettings.shared.getUserRole() == .broadcaster
        option.publishMicrophoneTrack = GlobalSettings.shared.getUserRole() == .broadcaster
        option.clientRoleType = GlobalSettings.shared.getUserRole()
        NetworkManager.shared.generateToken(channelName: channelName, success: { token in
            let result = self.agoraKit.joinChannel(byToken: token, channelId: channelName, uid: 0, mediaOptions: option)
            if result != 0 {
                // Usually happens with invalid parameters
                // Error code description can be found at:
                // en: https://api-ref.agora.io/en/video-sdk/ios/4.x/documentation/agorartckit/agoraerrorcode
                // cn: https://doc.shengwang.cn/api-ref/rtc/ios/error-code
                LogUtils.log(message: "joinChannel call failed: \(result), please check your params", level: .error)
            }
        })
    }
    
    // start AR World tracking
    func startARSession() {
        guard ARWorldTrackingConfiguration.isSupported else {
            isSupportedAR = false
            return
        }

        let configuration = ARWorldTrackingConfiguration()
        configuration.planeDetection = .horizontal
        // remember to set this to false, or ARKit may conflict with Agora SDK
        configuration.providesAudioData = false

        // start session
        sceneView?.session.run(configuration)
    }
    
    // stop AR Tracking
    func stopARSession() {
        sceneView?.session.pause()
    }
    
    func doSceneViewTapped(_ location: CGPoint) {
        if !planarDetected {
            LogUtils.log(message: "Planar not yet found", level: .warning)
            return
        }
        
        if let node = sceneView?.hitTest(location, options: nil).first?.node {
            removeNode(node)
        } else if let query = sceneView?.raycastQuery(from: location, allowing: .estimatedPlane, alignment: .any), let result = sceneView?.session.raycast(query).first {
            addNode(withTransform: result.worldTransform)
        }
    }
    
    func onDestory() {
        agoraKit.disableAudio()
        agoraKit.disableVideo()
        if isJoined {
            agoraKit.stopPreview()
            agoraKit.leaveChannel { (stats) -> Void in
                LogUtils.log(message: "left channel, duration: \(stats.duration)", level: .info)
            }
        }
        stopARSession()
        AgoraRtcEngineKit.destroy()
    }
}

private extension ARKitRTC {
    func renderRemoteUser(uid: UInt, toNode node: SCNNode) {
        renderer.renderNode = node
        activeScreens[uid] = node
    }
    func addNode(withTransform transform: matrix_float4x4) {
        guard let scene = SCNScene(named: "AR.scnassets/displayer.scn") else { return }
        let rootNode = scene.rootNode
        rootNode.position = SCNVector3(
            transform.columns.3.x,
            transform.columns.3.y,
            transform.columns.3.z
        )
        guard let currentFrame = sceneView?.session.currentFrame else { return }
        rootNode.rotation = SCNVector4(0, 1, 0, currentFrame.camera.eulerAngles.y)
        sceneView?.scene.rootNode.addChildNode(rootNode)
        let displayer = rootNode.childNode(withName: "displayer",
                                           recursively: false)
        guard let screen = displayer?.childNode(withName: "screen",
                                                recursively: false) else { return }
        if let undisplayedUid = undisplayedUsers.first {
            undisplayedUsers.removeFirst()
            renderRemoteUser(uid: undisplayedUid, toNode: screen)
        } else {
            unusedScreenNodes.append(screen)
        }
    }
    func removeNode(_ node: SCNNode) {
        let rootNode: SCNNode
        let screen: SCNNode
        if node.name == "screen", let parent = node.parent?.parent {
            rootNode = parent
            screen = node
        } else if node.name == "displayer", let parent = node.parent {
            rootNode = parent
            screen = parent.childNode(withName: "screen", recursively: false) ?? SCNNode()
        } else {
            rootNode = node
            screen = node
        }
        rootNode.removeFromParentNode()
        if let index = unusedScreenNodes.firstIndex(where: {$0 == screen}) {
            unusedScreenNodes.remove(at: index)
        }
        if let (uid, _) = activeScreens.first(where: {$1 == screen}) {
            activeScreens.removeValue(forKey: uid)
            if let screenNode = unusedScreenNodes.first {
                unusedScreenNodes.removeFirst()
                renderRemoteUser(uid: uid, toNode: screenNode)
            } else {
                undisplayedUsers.insert(uid, at: 0)
            }
        }
    }
}

// agora rtc engine delegate events
extension ARKitRTC: AgoraRtcEngineDelegate {
    /// callback when warning occured for agora sdk, warning can usually be ignored, still it's nice to check out
    /// what is happening
    /// Warning code description can be found at:
    /// en: https://api-ref.agora.io/en/voice-sdk/ios/3.x/Constants/AgoraWarningCode.html
    /// cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraWarningCode.html
    /// @param warningCode warning code of the problem
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurWarning warningCode: AgoraWarningCode) {
        LogUtils.log(message: "warning: \(warningCode.description)", level: .warning)
    }
    
    /// callback when error occured for agora sdk, you are recommended to display the error descriptions on demand
    /// to let user know something wrong is happening
    /// Error code description can be found at:
    /// en: https://api-ref.agora.io/en/video-sdk/ios/4.x/documentation/agorartckit/agoraerrorcode
    /// cn: https://doc.shengwang.cn/api-ref/rtc/ios/error-code
    /// @param errorCode error code of the problem
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurError errorCode: AgoraErrorCode) {
        LogUtils.log(message: "error: \(errorCode)", level: .error)
//        self.showAlert(title: "Error", message: "Error \(errorCode.description) occur")
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinChannel channel: String, withUid uid: UInt, elapsed: Int) {
        self.isJoined = true
        LogUtils.log(message: "Join \(channel) with uid \(uid) elapsed \(elapsed)ms", level: .info)
    }
    
    /// callback when a remote user is joinning the channel, note audience in live broadcast mode will NOT trigger this event
    /// @param uid uid of remote joined user
    /// @param elapsed time elapse since current sdk instance join the channel in ms
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinedOfUid uid: UInt, elapsed: Int) {
        LogUtils.log(message: "remote user join: \(uid) \(elapsed)ms", level: .info)
        // Only one remote video view is available for this
        // tutorial. Here we check if there exists a surface
        // view tagged as this uid.
        if let screenNode = unusedScreenNodes.first {
            unusedScreenNodes.removeFirst()
            renderRemoteUser(uid: uid, toNode: screenNode)
        } else {
            undisplayedUsers.append(uid)
        }
    }
    
    /// callback when a remote user is leaving the channel, note audience in live broadcast mode will NOT trigger this event
    /// @param uid uid of remote joined user
    /// @param reason reason why this user left, note this event may be triggered when the remote user
    /// become an audience in live broadcasting profile
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOfflineOfUid uid: UInt, reason: AgoraUserOfflineReason) {
        LogUtils.log(message: "remote user left: \(uid) reason \(reason)", level: .info)
        // to unlink your view from sdk, so that your view reference will be released
        // note the video will stay at its last frame, to completely remove it
        // you will need to remove the EAGL sublayer from your binded view
        if let screenNode = activeScreens[uid] {
//            agoraKit.setRemoteVideoRenderer(nil, forUserId: uid)
            unusedScreenNodes.insert(screenNode, at: 0)
            activeScreens[uid] = nil
        } else if let index = undisplayedUsers.firstIndex(of: uid) {
            undisplayedUsers.remove(at: index)
        }
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit,
                   videoRenderingTracingResultOfUid uid: UInt,
                   currentEvent: AgoraMediaTraceEvent,
                   tracingInfo: AgoraVideoRenderingTracingInfo) {
        isHiddenStatsLabel = tracingInfo.elapsedTime <= 0
        stats = "firstFrameTime: \(tracingInfo.elapsedTime)"
    }
}

extension ARKitRTC: ARSCNViewDelegate {
    func renderer(_ renderer: SCNSceneRenderer, didAdd node: SCNNode, for anchor: ARAnchor) {
        guard let planeAnchor = anchor as? ARPlaneAnchor else {
            return
        }
        
        let plane = SCNBox(width: CGFloat(planeAnchor.extent.x),
                           height: CGFloat(planeAnchor.extent.y),
                           length: CGFloat(planeAnchor.extent.z),
                           chamferRadius: 0)
        plane.firstMaterial?.diffuse.contents = UIColor.red
        
        let planeNode = SCNNode(geometry: plane)
        node.addChildNode(planeNode)
        planeNode.runAction(SCNAction.fadeOut(duration: 3))
        
        // found planar
        if !planarDetected {
            DispatchQueue.main.async {[weak self] in
                guard let weakSelf = self else {
                    return
                }
                weakSelf.planarDetected = true
            }
        }
    }
}

extension ARKitRTC: ARSessionDelegate {
    func session(_ session: ARSession, didUpdate frame: ARFrame) {
        // send captured image to remote device
        renderer.pixelBuffer = frame.capturedImage
    }
}
