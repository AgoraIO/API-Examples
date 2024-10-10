//
//  PixelBufferPIPViewController.swift
//  PIPDemo
//
//  Created by qinhui on 2024/8/8.
//
import UIKit
import AVKit
import AgoraRtcKit

@available(iOS 15.0, *)
class PixelBufferPIPViewController: PIPBaseViewController {
    private let mockUid: UInt = UInt.random(in: 0...100000)
    private var pipController: AVPictureInPictureController?
    private var videoCallbackController: AVPictureInPictureVideoCallViewController?
    var isJoined = false
    private var pipSizes = [
        CGSize(width: 150, height: 300),
        CGSize(width: 300, height: 150)
    ]

    private lazy var pipButton: UIButton = {
        let button = UIButton(type: .custom)
        button.setTitle("画中画", for: .normal)
        button.setTitleColor(.black, for: .normal)
        button.addTarget(self, action: #selector(pipAction), for: .touchUpInside)
        
        return button
    }()
    
    private lazy var sizeButton: UIButton = {
        let button = UIButton(type: .custom)
        button.setTitle("切换尺寸", for: .normal)
        button.setTitleColor(.black, for: .normal)
        button.addTarget(self, action: #selector(sizeAction), for: .touchUpInside)
        
        return button
    }()
    
    private lazy var topLeftView: PixelBufferRenderView = {
        let view = PixelBufferRenderView()
        view.backgroundColor = .blue
        return view
    }()
    
    private lazy var topRightView: PixelBufferRenderView = {
        let view = PixelBufferRenderView()
        view.backgroundColor = .red
        return view
    }()
    
    private lazy var bottomLeftView: PixelBufferRenderView = {
        let view = PixelBufferRenderView()
        view.backgroundColor = .green
        return view
    }()
    
    private lazy var bottomRightView: PixelBufferRenderView = {
        let view = PixelBufferRenderView()
        view.backgroundColor = .purple
        return view
    }()
    
    private lazy var videoContainerView: UIView = {
        let view = UIView()
        view.backgroundColor = .purple
        return view
    }()
    
    private lazy var displayViews: NSHashTable = {
        let table = NSHashTable<PixelBufferRenderView>(options: .weakMemory, capacity: 4)
        table.add(self.topLeftView)
        table.add(self.topRightView)
        table.add(bottomLeftView)
        table.add(bottomRightView)
        return table
    }()
    
    private var rtcService: PixelBufferPIPService!
    
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
extension PixelBufferPIPViewController {
    private func configPIPViewController() {
        let videoCallViewController = AVPictureInPictureVideoCallViewController()
        videoCallViewController.preferredContentSize = view.bounds.size
        videoCallViewController.view.backgroundColor = .clear
        videoCallViewController.modalPresentationStyle = .overFullScreen
        
        self.videoCallbackController = videoCallViewController
        pipController = AVPictureInPictureController(contentSource: .init(activeVideoCallSourceView: videoContainerView, 
                                                                          contentViewController: videoCallViewController))
        pipController?.canStartPictureInPictureAutomaticallyFromInline = true
        pipController?.delegate = self
        pipController?.setValue(1, forKey: "controlsStyle")
    }
    
    private func configViews() {
        self.view.addSubview(videoContainerView)
        videoContainerView.addSubview(topLeftView)
        videoContainerView.addSubview(topRightView)
        videoContainerView.addSubview(bottomLeftView)
        videoContainerView.addSubview(bottomRightView)
        
        self.view.addSubview(pipButton)
        self.view.addSubview(sizeButton)
        
        videoContainerView.snp.makeConstraints { make in
            make.left.top.right.bottom.equalTo(0)
        }
        
        topLeftView.snp.makeConstraints { make in
            make.top.left.equalToSuperview()
            make.width.equalToSuperview().dividedBy(2)
            make.height.equalToSuperview().dividedBy(2)
        }

        topRightView.snp.makeConstraints { make in
            make.top.equalToSuperview()
            make.right.equalToSuperview()
            make.width.equalToSuperview().dividedBy(2)
            make.height.equalToSuperview().dividedBy(2)
        }

        bottomLeftView.snp.makeConstraints { make in
            make.bottom.equalToSuperview()
            make.left.equalToSuperview()
            make.width.equalToSuperview().dividedBy(2)
            make.height.equalToSuperview().dividedBy(2)
        }

        bottomRightView.snp.makeConstraints { make in
            make.bottom.equalToSuperview()
            make.right.equalToSuperview()
            make.width.equalToSuperview().dividedBy(2)
            make.height.equalToSuperview().dividedBy(2)
        }
        
        pipButton.snp.makeConstraints { make in
            make.center.equalTo(view)
        }
        
        sizeButton.snp.makeConstraints { make in
            make.top.equalTo(self.pipButton.snp.bottom).offset(10)
            make.centerX.equalTo(self.pipButton.snp.centerX)
        }
    }
    
    private func initRtc() {
        guard let channelId = channelId else { return }
        rtcService = PixelBufferPIPService(channelId: channelId, uid: mockUid, localView: topLeftView)
        rtcService.videoFrameDelegte = self
        rtcService.rtcEngineDelegate = self
    }
    
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
}

@available(iOS 15.0, *)
extension PixelBufferPIPViewController: AVPictureInPictureControllerDelegate {
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
        view.bringSubviewToFront(pipButton)
        view.bringSubviewToFront(sizeButton)

        videoContainerView.snp.makeConstraints { make in
            make.left.top.bottom.right.equalTo(0)
        }
    }
}

@available(iOS 15.0, *)
extension PixelBufferPIPViewController: AgoraRtcEngineDelegate {
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinChannel channel: String, withUid uid: UInt, elapsed: Int) {
        isJoined = true
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinedOfUid uid: UInt, elapsed: Int) {
        if let view = displayViews.allObjects.first(where: { $0.uid == 0 }) {
            view.uid = uid
        }
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOfflineOfUid uid: UInt, reason: AgoraUserOfflineReason) {
        if let view = displayViews.allObjects.first(where: { $0.uid == uid }) {
            view.clean()
        }
    }
}

@available(iOS 15.0, *)
extension PixelBufferPIPViewController: AgoraVideoFrameDelegate {
    func onCapture(_ videoFrame: AgoraOutputVideoFrame, sourceType: AgoraVideoSourceType) -> Bool {
        if let view = displayViews.allObjects.first(where: { $0.uid == mockUid }), let pixelBuffer = videoFrame.pixelBuffer {
            view.renderVideoPixelBuffer(pixelBuffer: pixelBuffer, width: videoFrame.width, height: videoFrame.height)
        }
        
        return true
    }
    
    func onRenderVideoFrame(_ videoFrame: AgoraOutputVideoFrame, uid: UInt, channelId: String) -> Bool {
        if let view = displayViews.allObjects.first(where: { $0.uid == uid }) {
            view.renderFromVideoFrameData(videoData: videoFrame, uid: Int(uid))
        }
        
        return true
    }
}
