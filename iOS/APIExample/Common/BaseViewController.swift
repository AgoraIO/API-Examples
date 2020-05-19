//
//  BaseVC.swift
//  APIExample
//
//  Created by 张乾泽 on 2020/4/17.
//  Copyright © 2020 Agora Corp. All rights reserved.
//

import Foundation
import UIKit
import AGEVideoLayout

class BaseViewController: UIViewController {
    override func viewDidLoad() {
        self.navigationItem.rightBarButtonItem = UIBarButtonItem(title: "Show Log",
                                                                 style: .plain,
                                                                 target: self,
                                                                 action: #selector(showLog))
        LogUtils.removeAll()
    }
    
    @objc func showLog() {
        let storyBoard: UIStoryboard = UIStoryboard(name: "Main", bundle: nil)
        let newViewController = storyBoard.instantiateViewController(withIdentifier: "LogViewController")
        self.present(newViewController, animated: true, completion: nil)
    }
    
    func showAlert(title: String?, msg:String) {
        let alertController = UIAlertController(title: title, message: msg, preferredStyle: .alert)
        let action = UIAlertAction(title: "OK", style: .cancel, handler: nil)
        alertController.addAction(action)
        self.present(alertController, animated: true, completion: nil)
    }
}

class RenderViewController: UIViewController {
    private var streamViews: [UIView]?
    
    override func viewDidLoad() {
        super.viewDidLoad()
    }
    
    func layoutStream(views: [UIView]) {
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
    
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        guard let identifier = segue.identifier else {
            return
        }
        
        switch identifier {
        case "RenderViewController":
            let vc = segue.destination as! RenderViewController
            renderVC = vc
        default:
            break
        }
    }
}
