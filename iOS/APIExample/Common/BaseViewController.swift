//
//  BaseVC.swift
//  APIExample
//
//  Created by 张乾泽 on 2020/4/17.
//  Copyright © 2020 Agora Corp. All rights reserved.
//

#if os(iOS)
import UIKit
#else
import Cocoa
#endif
import AGEVideoLayout

#if os(macOS)
protocol ViewControllerCloseDelegate: NSObjectProtocol {
    func viewControllerNeedClose(_ liveVC: AGViewController)
}
#endif

class BaseViewController: AGViewController {
    #if os(macOS)
    var closeDelegate: ViewControllerCloseDelegate?
    #endif
    
    override func viewDidLoad() {
        #if os(iOS)
        self.navigationItem.rightBarButtonItem = UIBarButtonItem(title: "Show Log",
                                                                 style: .plain,
                                                                 target: self,
                                                                 action: #selector(showLog))
        #endif
        LogUtils.removeAll()
    }
    
    #if os(iOS)
    @objc func showLog() {
        let storyBoard: UIStoryboard = UIStoryboard(name: "Main", bundle: nil)
        let newViewController = storyBoard.instantiateViewController(withIdentifier: "LogViewController")
        self.present(newViewController, animated: true, completion: nil)
    }
    
    #else
    
    override func viewDidAppear() {
        super.viewDidAppear()
        view.window?.delegate = self
    }
    #endif
    
    func showAlert(title: String? = nil, message: String) {
        #if os(iOS)
        let alertController = UIAlertController(title: title, message: message, preferredStyle: .alert)
        let action = UIAlertAction(title: "OK", style: .cancel, handler: nil)
        alertController.addAction(action)
        self.present(alertController, animated: true, completion: nil)
        
        #else
        
        let alert = NSAlert()
        
        var full = message
        if let title = title {
            full = title + full
        }
        
        alert.messageText = full
        alert.addButton(withTitle: "OK")
        alert.alertStyle = .informational
        guard let window = NSApplication.shared.windows.first else {
            return
        }
        alert.beginSheetModal(for: window, completionHandler: nil)
        #endif
    }
}

#if os(macOS)
extension BaseViewController: NSWindowDelegate {
    func windowShouldClose(_ sender: NSWindow) -> Bool {
        closeDelegate?.viewControllerNeedClose(self)
        return false
    }
}
#endif

class RenderViewController: AGViewController {
    private var streamViews: [AGView]?
    
    func layoutStream(views: [AGView]) {
        self.streamViews = views
        let container = self.view as! AGEVideoContainer
        let count = views.count
        
        var layout: AGEVideoLayout
        
        if count == 1 {
            layout = AGEVideoLayout(level: 0)
                .itemSize(.scale(CGSize(width: 1, height: 1)))
        } else if count == 2 {
            layout = AGEVideoLayout(level: 0)
                .itemSize(.scale(CGSize(width: 0.5, height: 1)))
        } else if count > 2, count < 5 {
            layout = AGEVideoLayout(level: 0)
                .itemSize(.scale(CGSize(width: 0.5, height: 0.5)))
        } else {
            return
        }
        
        container.listCount { [unowned self] (level) -> Int in
            return self.streamViews?.count ?? 0
        }.listItem { [unowned self] (index) -> AGEView in
            return self.streamViews![index.item]
        }
        
        container.setLayouts([layout])
    }
}

class BasicVideoViewController: BaseViewController {
    var renderVC: RenderViewController!
    
    override func viewDidLoad() {
        super.viewDidLoad()
    }
    
    override func prepare(for segue: AGStoryboardSegue, sender: Any?) {
        guard let identifier = segue.identifier else {
            return
        }
        
        switch identifier {
        case "RenderViewController":
            let vc = segue.destinationController as! RenderViewController
            renderVC = vc
        default:
            break
        }
    }
}
