//
//  ViewController.swift
//  PIPDemo
//
//  Created by qinhui on 2024/8/7.
//

import UIKit

class ChannelViewController<T:PIPBaseViewController>: UIViewController {
    lazy var textField: UITextField = {
        let t = UITextField()
        t.placeholder = "输入房间号"
        t.borderStyle = .line
        t.backgroundColor = .orange
        return t
    }()
    
    var pipCls: T.Type?
    
    lazy var button: UIButton = {
        let b = UIButton(type: .custom)
        b.setTitle("加入房间", for: .normal)
        b.setTitleColor(.blue, for: .normal)
        b.addTarget(self, action: #selector(joinAction), for: .touchUpInside)
        return b
    }()
    
    override func viewDidLoad() {
        super.viewDidLoad()
        view.backgroundColor = .white
        
        view.addSubview(textField)
        view.addSubview(button)
        
        button.snp.makeConstraints { make in
            make.center.equalTo(view)
        }
        
        textField.snp.makeConstraints { make in
            make.bottom.equalTo(button.snp.top).offset(-50)
            make.centerX.equalTo(button)
            make.width.equalTo(150)
            make.height.equalTo(30)
        }
    }

    @objc func joinAction() {
        guard let channelId = textField.text, let cls = pipCls else { return }
        
        let vc = cls.init()
        vc.channelId = channelId
        self.navigationController?.pushViewController(vc, animated: true)
    }
    
    override func touchesBegan(_ touches: Set<UITouch>, with event: UIEvent?) {
        view.endEditing(true)
    }
}
