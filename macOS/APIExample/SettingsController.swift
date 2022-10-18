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
    
    @IBOutlet weak var proxyLabel: NSTextField!
    @IBOutlet weak var proxyPicker: NSPopUpButton!
    
    @IBOutlet weak var ipTextField: NSTextField!
    @IBOutlet weak var logReportSwitch: NSSwitch!
    @IBOutlet weak var logServerDomainTextField: NSTextField!
    @IBOutlet weak var logServerPortTextField: NSTextField!
    @IBOutlet weak var logServerPathTextField: NSTextField!
    @IBOutlet weak var useHttpsSwitch: NSSwitch!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do view setup here.
        self.resolutionLabel.cell?.title = "Resolution".localized
        self.resolutionPicker.addItems(withTitles: GlobalSettings.shared.resolutionSetting.options.map { $0.label })
        self.resolutionPicker.selectItem(at: GlobalSettings.shared.resolutionSetting.selected)
        
        self.fpsLabel.cell?.title = "Frame Rate".localized
        self.fpsPicker.addItems(withTitles: GlobalSettings.shared.fpsSetting.options.map { $0.label })
        self.fpsPicker.selectItem(at: GlobalSettings.shared.fpsSetting.selected)
        
        self.proxyLabel.cell?.title = "Enable Cloud Proxy".localized
        self.proxyPicker.addItems(withTitles: GlobalSettings.shared.proxySetting.options.map { $0.label })
        self.proxyPicker.selectItem(at: GlobalSettings.shared.proxySetting.selected)
        
        self.sdkVersion.cell?.title = "v\(AgoraRtcEngineKit.getSdkVersion())"
        
        ipTextField.stringValue = GlobalSettings.shared.getCache(key: "ip")
        logReportSwitch.state = GlobalSettings.shared.getCache(key: "report") == true ? .on : .off
        logServerDomainTextField.stringValue = GlobalSettings.shared.getCache(key: "log_server_domain")
        logServerPortTextField.stringValue = GlobalSettings.shared.getCache(key: "log_server_port")
        logServerPathTextField.stringValue = GlobalSettings.shared.getCache(key: "log_server_path")
        useHttpsSwitch.state = GlobalSettings.shared.getCache(key: "log_server_https") == true ? .on : .off
    }

    @IBAction func onResolutionChanged(_ sender: NSPopUpButton) {
        GlobalSettings.shared.resolutionSetting.selected = sender.indexOfSelectedItem
    }
    
    @IBAction func onFpsChanged(_ sender: NSPopUpButton) {
        GlobalSettings.shared.fpsSetting.selected = sender.indexOfSelectedItem
    }
    
    @IBAction func onProxyChanged(_ sender: NSPopUpButton) {
        GlobalSettings.shared.proxySetting.selected = sender.indexOfSelectedItem
    }
    
    @IBAction func OnIPAddressChanged(_ sender: NSTextField) {
        GlobalSettings.shared.cache["ip"] = sender.stringValue
    }
    @IBAction func onLogReportChanged(_ sender: NSSwitch) {
        GlobalSettings.shared.cache["report"] = sender.state == .on
    }
    @IBAction func onLogServerDomainChanged(_ sender: NSTextField) {
        GlobalSettings.shared.cache["log_server_domain"] = sender.stringValue
    }
    @IBAction func onLogServerPortChanged(_ sender: NSTextField) {
        GlobalSettings.shared.cache["log_server_port"] = sender.stringValue
    }
    @IBAction func onLogServerPathChanged(_ sender: NSTextField) {
        GlobalSettings.shared.cache["log_server_path"] = sender.stringValue
    }
    @IBAction func onUseHttpsChanged(_ sender: NSSwitch) {
        GlobalSettings.shared.cache["log_server_https"] = sender.state == .on
    }
}

