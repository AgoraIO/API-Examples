//
//  ViewController.swift
//  APIExample
//
//  Created by 张乾泽 on 2020/4/16.
//  Copyright © 2020 Agora Corp. All rights reserved.
//

import UIKit
import Floaty

struct MenuSection {
    var name: String
    var rows:[MenuItem]
}

struct MenuItem {
    var name: String
    var entry: String = "EntryViewController"
    var storyboard: String = "Main"
    var controller: String
    var note: String = ""
}

class ViewController: AGViewController {
    var menus:[MenuSection] = [
        MenuSection(name: "Basic", rows: [
            MenuItem(name: "Join a channel (Token)".localized, storyboard: "JoinChannelAudioToken", controller: ""),
            MenuItem(name: "Join a channel (Audio)".localized, storyboard: "JoinChannelAudio", controller: "")
        ]),
        MenuSection(name: "Anvanced", rows: [
            MenuItem(name: "Voice Changer".localized, storyboard: "VoiceChanger", controller: ""),
            MenuItem(name: "Custom Audio Source".localized, storyboard: "CustomAudioSource", controller: ""),
            MenuItem(name: "Custom Audio Source(PCM)".localized, storyboard: "CustomPcmAudioSource", controller: "CustomPcmAudioSource"),
            MenuItem(name: "Custom Audio Render".localized, storyboard: "CustomAudioRender", controller: "CustomAudioRender"),
            MenuItem(name: "Raw Audio Data".localized, storyboard: "RawAudioData", controller: ""),
            MenuItem(name: "Audio Mixing".localized, storyboard: "AudioMixing", controller: ""),
            MenuItem(name: "Rhythm Player".localized, storyboard: "RhythmPlayer", controller: "RhythmPlayer"),
            MenuItem(name: "Precall Test".localized, storyboard: "PrecallTest", controller: ""),
            MenuItem(name: "Spatial Audio".localized, storyboard: "SpatialAudio", controller: "SpatialAudio"),
        ]),
    ]
    override func viewDidLoad() {
        super.viewDidLoad()
        Floaty.global.button.addItem(title: "Send Logs", handler: {item in
            LogUtils.writeAppLogsToDisk()
            let activity = UIActivityViewController(activityItems: [NSURL(fileURLWithPath: LogUtils.logFolder(), isDirectory: true)], applicationActivities: nil)
            activity.modalPresentationStyle = .popover
            if UIDevice.current.userInterfaceIdiom == .pad {
                activity.popoverPresentationController?.sourceView = Floaty.global.button
            }
            self.present(activity, animated: true, completion: nil)
        })
        
        Floaty.global.button.addItem(title: "Clean Up", handler: {item in
            LogUtils.cleanUp()
        })
        Floaty.global.button.isDraggable = true
        Floaty.global.show()
    }
    
    @IBAction func onSettings(_ sender:UIBarButtonItem) {
        let storyBoard: UIStoryboard = UIStoryboard(name: "Main", bundle: nil)
        guard let settingsViewController = storyBoard.instantiateViewController(withIdentifier: "settings") as? SettingsViewController else { return }
        
        settingsViewController.settingsDelegate = self
        settingsViewController.sectionNames = ["Video Configurations","Metadata", "Private cloud"]
        settingsViewController.sections = [
            [
                SettingsSelectParam(key: "resolution", label:"Resolution".localized, settingItem: GlobalSettings.shared.getSetting(key: "resolution")!, context: self),
                SettingsSelectParam(key: "fps", label:"Frame Rate".localized, settingItem: GlobalSettings.shared.getSetting(key: "fps")!, context: self),
                SettingsSelectParam(key: "orientation", label:"Orientation".localized, settingItem: GlobalSettings.shared.getSetting(key: "orientation")!, context: self),
                SettingsSelectParam(key: "role", label:"Pick Role".localized, settingItem: GlobalSettings.shared.getSetting(key: "role")!, context: self)
            ],
            [SettingsLabelParam(key: "sdk_ver", label: "SDK Version", value: "\(AgoraRtcEngineKit.getSdkVersion())")],
            [
                SettingsTextFieldParam(key: "ip",
                                       label: "IP Address",
                                       text: GlobalSettings.shared.getCache(key: "ip"),
                                       placeHolder: "please input IP"),
                SettingsSwitchParam(key: "report",
                                    label: "Log Report",
                                    isOn: false),
                SettingsTextFieldParam(key: "log_server_domain", label: "Log Server Domain",
                                       text: GlobalSettings.shared.getCache(key: "log_server_ip"),
                                       placeHolder: "please input log server IP"),
                SettingsTextFieldParam(key: "log_server_port", label: "Log Server Port",
                                       text: GlobalSettings.shared.getCache(key: "log_server_port"),
                                       placeHolder: "please input Server Port"),
                SettingsTextFieldParam(key: "log_server_path", label: "Log Server Path",
                                       text: GlobalSettings.shared.getCache(key: "log_server_path"),
                                       placeHolder: "please input Server Path"),
                SettingsSwitchParam(key: "log_server_https",
                                    label: "Use Https",
                                    isOn: false)
            ],
        ]
        self.navigationController?.pushViewController(settingsViewController, animated: true)
    }
}

extension ViewController: UITableViewDataSource {
    func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        return menus[section].rows.count
    }
    
    func numberOfSections(in tableView: UITableView) -> Int {
        return menus.count
    }
    
    func tableView(_ tableView: UITableView, titleForHeaderInSection section: Int) -> String? {
        return menus[section].name
    }
    
    func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        let cellIdentifier = "menuCell"
        var cell = tableView.dequeueReusableCell(withIdentifier: cellIdentifier)
        if cell == nil {
            cell = UITableViewCell(style: .default, reuseIdentifier: cellIdentifier)
        }
        cell?.textLabel?.text = menus[indexPath.section].rows[indexPath.row].name
        return cell!
    }
}

extension ViewController: UITableViewDelegate {
    func tableView(_ tableView: UITableView, didSelectRowAt indexPath: IndexPath) {
        tableView.deselectRow(at: indexPath, animated: true)
        
        let menuItem = menus[indexPath.section].rows[indexPath.row]
        let storyBoard: UIStoryboard = UIStoryboard(name: menuItem.storyboard, bundle: nil)
        
        if(menuItem.storyboard == "Main") {
            guard let entryViewController = storyBoard.instantiateViewController(withIdentifier: menuItem.entry) as? EntryViewController else { return }
            
            entryViewController.nextVCIdentifier = menuItem.controller
            entryViewController.title = menuItem.name
            entryViewController.note = menuItem.note
            self.navigationController?.pushViewController(entryViewController, animated: true)
        } else {
            let entryViewController:UIViewController = storyBoard.instantiateViewController(withIdentifier: menuItem.entry)
            entryViewController.title = menuItem.name
            self.navigationController?.pushViewController(entryViewController, animated: true)
        }
    }
}

extension ViewController: SettingsViewControllerDelegate {
    func didChangeValue(type: String, key: String, value: Any) {
        LogUtils.log(message: "type == \(type) key == \(key) value == \(value)", level: .info)
        if(type == "SettingsSelectCell") {
            guard let setting = value as? SettingItem else {return}
            LogUtils.log(message: "select \(setting.selectedOption().label) for \(key)", level: .info)
        }
        switch type {
        case "SettingsSelectCell":
            guard let setting = value as? SettingItem else {return}
            LogUtils.log(message: "select \(setting.selectedOption().label) for \(key)", level: .info)
            
        case "SettingsTextFieldCell":
            GlobalSettings.shared.cache[key] = value
            
        case "SettingsSwitchCell":
            GlobalSettings.shared.cache[key] = value
            
        default: break
        }
    }
}
