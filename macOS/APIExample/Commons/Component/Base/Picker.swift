//
//  Picker.swift
//  APIExample
//
//  Created by XC on 2020/12/18.
//  Copyright © 2020 Agora Corp. All rights reserved.
//

import Cocoa

class Picker: NSView {

    @IBOutlet var contentView: NSView!
    @IBOutlet weak var label: NSTextField!
    @IBOutlet weak var picker: NSPopUpButton!
    
    private var listener: (() -> Void)?
    
    var isEnabled: Bool {
        get {
            picker.isEnabled
        }
        set {
            picker.isEnabled = newValue
        }
    }
    
    var indexOfSelectedItem: Int {
        get {
            picker.indexOfSelectedItem
        }
    }
    
    open func title() -> String {
        return "Label"
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
        let nib = NSNib(nibNamed: .init("Picker"), bundle: bundle)!
        nib.instantiate(withOwner: self, topLevelObjects: nil)

        addSubview(contentView)
        label.cell?.title = title()
        
        self.picker.target = self
        self.picker.action = #selector(onSelect)
    }
    
    @IBAction open func onSelect(_ sender: NSPopUpButton) {
        listener?()
    }
        
    func onSelectChanged(_ callback: @escaping () -> Void) {
        listener = callback
    }
}
