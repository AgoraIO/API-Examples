//
//  SettingsController.swift
//  APIExample
//
//  Created by XC on 2020/12/15.
//  Copyright © 2020 Agora Corp. All rights reserved.
//

import Cocoa

class SettingsController: BaseViewController {
        
    @IBOutlet weak var resolutionLabel: NSTextField!
    @IBOutlet weak var resolutionPicker: NSPopUpButton!
    
    @IBOutlet weak var fpsLabel: NSTextField!
    @IBOutlet weak var fpsPicker: NSPopUpButton!
    
    @IBOutlet weak var sdkVersionLabel: NSTextField!
    @IBOutlet weak var sdkVersion: NSTextField!
    
    var selections: [SettingItem] = [
        GlobalSettings.shared.settings["resolution"]!,
        GlobalSettings.shared.settings["fps"]!
    ]
        
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do view setup here.
        self.resolutionLabel.cell?.title = "Resolution".localized
        self.resolutionPicker.addItems(withTitles: selections[0].options.map({ (option: SettingItemOption) -> String in
            return option.label
        }))
        self.resolutionPicker.selectItem(at: selections[0].selected)
        
        self.fpsLabel.cell?.title = "Frame Rate".localized
        self.fpsPicker.addItems(withTitles: selections[1].options.map({ (option: SettingItemOption) -> String in
            return option.label
        }))
        self.fpsPicker.selectItem(at: selections[1].selected)
        
        self.sdkVersion.cell?.title = AgoraRtcEngineKit.getSdkVersion()
    }

    @IBAction func onResolutionChanged(_ sender: NSPopUpButton) {
        selections[0].selected = sender.indexOfSelectedItem
    }
    
    @IBAction func onFpsChanged(_ sender: NSPopUpButton) {
        selections[1].selected = sender.indexOfSelectedItem
    }
}

