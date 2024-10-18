//
//  RtePlayer.swift
//  APIExample
//
//  Created by 张乾泽 on 2020/4/17.
//  Copyright © 2020 Agora Corp. All rights reserved.
//
import UIKit
import AgoraRtcKit

class RtePlayerEntry: UIViewController {
    @IBOutlet weak var joinButton: UIButton!
    @IBOutlet weak var channelTextField: UITextField!
    let identifier = "RtePlayer"
    private var backgroundColor: UInt32 = 0x000000
    
    private lazy var agoraKit: AgoraRtcEngineKit = {
        let config = AgoraRtcEngineConfig()
        config.appId = KeyCenter.AppId
        config.channelProfile = .liveBroadcasting
        let kit = AgoraRtcEngineKit.sharedEngine(with: config, delegate: nil)
        Util.configPrivatization(agoraKit: kit)
        kit.setLogFile(LogUtils.sdkLogPath())
        return kit
    }()
    
    override func viewDidLoad() {
        super.viewDidLoad()
        channelTextField.text = "rte://\(KeyCenter.AppId)/{\("Live Channel Name".localized)}"
    }
    
    override func willMove(toParent parent: UIViewController?) {
        if parent == nil {
            AgoraRtcEngineKit.destroy()
        }
    }
    
    @IBAction func doJoinPressed(sender: UIButton) {
        // resign channel text field
        channelTextField.resignFirstResponder()
        
        guard let channelName = channelTextField.text else { return }
        let storyBoard: UIStoryboard = UIStoryboard(name: identifier, bundle: nil)
        // create new view controller every time to ensure we get a clean vc
        guard let newViewController = storyBoard.instantiateViewController(withIdentifier: identifier) as? BaseViewController else { return
        }
        newViewController.title = channelName
        newViewController.configs = ["channelName": channelName,
                                     "backgroundColor": backgroundColor,
                                     "engine": agoraKit]
        navigationController?.pushViewController(newViewController, animated: true)
    }
    
    override func touchesEnded(_ touches: Set<UITouch>, with event: UIEvent?) {
        super.touchesEnded(touches, with: event)
        self.view.endEditing(true)
    }
}

class RtePlayerMain: BaseViewController {
    @IBOutlet weak var playerView: UIView?
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
        
        guard let url = configs["channelName"] as? String else {return}
        // layout render view
        let channelName = url.components(separatedBy: "/").last ?? ""
        NetworkManager.shared.generateToken(channelName: channelName, success: {[weak self] token in
            guard let self = self else {return}
            print("token: \(token ?? "")")
            self.playerUrl = "\(url)?token=\(self.urlEncoded(content: token ?? "") ?? "")"
            self.initRte {[weak self] success in
                print("initRte: ret: \(success)")
                guard success else {return}
                self?.onRtePlayerStart()
            }
        })
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
        let initErr: AgoraRteError? = nil
        rte.initMediaEngine({[weak self] err in
            guard let self = self else {return}
            print("initMediaEngine callback: \(err?.code().rawValue ?? 0)")
            let success = err?.code() == .ok ? true : false
            DispatchQueue.main.async {
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
    
    @IBAction func onRtePlayerStart() {
        guard let url = self.playerUrl else {return}
        print("open: \(url)")
        player.open(withUrl: url, startTime: 0) { err in
            print("open completion: \(err?.code().rawValue ?? 0)")
        }
    }
    
    @IBAction func onRtePlayerStop() {
        player.stop(nil)
    }
}

extension RtePlayerMain: PlayerObserverDelegate {
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
