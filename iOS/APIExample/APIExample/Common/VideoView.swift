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
    
    static func loadVideoView(type:VideoView.StreamType, audioOnly:Bool) -> VideoView {
        let view = Bundle.loadView(fromNib: "VideoView", withType: VideoView.self)
        view.audioOnly = audioOnly
        view.type = type
        if(type.isLocal()) {
            view.statsInfo = StatisticsInfo(type: .local(StatisticsInfo.LocalInfo()))
        } else {
            view.statsInfo = StatisticsInfo(type: .remote(StatisticsInfo.RemoteInfo()))
        }
        return view
    }
}

class VideoView: UIView {

    @IBOutlet weak var videoView:UIView!
    @IBOutlet weak var placeholderLabel:UILabel!
    @IBOutlet weak var infoLabel:UILabel!
    @IBOutlet weak var statsLabel:UILabel!
    var audioOnly:Bool = false
    var uid:UInt = 0
    enum StreamType {
        case local
        case remote
        
        func isLocal() -> Bool{
            switch self {
            case .local:  return true
            case .remote: return false
            }
        }
    }
    var statsInfo:StatisticsInfo? {
        didSet{
            statsLabel.text = statsInfo?.description(audioOnly: audioOnly)
        }
    }
    var type:StreamType?
    
    func setPlaceholder(text:String) {
        placeholderLabel.text = text
    }
    
    func setInfo(text:String) {
        infoLabel.text = text
    }
    
    override func awakeFromNib() {
        super.awakeFromNib()
        statsLabel.layer.shadowColor = UIColor.appColor(.textShadow)?.cgColor
        statsLabel.layer.shadowOffset = CGSize(width: 1, height: 1)
        statsLabel.layer.shadowRadius = 1.0
        statsLabel.layer.shadowOpacity = 0.7
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

class SampleBufferDisplayView: UIView {
    @IBOutlet weak var placeholder: UILabel!
    @IBOutlet weak var videoView: AgoraSampleBufferRender!
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
