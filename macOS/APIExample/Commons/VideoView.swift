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

class VideoView: NSView,NibLoadable {
    @IBOutlet var placeholder: NSTextField!
    @IBOutlet var videocanvas: NSView!
    @IBOutlet var infolabel: NSTextField!
    
    var uid:UInt? {
        didSet {
            infolabel.stringValue = uid == nil ? "" : "\(uid!)"
        }
    }
    
    override func awakeFromNib() {
        super.awakeFromNib()
    }
}
