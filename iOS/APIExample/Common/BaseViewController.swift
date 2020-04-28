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
        self.navigationItem.rightBarButtonItem = UIBarButtonItem(title: "Show Log", style: .plain, target: self, action: #selector(showLog))
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
    
}

class RTCViewController: BaseViewController {
    override func viewDidLoad() {
        super.viewDidLoad()
    }
}
