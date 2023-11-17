//
//  VideoView.swift
//  OpenVideoCall
//
//  Created by GongYuhua on 2/14/16.
//  Copyright © 2016 Agora. All rights reserved.
//

import Cocoa

protocol NibLoadable {
    static var nibName: String? { get }
    static func createFromNib(in bundle: Bundle) -> Self?
}

extension NibLoadable where Self: NSView {

    static var nibName: String? {
        return String(describing: Self.self)
    }

    static func createFromNib(in bundle: Bundle = Bundle.main) -> Self? {
        guard let nibName = nibName else { return nil }
        var topLevelArray: NSArray? = nil
        bundle.loadNibNamed(NSNib.Name(nibName), owner: self, topLevelObjects: &topLevelArray)
        guard let results = topLevelArray else { return nil }
        let views = Array<Any>(results).filter { $0 is Self }
        return views.last as? Self
    }
}

class VideoView: NSView, NibLoadable {
    @IBOutlet weak var placeholder: NSTextField!
    @IBOutlet weak var videocanvas: NSView!
    @IBOutlet weak var infolabel: NSTextField!
    @IBOutlet weak var statsLabel:NSTextField!
    
    var uid:UInt? {
        didSet {
            infolabel.stringValue = uid == nil ? "" : "\(uid!)"
        }
    }
    
    
    var audioOnly:Bool = false
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
            guard let stats = statsInfo else {return}
            if Thread.isMainThread {
                statsLabel.stringValue = stats.description(audioOnly: audioOnly)
            } else {
                DispatchQueue.main.async {
                    self.statsLabel.stringValue = stats.description(audioOnly: self.audioOnly)
                }
            }
        }
    }
    var type:StreamType?
    
    override func awakeFromNib() {
        super.awakeFromNib()
    }
}

class MetalVideoView: NSView,NibLoadable {
    @IBOutlet weak var placeholder: NSTextField!
    @IBOutlet weak var videocanvas: AgoraMetalRender!
    @IBOutlet weak var infolabel: NSTextField!
    
    var uid:UInt? {
        didSet {
            infolabel.stringValue = uid == nil ? "" : "\(uid!)"
        }
    }
    
    override func awakeFromNib() {
        super.awakeFromNib()
    }
}

class SampleBufferDisplayView: NSView,NibLoadable {
    @IBOutlet weak var videoView: AgoraSampleBufferRender!
    
    override func awakeFromNib() {
        super.awakeFromNib()
    }
}
