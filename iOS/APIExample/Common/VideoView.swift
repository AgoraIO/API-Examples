//
//  VideoView.swift
//  APIExample
//
//  Created by 张乾泽 on 2020/9/16.
//  Copyright © 2020 Agora Corp. All rights reserved.
//

import UIKit

extension Bundle {

    static func loadView<T>(fromNib name: String, withType type: T.Type) -> T {
        if let view = Bundle.main.loadNibNamed(name, owner: nil, options: nil)?.first as? T {
            return view
        }

        fatalError("Could not load view with type " + String(describing: type))
    }
}

class VideoView: UIView {

    @IBOutlet weak var videoView:UIView!
    @IBOutlet weak var placeholderLabel:UILabel!
    @IBOutlet weak var infoLabel:UILabel!

    func setPlaceholder(text:String) {
        placeholderLabel.text = text
    }
    
    func setInfo(text:String) {
        infoLabel.text = text
    }
    
    override func awakeFromNib() {
        super.awakeFromNib()
    }
}

class MetalVideoView: UIView {
    @IBOutlet weak var placeholder: UILabel!
    @IBOutlet weak var videoView: AgoraMetalRender!
    @IBOutlet weak var infolabel: UILabel!
    
    override func awakeFromNib() {
        super.awakeFromNib()
    }

    func setPlaceholder(text:String) {
        placeholder.text = text
    }
    
    func setInfo(text:String) {
        infolabel.text = text
    }
}
