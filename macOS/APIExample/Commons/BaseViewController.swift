//
//  BaseVC.swift
//  APIExample
//
//  Created by 张乾泽 on 2020/4/17.
//  Copyright © 2020 Agora Corp. All rights reserved.
//

import Cocoa
import AGEVideoLayout


class BaseViewController: NSViewController {
    var configs: [String:Any] = [:]
    override func viewDidLoad() {
//        self.navigationItem.rightBarButtonItem = UIBarButtonItem(title: "Show Log",
//                                                                 style: .plain,
//                                                                 target: self,
//                                                                 action: #selector(showLog))
//        LogUtils.removeAll()
    }
    
    func showAlert(title: String? = nil, message: String) {
        let alert = NSAlert()
        alert.alertStyle = .critical
        alert.addButton(withTitle: "OK")
        if let stitle = title {
            alert.messageText = stitle
        }
        alert.informativeText = message
        
        alert.runModal()
    }
    
    func getAudioLabel(uid:UInt, isLocal:Bool) -> String {
        return "AUDIO ONLY\n\(isLocal ? "Local" : "Remote")\n\(uid)"
    }
    
    func viewWillBeRemovedFromSplitView() {
        
    }
}

extension AGEVideoContainer {
    func layoutStream(views: [NSView]) {
        let count = views.count
        
        var layout: AGEVideoLayout
        
        switch count {
        case 1:
            layout = AGEVideoLayout(level: 0)
            .itemSize(.scale(CGSize(width: 1, height: 1)))
            break
        case 2:
            layout = AGEVideoLayout(level: 0)
                .itemSize(.scale(CGSize(width: 1, height: 0.5)))
            break
        case 4:
            layout = AGEVideoLayout(level: 0)
                .itemSize(.scale(CGSize(width: 0.5, height: 0.5)))
            break
        case 9:
            layout = AGEVideoLayout(level: 0)
                .itemSize(.scale(CGSize(width: 0.33, height: 0.33)))
            break
        case 16:
            layout = AGEVideoLayout(level: 0)
                .itemSize(.scale(CGSize(width: 0.25, height: 0.25)))
            break
        default:
            return
        }
        
        self.listCount { (level) -> Int in
            return views.count
        }.listItem { (index) -> AGEView in
            return views[index.item]
        }
        
        self.setLayouts([layout])
    }
    
    func layoutStream2(views: [NSView]) {
        let count = views.count
        
        var layout: AGEVideoLayout
        
        switch count {
        case 2:
            layout = AGEVideoLayout(level: 0)
                .itemSize(.scale(CGSize(width: 0.5, height: 1)))
            break
        default:
            return
        }
        
        self.listCount { (level) -> Int in
            return views.count
        }.listItem { (index) -> AGEView in
            return views[index.item]
        }
        
        self.setLayouts([layout])
    }
    
    func layoutStream3x3(views: [NSView]) {
        let count = views.count
        
        var layout: AGEVideoLayout
        
        if count > 9  {
            return
        } else {
            layout = AGEVideoLayout(level: 0)
                .itemSize(.scale(CGSize(width: 0.33, height: 0.33)))
        }
        
        self.listCount { (level) -> Int in
            return views.count
        }.listItem { (index) -> AGEView in
            return views[index.item]
        }
        
        self.setLayouts([layout])
    }
}
