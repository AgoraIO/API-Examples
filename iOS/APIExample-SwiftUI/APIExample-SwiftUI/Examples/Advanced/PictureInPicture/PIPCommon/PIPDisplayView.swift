//
//  RTCContainerView.swift
//  APIExample-SwiftUI
//
//  Created by qinhui on 2024/8/12.
//
import SwiftUI
import Combine

protocol PIPController {

    /**
     Initializes the picture-in-picture controller
     pipSourceView: The view that needs to be displayed in the picture-in-picture window
     */
    init(pipSourceView: UIView)
    
    /**
     Change the picture-in-picture window size
     */
    func changePIPSize(size: () -> CGSize)
    
    /**
     Open and close picture-in-picture
     result: Picture-in-picture: true: on, flase: off
     */
    @discardableResult func togglePiP() -> Bool
}

/**
    @class      PIPDisplayView，SwiftUI Picture-in-picture view wrapper class
    @abstract   The picture-in-picture view is a UIKit-related API that allows you to bridge SwiftUI's controls to the PIP view (view with UIKit layout, class).
 */
struct PIPDisplayView: UIViewRepresentable {
    ///PIP controller, used to control picture-in-picture
    @ObservedObject var viewModel: PIPViewModel
    
    func makeUIView(context: Context) -> PIPBackgroundView {
        return viewModel.backgroundView
    }
    
    func updateUIView(_ uiView: PIPBackgroundView, context: Context) { }
}

/**
    @class      PIPViewModel，Picture-in-picture view control class
    @abstract   Provide the view that needs to be displayed in picture to PIPViewModel to quickly realize the picture-in-picture function (picture-in-picture window open or close, window              size switch)
 */
class PIPViewModel:NSObject, ObservableObject, PIPController {
    var backgroundView: PIPBackgroundView
    private var videoCallController: AVPictureInPictureVideoCallViewController?
    private var pipController: AVPictureInPictureController?
    
    required init(pipSourceView: UIView) {
        backgroundView = PIPBackgroundView(pipSourceView: pipSourceView)
        videoCallController = AVPictureInPictureVideoCallViewController()
        videoCallController?.preferredContentSize = UIScreen.main.bounds.size
        videoCallController?.view.backgroundColor = .clear
        videoCallController?.modalPresentationStyle = .overFullScreen
        
        super.init()
        self.setupPIP()
    }

    func setupPIP() {
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
    
    @discardableResult func togglePiP() -> Bool {
        guard let pipController = pipController else { return false }
        
        if pipController.isPictureInPictureActive {
            pipController.stopPictureInPicture()
        } else {
            pipController.startPictureInPicture()
        }
        
        return true
    }
}

/**
    @class     (inner class) PIPViewModel extension to handle draw-in-picture agent callbacks
    @abstract   Add or delete views through draw-in-picture proxy callbacks
 */
extension PIPViewModel: AVPictureInPictureControllerDelegate {
    func pictureInPictureControllerWillStartPictureInPicture(_ pictureInPictureController: AVPictureInPictureController) {
        guard let vc = pictureInPictureController.contentSource?.activeVideoCallContentViewController, let pipSourceView = backgroundView.pipSourceView else { return }
        
        vc.view.addSubview(pipSourceView)
        pipSourceView.frame = vc.view.bounds
    }
    
    func pictureInPictureControllerDidStopPictureInPicture(_ pictureInPictureController: AVPictureInPictureController) {
        guard let pipSourceView = backgroundView.pipSourceView else { return }
        
        pipSourceView.removeFromSuperview()
        backgroundView.addSubview(pipSourceView)
    }
}

/**
    @class      （Inner class) PIPBackgroundView
    @abstract   Picture-in-picture view wrapping class
 */
final class PIPBackgroundView: UIView {
    weak var pipSourceView: UIView?

    init(pipSourceView: UIView, frame: CGRect = .zero) {
        self.pipSourceView = pipSourceView
        super.init(frame: frame)
        if let view = self.pipSourceView {
            self.addSubview(view)
        }
    }

    required init?(coder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }

    override func layoutSubviews() {
        super.layoutSubviews()
        if let view = pipSourceView {
            view.frame = bounds
        }
    }
}
