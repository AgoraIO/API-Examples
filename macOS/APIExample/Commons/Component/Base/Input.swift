//
//  Input.swift
//  APIExample
//
//  Created by XC on 2020/12/21.
//  Copyright © 2020 Agora Corp. All rights reserved.
//

import Cocoa

class Input: NSView {

    @IBOutlet var contentView: NSView!
    @IBOutlet weak var label: NSTextField!
    @IBOutlet weak var field: NSTextField!
    
    var isEnabled: Bool {
        get {
            field.isEnabled
        }
        set {
            field.isEnabled = newValue
        }
    }
    
    var stringValue: String {
        get {
            field.stringValue
        }
        set {
            field.stringValue = newValue
        }
    }
    
    override init(frame frameRect: NSRect) {
        super.init(frame: frameRect)
        initUI()
    }
    
    required init?(coder: NSCoder) {
        super.init(coder: coder)
        initUI()
    }
        
    open func initUI() {
        let bundle = Bundle(for: type(of: self))
        let nib = NSNib(nibNamed: .init("Input"), bundle: bundle)!
        nib.instantiate(withOwner: self, topLevelObjects: nil)

        addSubview(contentView)
        label.cell?.title = title()
        field.placeholderString = placeholderString()
    }
    
    open func title() -> String {
        return "Label"
    }
    
    open func placeholderString() -> String {
        return ""
    }
}
