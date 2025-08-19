//
//  VideoViewController.swift
//  PIPDemo
//
//  Created by qinhui on 2024/8/7.
//

import UIKit
import SnapKit
import AgoraRtcKit
import AVKit

@available(iOS 15.0, *)
class CustomViewPIPViewController: PIPBaseViewController {
    private let containerH = 250.0
    private var isJoined: Bool = false
    private var pipController: AVPictureInPictureController?
    private var videoCallbackController: AVPictureInPictureVideoCallViewController?
    private var backgroundTask: UIBackgroundTaskIdentifier = .invalid

    private var pipSizes = [
        CGSize(width: 150, height: 300),
        CGSize(width: 300, height: 150)
    ]
    
    private lazy var pipButton: UIButton = {
        let button = UIButton(type: .custom)
        button.setTitle("Picture in Picture", for: .normal)
        button.addTarget(self, action: #selector(pipAction), for: .touchUpInside)
        button.backgroundColor = .purple
        return button
    }()
    
    private lazy var sizeButton: UIButton = {
        let button = UIButton(type: .custom)
        button.setTitle("Switch Size", for: .normal)
        button.addTarget(self, action: #selector(sizeAction), for: .touchUpInside)
        button.backgroundColor = .red
        
        return button
    }()

    private lazy var localVideoView: UIView = {
        let view = UIView()
        view.backgroundColor = .green
        return view
    }()
    
    private lazy var remoteVideoView: UIView = {
        let view = UIView()
        view.backgroundColor = .orange
        return view
    }()
    
    private lazy var videoContainerView: UIView = {
        let view = UIView()
        view.backgroundColor = .purple
        return view
    }()
    
    private var rtcService: CustomViewPIPService!
 
    override func viewDidLoad() {
        super.viewDidLoad()
        view.backgroundColor = .white
        initRtc()
        configViews()
        if AVPictureInPictureController.isPictureInPictureSupported() {
            configPIPViewController()
        }
    }
    
    override func viewWillDisappear(_ animated: Bool) {
        super.viewWillDisappear(animated)
        guard let pipController = pipController else { return }
        pipController.stopPictureInPicture()
    }
    
    override func viewDidDisappear(_ animated: Bool) {
        super.viewDidDisappear(animated)
        rtcService.disable()
        
        if isJoined {
            rtcService.leave()
        }
    }
}

@available(iOS 15.0, *)
extension CustomViewPIPViewController {
    @objc func pipAction() {
        guard let pipController = pipController else { return }
        
        if pipController.isPictureInPictureActive {
            pipController.stopPictureInPicture()
        } else {
            pipController.startPictureInPicture()
        }
    }
    
    @objc func sizeAction() {
        guard let videoCallbackController = videoCallbackController else { return }
        
        let i = Int.random(in: 0..<pipSizes.count)
        let size = pipSizes[i]
        videoCallbackController.preferredContentSize = size
    }
    
    private func initRtc() {
        guard let channelId = channelId else { return }
        rtcService = CustomViewPIPService(localView: localVideoView, remoteView: remoteVideoView, channelId: channelId)
        rtcService.rtcEngineDelegate = self
        startBackgroundTask()
    }
    
    private func configPIPViewController() {
        do {
            try AVAudioSession.sharedInstance().setCategory(.playback)
            try AVAudioSession.sharedInstance().setActive(true, options: .notifyOthersOnDeactivation)
        } catch {
            print(error)
        }
        
        let videoCallViewController = AVPictureInPictureVideoCallViewController()
        videoCallViewController.preferredContentSize = CGSize(width: 300, height: 150)
        videoCallViewController.view.backgroundColor = .clear
        videoCallViewController.modalPresentationStyle = .overFullScreen
        
        self.videoCallbackController = videoCallViewController
        pipController = AVPictureInPictureController(contentSource: .init(activeVideoCallSourceView: videoContainerView,
                                                                          contentViewController: videoCallViewController))
        pipController?.canStartPictureInPictureAutomaticallyFromInline = true
        pipController?.delegate = self
        pipController?.setValue(1, forKey: "controlsStyle")
        
        NotificationCenter.default.addObserver(self, 
                                               selector: #selector(handleEnterForeground),
                                               name: UIApplication.willEnterForegroundNotification,
                                               object: nil)
        NotificationCenter.default.addObserver(self, 
                                               selector: #selector(handleEnterBackground),
                                               name: UIApplication.didEnterBackgroundNotification,
                                               object: nil)
    }
    
    private func configViews() {
        view.addSubview(videoContainerView)
        videoContainerView.addSubview(localVideoView)
        videoContainerView.addSubview(remoteVideoView)
        view.addSubview(pipButton)
        view.addSubview(sizeButton)
        
        videoContainerView.snp.makeConstraints { make in
            make.left.top.right.equalToSuperview()
            make.height.equalTo(containerH)
        }

        localVideoView.snp.makeConstraints { make in
            make.left.top.bottom.equalToSuperview()
            make.right.equalTo(remoteVideoView.snp.left)
            make.width.equalTo(remoteVideoView)
        }

        remoteVideoView.snp.makeConstraints { make in
            make.right.top.bottom.equalToSuperview()
            make.width.equalTo(localVideoView)
        }
        
        pipButton.snp.makeConstraints { make in
            make.center.equalTo(view)
        }
        
        sizeButton.snp.makeConstraints { make in
            make.top.equalTo(self.pipButton.snp.bottom).offset(10)
            make.centerX.equalTo(self.pipButton.snp.centerX)
        }
    }
    

    @objc private func handleEnterForeground() {

    }
    
    @objc private func handleEnterBackground() {

    }
    
    func startBackgroundTask() {
        backgroundTask = UIApplication.shared.beginBackgroundTask {
            self.endBackgroundTask()
        }
    }

    func endBackgroundTask() {
        UIApplication.shared.endBackgroundTask(backgroundTask)
        backgroundTask = .invalid
    }

}

@available(iOS 15.0, *)
extension CustomViewPIPViewController: AgoraRtcEngineDelegate {
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinChannel channel: String, withUid uid: UInt, elapsed: Int) {
        isJoined = true
    }
}

@available(iOS 15.0, *)
extension CustomViewPIPViewController: AVPictureInPictureControllerDelegate {
    func pictureInPictureControllerWillStartPictureInPicture(_ pictureInPictureController: AVPictureInPictureController) {
        videoContainerView.removeFromSuperview()
        guard let vc = pictureInPictureController.contentSource?.activeVideoCallContentViewController else { return }
        vc.view.addSubview(videoContainerView)

        videoContainerView.snp.remakeConstraints { make in
            make.edges.equalTo(UIEdgeInsets.zero)
        }
    }
    
    func pictureInPictureControllerDidStartPictureInPicture(_ pictureInPictureController: AVPictureInPictureController) {
        
    }
    
    func pictureInPictureControllerWillStopPictureInPicture(_ pictureInPictureController: AVPictureInPictureController) {
        videoContainerView.removeFromSuperview()
        view.addSubview(videoContainerView)
        
        videoContainerView.snp.makeConstraints { make in
            make.left.top.right.equalTo(0)
            make.height.equalTo(containerH)
        }
    }
}
