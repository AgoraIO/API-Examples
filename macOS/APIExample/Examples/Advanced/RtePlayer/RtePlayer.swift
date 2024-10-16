//
//  CreateDataStream.swift
//  APIExample
//
//  Created by XC on 2020/12/28.
//  Copyright © 2020 Agora Corp. All rights reserved.
//

import Cocoa
import AgoraRtcKit
import AGEVideoLayout

class RtePlayer: BaseViewController {
    @IBOutlet weak var playerView: NSView?
    
    var agoraKit: AgoraRtcEngineKit!
    
    private var isRteInited = false
    /**
     --- Channel TextField ---
     */
    @IBOutlet weak var channelField: Input!
    func initChannelField() {
        channelField.stringValue = "rte://\(KeyCenter.AppId)/{\("Live Channel Name".localized)}"
        channelField.field.placeholderString = "Channel Name".localized
    }
    
    /**
     --- Join Button ---
     */
    @IBOutlet weak var joinChannelButton: NSButton!
    func initJoinChannelButton() {
        joinChannelButton.title = isPlaying ? "Stop".localized : "Play".localized
    }
    
    // indicate if current instance has joined channel
    var isPlaying: Bool = false {
        didSet {
            channelField.isEnabled = !isPlaying
            initJoinChannelButton()
        }
    }
    
    private var playerUrl: String?
    private lazy var rte: AgoraRte = {
        let initialConfig = AgoraRteInitialConfig()
        let rte = AgoraRte(initialConfig: initialConfig)
        
        let config = AgoraRteConfig()
        config.setAppId(KeyCenter.AppId, error: nil)
        rte.setConfigs(config, error: nil)
        
        return rte
    }()
    
    private lazy var playerObserver: PlayerObserver = {
        let observer = PlayerObserver()
        observer.delegate = self
        
        return observer
    }()
    
    private lazy var player: AgoraRtePlayer = {
        // Create player
        let playerInitialConfig = AgoraRtePlayerInitialConfig()
        self.player = AgoraRtePlayer(rte: rte, initialConfig: playerInitialConfig)
        
        let playerConfig = AgoraRtePlayerConfig()
        playerConfig.setAutoPlay(true, error: nil)
        player.setConfigs(playerConfig, error: nil)
        
        // Create playerObserver
        player.register(playerObserver, error: nil)
        
        return player
    }()
    
    private lazy var canvas: AgoraRteCanvas = {
        // Create canvas
        let canvasInitialConfig = AgoraRteCanvasInitialConfig()
        let canvas = AgoraRteCanvas(rte: self.rte, initialConfig: canvasInitialConfig)

        let canvasConfig = AgoraRteCanvasConfig()
        canvasConfig.setVideoRenderMode(.fit, error: nil)
        canvas.setConfigs(canvasConfig, error: nil)
        
        return canvas
    }()
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        // Do view setup here.
        let config = AgoraRtcEngineConfig()
        config.appId = KeyCenter.AppId
        config.areaCode = GlobalSettings.shared.area
        agoraKit = AgoraRtcEngineKit.sharedEngine(with: config, delegate: self)
        // Configuring Privatization Parameters
        Util.configPrivatization(agoraKit: agoraKit)
        agoraKit.enableVideo()
        
        initChannelField()
        initJoinChannelButton()
    }
    
    override func viewWillBeRemovedFromSplitView() {
        AgoraRtcEngineKit.destroy()
    }
    
    @IBAction func onJoinPressed(_ sender:Any) {
        if !isPlaying {
            // check configuration
            let url = channelField.stringValue
            if url.isEmpty {
                return
            }
            self.playerUrl = url
            // layout render view
            let channelName = url.components(separatedBy: "/").last ?? ""
            NetworkManager.shared.generateToken(channelName: channelName, success: {[weak self] token in
                guard let self = self else {return}
                print("token: \(token ?? "")")
                self.playerUrl = "\(url)?token=\(self.urlEncoded(content: token ?? "") ?? "")"
                self.initRte {[weak self] success in
                    print("initRte: ret: \(success)")
                    guard success else {return}
                    self?.rtePlayerStart()
                    self?.isPlaying = true
                }
            })
        } else {
            self.playerUrl = nil
            self.isPlaying = false
        }
    }
    
    private func urlEncoded(content: String?) -> String? {
        // Create a mutable allowed character set
        var allowedCharacters = CharacterSet.urlQueryAllowed

        // Remove the characters that need to be encoded
        allowedCharacters.remove(charactersIn: "+/")

        // Encode the string
        if let encodedString = content?.addingPercentEncoding(withAllowedCharacters: allowedCharacters) {
            print("Encoded token: \(encodedString)")
            return encodedString
        } else {
            print("Encoding failed.")
            return nil
        }
    }
    
    private func initRte(_ completion: ((Bool) -> Void)?) {
        if isRteInited {
            completion?(true)
            return
        }
        let initErr: AgoraRteError? = nil
        rte.initMediaEngine({[weak self] err in
            guard let self = self else {return}
            print("initMediaEngine callback: \(err?.code().rawValue ?? 0)")
            let success = err?.code() == .ok ? true : false
            DispatchQueue.main.async {
                self.isRteInited = success
                guard success, let playerView = self.playerView else {
                    completion?(false)
                    return
                }

                self.canvas.add(playerView, config: nil, error: nil)
                self.player.setCanvas(self.canvas, error: nil)
                completion?(true)
            }
        }, error: initErr)
        print("initMediaEngine err: \(initErr?.code().rawValue ?? 0)")
    }
    
    private func rtePlayerStart() {
        guard let url = self.playerUrl else {return}
        print("open: \(url)")
        player.open(withUrl: url, startTime: 0) { err in
            print("open completion: \(err?.code().rawValue ?? 0)")
        }
    }
    
    private func rtePlayerStop() {
        player.stop(nil)
    }
}

/// agora rtc engine delegate events
extension RtePlayer: AgoraRtcEngineDelegate {
    /// callback when warning occured for agora sdk, warning can usually be ignored, still it's nice to check out
    /// what is happening
    /// Warning code description can be found at:
    /// en: https://api-ref.agora.io/en/voice-sdk/ios/3.x/Constants/AgoraWarningCode.html
    /// cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraWarningCode.html
    /// @param warningCode warning code of the problem
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurWarning warningCode: AgoraWarningCode) {
        LogUtils.log(message: "warning: \(warningCode.rawValue)", level: .warning)
    }
    
    /// callback when error occured for agora sdk, you are recommended to display the error descriptions on demand
    /// to let user know something wrong is happening
    /// Error code description can be found at:
    /// en: https://api-ref.agora.io/en/video-sdk/ios/4.x/documentation/agorartckit/agoraerrorcode
    /// cn: https://doc.shengwang.cn/api-ref/rtc/ios/error-code
    /// @param errorCode error code of the problem
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurError errorCode: AgoraErrorCode) {
        LogUtils.log(message: "error: \(errorCode)", level: .error)
        self.showAlert(title: "Error", message: "Error \(errorCode.rawValue) occur")
    }
    
    /// callback when the local user joins a specified channel.
    /// @param channel
    /// @param uid uid of local user
    /// @param elapsed time elapse since current sdk instance join the channel in ms
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinChannel channel: String, withUid uid: UInt, elapsed: Int) {
        LogUtils.log(message: "Join \(channel) with uid \(uid) elapsed \(elapsed)ms", level: .info)
    }
    
    /// callback when a remote user is joinning the channel, note audience in live broadcast mode will NOT trigger this event
    /// @param uid uid of remote joined user
    /// @param elapsed time elapse since current sdk instance join the channel in ms
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinedOfUid uid: UInt, elapsed: Int) {
        LogUtils.log(message: "remote user join: \(uid) \(elapsed)ms", level: .info)
    }
    
    /// callback when a remote user is leaving the channel, note audience in live broadcast mode will NOT trigger this event
    /// @param uid uid of remote joined user
    /// @param reason reason why this user left, note this event may be triggered when the remote user
    /// become an audience in live broadcasting profile
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOfflineOfUid uid: UInt, reason: AgoraUserOfflineReason) {
        LogUtils.log(message: "remote user left: \(uid) reason \(reason)", level: .info)
    }
}

extension RtePlayer: PlayerObserverDelegate {
    func onStateChanged(oldState: AgoraRtePlayerState, newState: AgoraRtePlayerState, error: AgoraRteError?) {
        print("onStateChanged: oldState: \(oldState.rawValue) newState:\(newState.rawValue) error: \(error?.code().rawValue ?? 0)")
    }
    
    func onPositionChanged(currentTime: UInt64, utcTime: UInt64) {
        
    }
    
    func onResolutionChanged(width: Int, height: Int) {
        print("onResolutionChanged: {\(width), \(height)}")
    }
    
    func onEvent(event: AgoraRtePlayerEvent) {
        print("onEvent: \(event.rawValue)")
    }
    
    func onMetadata(type: AgoraRtePlayerMetadataType, data: Data) {
        
    }
    
    func onPlayerInfoUpdated(info: AgoraRtePlayerInfo) {
        
    }
    
    func onAudioVolumeIndication(volume: Int32) {
        
    }
}
