//
//  KtvCopyrightMusic.swift
//  APIExample
//
//  Created by zhaoyongqiang on 2022/10/20.
//  Copyright © 2022 Agora Corp. All rights reserved.
//

import UIKit

class KtvCopyrightMusic: UIViewController {

    private let urlString = "https://doc.shengwang.cn/doc/online-ktv/ios/ktv-scenario/get-started/integrate-ktvapi"
    
    override func viewDidLoad() {
        super.viewDidLoad()

    }
    @IBAction func onTapKtvCopyrightButton(_ sender: Any) {
        guard let url = URL(string: urlString) else { return }
        UIApplication.shared.open(url, options: [:], completionHandler: nil)
    }
}
