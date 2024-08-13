//
//  RTCContainerView.swift
//  APIExample-SwiftUI
//
//  Created by qinhui on 2024/8/12.
//
import SwiftUI
import Combine

struct PIPDisplayView: UIViewRepresentable {
    @ObservedObject var viewModel: PIPViewModel
    
    func makeUIView(context: Context) -> PIPBackgroundView {
        return viewModel.backgroundView
    }
    
    func updateUIView(_ uiView: PIPBackgroundView, context: Context) {
        // 在此处可以更新UIView，如果需要
    }
}

class PIPBackgroundView: UIView {
    var pipSourceView: UIView

    init(pipSourceView: UIView, frame: CGRect = .zero) {
        self.pipSourceView = pipSourceView
        super.init(frame: frame)
        self.addSubview(self.pipSourceView)
    }

    required init?(coder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }

    override func layoutSubviews() {
        super.layoutSubviews()
        pipSourceView.frame = bounds
    }
}

class PIPViewModel:NSObject, ObservableObject {
    var backgroundView: PIPBackgroundView
    private var videoCallController: AVPictureInPictureVideoCallViewController?
    private var pipController: AVPictureInPictureController?

    init(pipSourceView: UIView) {
        backgroundView = PIPBackgroundView(pipSourceView: pipSourceView)
        videoCallController = AVPictureInPictureVideoCallViewController()
        videoCallController?.preferredContentSize = UIScreen.main.bounds.size
        videoCallController?.view.backgroundColor = .clear
        videoCallController?.modalPresentationStyle = .overFullScreen
        
        super.init()
        self.setupPiP()
    }

    func setupPiP() {
        guard let videoCallController = videoCallController else { return }
        pipController = AVPictureInPictureController(contentSource: .init(activeVideoCallSourceView: backgroundView, contentViewController: videoCallController))
        pipController?.canStartPictureInPictureAutomaticallyFromInline = true
        pipController?.delegate = self
        pipController?.setValue(1, forKey: "controlsStyle")
    }
    
    func changePIPSize(size: () -> CGSize) {
        guard let vc = videoCallController else { return }
        vc.preferredContentSize = size()
    }

    func togglePiP() {
        guard let pipController = pipController else { return }
        if pipController.isPictureInPictureActive {
            pipController.stopPictureInPicture()
        } else {
            pipController.startPictureInPicture()
        }
    }
}

extension PIPViewModel: AVPictureInPictureControllerDelegate {
    func pictureInPictureControllerWillStartPictureInPicture(_ pictureInPictureController: AVPictureInPictureController) {
        guard let vc = pictureInPictureController.contentSource?.activeVideoCallContentViewController else { return }
        
        vc.view.addSubview(backgroundView.pipSourceView)
        backgroundView.pipSourceView.frame = vc.view.bounds
    }
    
    func pictureInPictureControllerDidStopPictureInPicture(_ pictureInPictureController: AVPictureInPictureController) {
        backgroundView.pipSourceView.removeFromSuperview()
        backgroundView.addSubview(backgroundView.pipSourceView)
    }
}
