//
//  JoinChannelVC.swift
//  APIExample
//
//  Created by 张乾泽 on 2020/4/17.
//  Copyright © 2020 Agora Corp. All rights reserved.
//
import UIKit
import AgoraRtcKit
import ARKit

class ARKitEntry : UIViewController
{
    @IBOutlet weak var joinButton: AGButton!
    @IBOutlet weak var channelTextField: AGTextField!
    @IBOutlet weak var firstFrameSwitch: UISwitch!
    private var isFirstFrame: Bool = false
    let identifier = "ARKit"
    
    override func viewDidLoad() {
        super.viewDidLoad()
    }

    @IBAction func doOptimizeFirstFrameSwitch(_ sender: UISwitch) {
        if sender.isOn {
            let alertVC = UIAlertController(title: "After this function is enabled, it cannot be disabled and takes effect only when both the primary and secondary ends are enabled".localized,
                                            message: nil,
                                            preferredStyle: .alert)
            
            let ok = UIAlertAction(title: "Sure".localized, style: .default, handler: nil)
            let cancel = UIAlertAction(title: "Cancel".localized, style: .cancel) { _ in
                sender.isOn = false
            }
            alertVC.addAction(ok)
            alertVC.addAction(cancel)
            present(alertVC, animated: true, completion: nil)
        }
    }
    
    @IBAction func doJoinPressed(sender: AGButton) {
        guard let channelName = channelTextField.text else {return}
        //resign channel text field
        channelTextField.resignFirstResponder()
        
        let storyBoard: UIStoryboard = UIStoryboard(name: identifier, bundle: nil)
        // create new view controller every time to ensure we get a clean vc
        guard let newViewController = storyBoard.instantiateViewController(withIdentifier: identifier) as? BaseViewController else {return}
        newViewController.title = channelName
        newViewController.configs = ["channelName":channelName, "isFirstFrame": firstFrameSwitch.isOn]
        navigationController?.pushViewController(newViewController, animated: true)
    }
}

class ARKitMain: BaseViewController {
    @IBOutlet weak var sceneView: ARSCNView!
    @IBOutlet weak var infoLabel: UILabel!
    @IBOutlet weak var statsLabel: UILabel!
    var agoraKit: AgoraRtcEngineKit!
    
    private var unusedScreenNodes = [SCNNode]()
    private var undisplayedUsers = [UInt]()
    private var activeScreens = [UInt: SCNNode]()
    private var renderer: ARVideoRenderer!
    
    // indicate if current instance has joined channel
    var isJoined: Bool = false
    var planarDetected: Bool = false {
        didSet {
            if(planarDetected) {
                infoLabel.text = "Tap to place remote video canvas".localized
            } else {
                infoLabel.text = "Move Camera to find a planar\n(Shown as Red Rectangle)".localized
            }
        }
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        //set AR Scene delegate
        sceneView.delegate = self
        sceneView.session.delegate = self
        sceneView.showsStatistics = true
        
        // set up agora instance when view loadedlet config = AgoraRtcEngineConfig()
        let config = AgoraRtcEngineConfig()
        config.appId = KeyCenter.AppId
        config.areaCode = GlobalSettings.shared.area
        // setup log file path
        let logConfig = AgoraLogConfig()
        logConfig.level = .info
        config.logConfig = logConfig
        
        agoraKit = AgoraRtcEngineKit.sharedEngine(with: config, delegate: self)
        
        if let isFirstFrame = configs["isFirstFrame"] as? Bool, isFirstFrame == true {
            agoraKit.enableInstantMediaRendering()
            agoraKit.startMediaRenderingTracing()
        }
        
        // get channel name from configs
        guard let channelName = configs["channelName"] as? String else {return}
        
        // make myself a broadcaster
        agoraKit.setChannelProfile(.liveBroadcasting)
        agoraKit.setClientRole(.broadcaster)
    
        // set AR video source as custom video source
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
        NetworkManager.shared.generateToken(channelName: channelName) { token in
            let result = self.agoraKit.joinChannel(byToken: token, channelId: channelName, info: nil, uid: 0, joinSuccess: nil)
            if result != 0 {
                // Usually happens with invalid parameters
                // Error code description can be found at:
                // en: https://docs.agora.io/en/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
                // cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
                self.showAlert(title: "Error", message: "joinChannel call failed: \(result), please check your params")
            }
        }
    }
    
    // start AR World tracking
    func startARSession() {
        guard ARWorldTrackingConfiguration.isSupported else {
            showAlert(title: "ARKit is not available on this device.".localized,
                      message: "This app requires world tracking, which is available only on iOS devices with the A9 processor or later.".localized)
            return
        }

        let configuration = ARWorldTrackingConfiguration()
        configuration.planeDetection = .horizontal
        // remember to set this to false, or ARKit may conflict with Agora SDK
        configuration.providesAudioData = false

        // start session
        sceneView.session.run(configuration)
    }
    
    // stop AR Tracking
    func stopARSession() {
        sceneView.session.pause()
    }
    
    @IBAction func doSceneViewTapped(_ recognizer: UITapGestureRecognizer) {
        if(!planarDetected) {
            LogUtils.log(message: "Planar not yet found", level: .warning)
            return
        }
        
        let location = recognizer.location(in: sceneView)
        
        if let node = sceneView.hitTest(location, options: nil).first?.node {
            removeNode(node)
        } else if let result = sceneView.hitTest(location, types: .existingPlane).first {
            addNode(withTransform: result.worldTransform)
        }
    }
    
    override func willMove(toParent parent: UIViewController?) {
        if parent == nil {
            // leave channel when exiting the view
            if isJoined {
                agoraKit.leaveChannel { (stats) -> Void in
                    LogUtils.log(message: "left channel, duration: \(stats.duration)", level: .info)
                }
            }
            stopARSession()
        }
    }
}

private extension ARKitMain {
    func renderRemoteUser(uid: UInt, toNode node: SCNNode) {
        renderer.renderNode = node
        activeScreens[uid] = node
    }
    
    func addNode(withTransform transform: matrix_float4x4) {
        let scene = SCNScene(named: "AR.scnassets/displayer.scn")!
        let rootNode = scene.rootNode
        
        rootNode.position = SCNVector3(
            transform.columns.3.x,
            transform.columns.3.y,
            transform.columns.3.z
        )
        rootNode.rotation = SCNVector4(0, 1, 0, sceneView.session.currentFrame!.camera.eulerAngles.y)
        
        sceneView.scene.rootNode.addChildNode(rootNode)
        
        let displayer = rootNode.childNode(withName: "displayer", recursively: false)!
        let screen = displayer.childNode(withName: "screen", recursively: false)!
        
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
            screen = parent.childNode(withName: "screen", recursively: false)!
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

/// agora rtc engine delegate events
extension ARKitMain: AgoraRtcEngineDelegate {
    /// callback when warning occured for agora sdk, warning can usually be ignored, still it's nice to check out
    /// what is happening
    /// Warning code description can be found at:
    /// en: https://docs.agora.io/en/Voice/API%20Reference/oc/Constants/AgoraWarningCode.html
    /// cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraWarningCode.html
    /// @param warningCode warning code of the problem
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurWarning warningCode: AgoraWarningCode) {
        LogUtils.log(message: "warning: \(warningCode.description)", level: .warning)
    }
    
    /// callback when error occured for agora sdk, you are recommended to display the error descriptions on demand
    /// to let user know something wrong is happening
    /// Error code description can be found at:
    /// en: https://docs.agora.io/en/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
    /// cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
    /// @param errorCode error code of the problem
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurError errorCode: AgoraErrorCode) {
        LogUtils.log(message: "error: \(errorCode.rawValue)", level: .error)
//        self.showAlert(title: "Error", message: "Error \(errorCode.description) occur")
    }
    
    /// callback when the local user joins a specified channel.
    /// @param channel
    /// @param uid uid of local user
    /// @param elapsed time elapse since current sdk instance join the channel in ms
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinChannel channel: String, withUid uid: UInt, elapsed: Int) {
        isJoined = true
        LogUtils.log(message: "Join \(channel) with uid \(uid) elapsed \(elapsed)ms", level: .info)
    }
    
    /// callback when a remote user is joinning the channel, note audience in live broadcast mode will NOT trigger this event
    /// @param uid uid of remote joined user
    /// @param elapsed time elapse since current sdk instance join the channel in ms
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinedOfUid uid: UInt, elapsed: Int) {
        LogUtils.log(message: "remote user join: \(uid) \(elapsed)ms", level: .info)
        
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
        
        if let screenNode = activeScreens[uid] {
//            agoraKit.setRemoteVideoRenderer(nil, forUserId: uid)
            unusedScreenNodes.insert(screenNode, at: 0)
            activeScreens[uid] = nil
        } else if let index = undisplayedUsers.firstIndex(of: uid) {
            undisplayedUsers.remove(at: index)
        }
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, videoRenderingTracingResultOfUid uid: UInt, currentEvent: AgoraMediaTraceEvent, tracingInfo: AgoraVideoRenderingTracingInfo) {
        statsLabel.isHidden = tracingInfo.elapsedTime <= 0
        statsLabel.text = "firstFrameTime: \(tracingInfo.elapsedTime)"
    }
}


extension ARKitMain: ARSCNViewDelegate {
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
        
        //found planar
        if(!planarDetected) {
            DispatchQueue.main.async {[weak self] in
                guard let weakSelf = self else {
                    return
                }
                weakSelf.planarDetected = true
            }
        }
    }
}

extension ARKitMain: ARSessionDelegate {
    func session(_ session: ARSession, didUpdate frame: ARFrame) {
        // send captured image to remote device
        renderer.pixelBuffer = frame.capturedImage
    }
}
