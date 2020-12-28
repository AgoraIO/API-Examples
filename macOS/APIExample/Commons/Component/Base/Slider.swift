//
//  Slider.swift
//  APIExample
//
//  Created by XC on 2020/12/22.
//  Copyright © 2020 Agora Corp. All rights reserved.
//

import Cocoa

class Slider: NSView {

    @IBOutlet var contentView: NSView!
    @IBOutlet weak var label: NSTextField!
    @IBOutlet weak var slider: NSSlider!
    
    private var listener: (() -> Void)?
    
    var isEnabled: Bool {
        get {
            slider.isEnabled
        }
        set {
            slider.isEnabled = newValue
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
        let nib = NSNib(nibNamed: .init("Slider"), bundle: bundle)!
        nib.instantiate(withOwner: self, topLevelObjects: nil)

        addSubview(contentView)
        label.cell?.title = title()
        
        self.slider.target = self
        self.slider.action = #selector(onChange)
    }
    
    @IBAction open func onChange(_ sender: NSSlider) {
        listener?()
    }
        
    func onSliderChanged(_ callback: @escaping () -> Void) {
        listener = callback
    }
}
