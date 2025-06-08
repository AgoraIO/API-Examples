//
//  ViewController.swift
//  APIExample
//
//  Created by 张乾泽 on 2020/4/16.
//  Copyright © 2020 Agora Corp. All rights reserved.
//

import UIKit
import Floaty
import AgoraRtcKit

struct MenuSection {
    var name: String
    var rows: [MenuItem]
}

struct MenuItem {
    var name: String
    var entry: String = "EntryViewController"
    var storyboard: String = "Main"
    var controller: String
    var note: String = ""
}

class ViewController: AGViewController {
    var menus: [MenuSection] = [
        MenuSection(name: "Basic", rows: [
            MenuItem(name: "Join a channel (Token)".localized, storyboard: "JoinChannelVideoToken", controller: ""),
            MenuItem(name: "Join a channel (Video)".localized, storyboard: "JoinChannelVideo", controller: ""),
            MenuItem(name: "Join a channel (Audio)".localized, storyboard: "JoinChannelAudio", controller: ""),
            MenuItem(name: "Local or remote recording".localized, storyboard: "JoinChannelVideoRecorder", controller: "")
        ]),
        MenuSection(name: "Anvanced", rows: [
//            MenuItem(name: "Group Video Chat".localized, storyboard: "VideoChat", controller: "VideoChat"),
            MenuItem(name: "Live Streaming".localized, storyboard: "LiveStreaming", controller: "LiveStreaming"),
            MenuItem(name: "RTMP Streaming".localized, storyboard: "RTMPStreaming", controller: "RTMPStreaming"),
            MenuItem(name: "Fusion CDN Streaming".localized, storyboard: "FusionCDN", controller: "FusionCDN"),
            MenuItem(name: "Video Metadata".localized, storyboard: "VideoMetadata", controller: "VideoMetadata".localized),
            MenuItem(name: "Voice Changer".localized, storyboard: "VoiceChanger", controller: ""),
            MenuItem(name: "Custom Audio Source".localized, storyboard: "CustomPcmAudioSource", controller: "CustomPcmAudioSource"),
            MenuItem(name: "Custom Audio Render".localized, storyboard: "CustomAudioRender", controller: "CustomAudioRender"),
            MenuItem(name: "Custom Video Source(Push)".localized, storyboard: "CustomVideoSourcePush", controller: "CustomVideoSourcePush"),
            MenuItem(name: "Custom Video Source(Multi)".localized,
                     storyboard: "CustomVideoSourcePushMulti",
                     controller: "CustomVideoSourcePushMulti"),
            MenuItem(name: "Custom Video Render".localized, storyboard: "CustomVideoRender", controller: "CustomVideoRender"),
//            MenuItem(name: "Raw Media Data".localized, storyboard: "RawMediaData", controller: "RawMediaData"),
            MenuItem(name: "Raw Audio Data".localized, storyboard: "RawAudioData", controller: ""),
            MenuItem(name: "Raw Video Data".localized, storyboard: "RawVideoData", controller: ""),
            MenuItem(name: "Picture In Picture(require iOS15)".localized, storyboard: "PictureInPicture", controller: "PictureInPicture"),
            MenuItem(name: "Simple Filter Extension".localized, storyboard: "SimpleFilter", controller: "SimpleFilter"),
            MenuItem(name: "Quick Switch Channel".localized, controller: "QuickSwitchChannel"),
            MenuItem(name: "Join Multiple Channels".localized, storyboard: "JoinMultiChannel", controller: "JoinMultiChannel"),
            MenuItem(name: "Stream Encryption".localized, storyboard: "StreamEncryption", controller: ""),
            MenuItem(name: "Audio Mixing".localized, storyboard: "AudioMixing", controller: ""),
            MenuItem(name: "Precall Test".localized, storyboard: "PrecallTest", controller: ""),
            MenuItem(name: "Media Player".localized, storyboard: "MediaPlayer", controller: ""),
            MenuItem(name: "Screen Share".localized, storyboard: "ScreenShare", controller: ""),
            MenuItem(name: "Local Composite Graph".localized, storyboard: "LocalCompositeGraph", controller: ""),
            MenuItem(name: "Video Process".localized, storyboard: "VideoProcess", controller: "VideoProcess"),
            MenuItem(name: "Agora Beauty".localized, storyboard: "AgoraBeauty", controller: "AgoraBeauty"),
            MenuItem(name: "Rhythm Player".localized, storyboard: "RhythmPlayer", controller: "RhythmPlayer"),
            MenuItem(name: "Create Data Stream".localized, storyboard: "CreateDataStream", controller: ""),
            MenuItem(name: "Media Channel Relay".localized, storyboard: "MediaChannelRelay", controller: ""),
            MenuItem(name: "Spatial Audio".localized, storyboard: "SpatialAudio", controller: "SpatialAudioMain"),
            MenuItem(name: "Content Inspect".localized, storyboard: "ContentInspect", controller: ""),
            MenuItem(name: "Mutli Camera(iOS13.0+)".localized, storyboard: "MutliCamera", controller: "MutliCamera"),
            MenuItem(name: "Ktv copyright music".localized, 
                     entry: "KtvCopyrightMusic",
                     storyboard: "KtvCopyrightMusic",
                     controller: "KtvCopyrightMusic"),
            MenuItem(name: "Third Beautify".localized, storyboard: "ThirdBeautify", controller: ""),
            MenuItem(name: "ARKit".localized, storyboard: "ARKit", controller: ""),
            MenuItem(name: "Audio Router(Third Party Player)".localized, storyboard: "AudioRouterPlayer", controller: ""),
            MenuItem(name: "Audio Waveform".localized, storyboard: "AudioWaveform", controller: ""),
            MenuItem(name: "Face Capture".localized, storyboard: "FaceCapture", controller: ""),
            MenuItem(name: "Transparent Render".localized, storyboard: "TransparentRender", controller: ""),
            MenuItem(name: "URL Streaming(RTE Player)".localized, storyboard: "RtePlayer", controller: "")
        ])
    ]
    override func viewDidLoad() {
        super.viewDidLoad()
        Floaty.global.button.addItem(title: "Send Logs", handler: { _ in
            LogUtils.writeAppLogsToDisk()
            let activity = UIActivityViewController(activityItems: [NSURL(fileURLWithPath: LogUtils.logFolder(), 
                                                                          isDirectory: true)],
                                                    applicationActivities: nil)
            activity.modalPresentationStyle = .popover
            if UIDevice.current.userInterfaceIdiom == .pad {
                activity.popoverPresentationController?.sourceView = Floaty.global.button
            }
            self.present(activity, animated: true, completion: nil)
        })
        
        Floaty.global.button.addItem(title: "Clean Up", handler: { _ in
            LogUtils.cleanUp()
        })
        Floaty.global.button.isDraggable = true
        Floaty.global.show()
    }
    
    @IBAction func onSettings(_ sender: UIBarButtonItem) {
        let storyBoard: UIStoryboard = UIStoryboard(name: "Main", bundle: nil)
        guard let settingsViewController = storyBoard.instantiateViewController(withIdentifier: "settings") as? SettingsViewController else {
            return
        }
        
        settingsViewController.settingsDelegate = self
        settingsViewController.sectionNames = ["Video Configurations", "Metadata", "Private cloud"]
        guard let resolution = GlobalSettings.shared.getSetting(key: "resolution"),
              let fps = GlobalSettings.shared.getSetting(key: "fps"),
              let orientation = GlobalSettings.shared.getSetting(key: "orientation"),
              let role = GlobalSettings.shared.getSetting(key: "role") else { return }
        settingsViewController.sections = [
            [
                SettingsSelectParam(key: "resolution", 
                                    label: "Resolution".localized,
                                    settingItem: resolution,
                                    context: self),
                SettingsSelectParam(key: "fps",
                                    label: "Frame Rate".localized,
                                    settingItem: fps,
                                    context: self),
                SettingsSelectParam(key: "orientation",
                                    label: "Orientation".localized,
                                    settingItem: orientation,
                                    context: self),
                SettingsSelectParam(key: "role", 
                                    label: "Pick Role".localized,
                                    settingItem: role,
                                    context: self)
            ],
            [SettingsLabelParam(key: "sdk_ver", label: "SDK Version",
                                value: "\(AgoraRtcEngineKit.getSdkVersion())")],
            [
                SettingsTextFieldParam(key: "ip",
                                       label: "IP Address",
                                       text: GlobalSettings.shared.getCache(key: "ip"),
                                       placeHolder: "please input IP"),
                SettingsSwitchParam(key: "report",
                                    label: "Log Report",
                                    isOn: false),
                SettingsTextFieldParam(key: "log_server_domain", label: "Log Server Domain",
                                       text: GlobalSettings.shared.getCache(key: "log_server_domain"),
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
            ]
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
        return cell ?? UITableViewCell()
    }
}

extension ViewController: UITableViewDelegate {
    func tableView(_ tableView: UITableView, didSelectRowAt indexPath: IndexPath) {
        tableView.deselectRow(at: indexPath, animated: true)
        
        let menuItem = menus[indexPath.section].rows[indexPath.row]
        let storyBoard: UIStoryboard = UIStoryboard(name: menuItem.storyboard, bundle: nil)
        
        if menuItem.storyboard == "Main" {
            guard let controller = storyBoard.instantiateViewController(withIdentifier: menuItem.entry) as? EntryViewController else {
                return
            }
            controller.nextVCIdentifier = menuItem.controller
            controller.title = menuItem.name
            controller.note = menuItem.note
            navigationController?.pushViewController(controller, animated: true)
        } else {
            let entryViewController: UIViewController = storyBoard.instantiateViewController(withIdentifier: menuItem.entry)
            entryViewController.title = menuItem.name
            navigationController?.pushViewController(entryViewController, animated: true)
        }
    }
}

extension ViewController: SettingsViewControllerDelegate {
    func didChangeValue(type: String, key: String, value: Any) {
        LogUtils.log(message: "type == \(type) key == \(key) value == \(value)", level: .info)
        if type == "SettingsSelectCell" {
            guard let setting = value as? SettingItem else { return }
            LogUtils.log(message: "select \(setting.selectedOption().label) for \(key)", level: .info)
        }
        switch type {
        case "SettingsSelectCell":
            guard let setting = value as? SettingItem else { return }
            LogUtils.log(message: "select \(setting.selectedOption().label) for \(key)", level: .info)
            
        case "SettingsTextFieldCell":
            GlobalSettings.shared.cache[key] = value
            
        case "SettingsSwitchCell":
            GlobalSettings.shared.cache[key] = value
            
        default: break
        }
    }
}
