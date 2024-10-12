//
//  AlertManager.swift
//  Scene-Examples
//
//  Created by zhaoyongqiang on 2021/11/10.
//

import UIKit
import AVFoundation

extension UIScreen {
    static var currentScreenSize: CGSize {
        // Get the main window's available size
        guard let window = UIApplication.shared.windows.first(where: { $0.isKeyWindow }) else {
            return CGSize.zero
        }
        
        // Calculate available width and height
        let bounds = window.bounds
        let insets = window.safeAreaInsets
        
        let width = bounds.width - insets.left - insets.right
        let height = bounds.height - insets.top - insets.bottom
        
        return CGSize(width: width, height: height)
    }
}

let cl_screenWidht = UIScreen.currentScreenSize.width
let cl_screenHeight = UIScreen.currentScreenSize.height
class AlertManager: NSObject {
    private struct AlertViewCache {
        var view: UIView?
        var index: Int = 0
    }
    enum AlertPosition {
        case center
        case bottom
    }
    private static var vc: UIViewController?
    private static var containerView: UIView?
    private static var currentPosition: AlertPosition = .center
    private static var viewCache: [AlertViewCache] = []
    private static var bottomAnchor: NSLayoutConstraint?
    
    public static func show(view: UIView,
                            alertPostion: AlertPosition = .center,
                            didCoverDismiss: Bool = true) {
        let index = viewCache.isEmpty ? 0 : viewCache.count
        viewCache.append(AlertViewCache(view: view, index: index))
        currentPosition = alertPostion
        if vc == nil {
            containerView = UIButton(frame: CGRect(x: 0, y: 0, width: cl_screenWidht, height: cl_screenHeight))
            containerView?.backgroundColor = UIColor(red: 0.0/255, green: 0.0/255, blue: 0.0/255, alpha: 0.0)
        }
        if didCoverDismiss {
            (containerView as? UIButton)?.addTarget(self, 
                                                    action: #selector(tapView),
                                                    for: .touchUpInside)
        }
        guard let containerView = containerView else { return }
        containerView.addSubview(view)
        view.translatesAutoresizingMaskIntoConstraints = false
        view.alpha = 0
        if alertPostion == .center {
            view.centerXAnchor.constraint(equalTo: containerView.centerXAnchor).isActive = true
            view.centerYAnchor.constraint(equalTo: containerView.centerYAnchor).isActive = true
        } else {
            bottomAnchor = view.bottomAnchor.constraint(equalTo: containerView.bottomAnchor)
            view.leadingAnchor.constraint(equalTo: containerView.leadingAnchor).isActive = true
            view.trailingAnchor.constraint(equalTo: containerView.trailingAnchor).isActive = true
        }
        if vc == nil {
            vc = UIViewController()
            vc?.view.backgroundColor = UIColor.clear
            vc?.view.addSubview(containerView)
            vc?.modalPresentationStyle = .custom
            UIViewController.cl_topViewController()?.present(vc ?? UIViewController(), animated: false) {
                showAlertPostion(alertPostion: alertPostion, view: view)
            }
        } else {
            showAlertPostion(alertPostion: alertPostion, view: view)
        }
        // 注册键盘出现通知
        NotificationCenter.default.addObserver(self, 
                                               selector: #selector(keyboardWillShow(notification:)),
                                               name: UIApplication.keyboardWillShowNotification,
                                               object: nil)
        
        // 注册键盘隐藏通知
        NotificationCenter.default.addObserver(self, 
                                               selector: #selector(keyboardWillHide(notification:)),
                                               name: UIApplication.keyboardWillHideNotification, 
                                               object: nil)
    }

    private static func showAlertPostion(alertPostion: AlertPosition, 
                                         view: UIView) {
        containerView?.layoutIfNeeded()
        if alertPostion == .center {
            showCenterView(view: view)
        } else {
            bottomAnchor?.constant = view.frame.height
            bottomAnchor?.isActive = true
            containerView?.layoutIfNeeded()
            showBottomView(view: view)
        }
    }
    
    private static func showCenterView(view: UIView) {
        if !viewCache.isEmpty {
            viewCache.forEach({ $0.view?.alpha = 0 })
        }
        UIView.animate(withDuration: 0.25, animations: {
            containerView?.backgroundColor = UIColor(red: 0.0/255,
                                                     green: 0.0/255,
                                                     blue: 0.0/255,
                                                     alpha: 0.5)
            view.alpha = 1.0
        })
    }
    
    private static func showBottomView(view: UIView) {
        if !viewCache.isEmpty {
            viewCache.forEach({ $0.view?.alpha = 0 })
        }
        view.alpha = 1.0
        bottomAnchor?.constant = 0
        bottomAnchor?.isActive = true
        UIView.animate(withDuration: 0.25, animations: {
            containerView?.backgroundColor = UIColor(red: 0.0/255,
                                                    green: 0.0/255,
                                                    blue: 0.0/255,
                                                    alpha: 0.5)
            containerView?.superview?.layoutIfNeeded()
        })
    }

    static func updateViewHeight() {
        UIView.animate(withDuration: 0.25, animations: {
            containerView?.layoutIfNeeded()
        })
    }
    
    static func hiddenView(all: Bool = true, 
                           completion: (() -> Void)? = nil) {
        if currentPosition == .bottom {
            guard let lastView = viewCache.last?.view else { return }
            bottomAnchor?.constant = lastView.frame.height
            bottomAnchor?.isActive = true
        }
        UIView.animate(withDuration: 0.25, animations: {
            if all || viewCache.isEmpty {
                containerView?.backgroundColor = UIColor(red: 255.0/255,
                                                         green: 255.0/255,
                                                         blue: 255.0/255,
                                                         alpha: 0.0)
                containerView?.layoutIfNeeded()
            }
            if currentPosition == .center {
                viewCache.last?.view?.alpha = 0
            }
        }, completion: { (_) in
            if all || viewCache.isEmpty {
                vc?.dismiss(animated: false, completion: completion)
                vc = nil
            } else {
                viewCache.removeLast()
                viewCache.last?.view?.alpha = 1
            }
        })
    }
    
    @objc
    private static func tapView() {
        DispatchQueue.main.asyncAfter(deadline: DispatchTime(uptimeNanoseconds: UInt64(0.1))) {
            self.hiddenView()
        }
    }
    
    private static var originFrame: CGRect = .zero
    // 键盘显示
    @objc 
    private static func keyboardWillShow(notification: Notification) {
        let keyboardHeight = (notification.userInfo?["UIKeyboardBoundsUserInfoKey"] as? CGRect)?.height
        guard let viewHeight = containerView?.frame.height else { return }
        let y = cl_screenHeight - (keyboardHeight ?? 304) - viewHeight
        if originFrame.origin.y != y {
            originFrame = containerView?.frame ?? .zero
        }
        UIView.animate(withDuration: 0.25) {
            containerView?.frame.origin.y = y
        }
    }
    // 键盘隐藏
    @objc private static func keyboardWillHide(notification: Notification) {
        UIView.animate(withDuration: 0.25) {
            containerView?.frame = originFrame
        } completion: { _ in
            if currentPosition == .bottom {
                hiddenView()
            }
        }
    }
}
