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
            MenuItem(name: "Join a channel (Video)".localized, storyboard: "JoinChannelVideo", controller: ""),
            MenuItem(name: "Join a channel (Audio)".localized, storyboard: "JoinChannelAudio", controller: "")
        ]),
        MenuSection(name: "Anvanced", rows: [
            MenuItem(name: "Group Video Chat".localized, storyboard: "VideoChat", controller: "VideoChat"),
            MenuItem(name: "Live Streaming".localized, storyboard: "LiveStreaming", controller: "LiveStreaming"),
            MenuItem(name: "RTMP Streaming".localized, storyboard: "RTMPStreaming", controller: "RTMPStreaming"),
            MenuItem(name: "Fusion CDN Streaming".localized, storyboard: "FusionCDN", controller: "FusionCDN"),
            MenuItem(name: "Video Metadata".localized, storyboard: "VideoMetadata", controller: "VideoMetadata".localized),
            MenuItem(name: "Voice Changer".localized, storyboard: "VoiceChanger", controller: ""),
            MenuItem(name: "Custom Audio Source".localized, storyboard: "CustomPcmAudioSource", controller: "CustomPcmAudioSource"),
            MenuItem(name: "Custom Audio Render".localized, storyboard: "CustomAudioRender", controller: "CustomAudioRender"),
            MenuItem(name: "Custom Video Source(Push)".localized, storyboard: "CustomVideoSourcePush", controller: "CustomVideoSourcePush"),
            MenuItem(name: "Custom Video Render".localized, storyboard: "CustomVideoRender", controller: "CustomVideoRender"),
//            MenuItem(name: "Raw Media Data".localized, storyboard: "RawMediaData", controller: "RawMediaData"),
            MenuItem(name: "Raw Video Data".localized, storyboard: "RawVideoData", controller: ""),
            MenuItem(name: "Simple Filter Extension".localized, storyboard: "SimpleFilter", controller: "SimpleFilter"),
            MenuItem(name: "Quick Switch Channel".localized, controller: "QuickSwitchChannel"),
            MenuItem(name: "Join Multiple Channels".localized, storyboard: "JoinMultiChannel", controller: "JoinMultiChannel"),
            MenuItem(name: "Stream Encryption".localized, storyboard: "StreamEncryption", controller: ""),
            MenuItem(name: "Audio Mixing".localized, storyboard: "AudioMixing", controller: ""),
            MenuItem(name: "Precall Test".localized, storyboard: "PrecallTest", controller: ""),
            MenuItem(name: "Media Player".localized, storyboard: "MediaPlayer", controller: ""),
            MenuItem(name: "Screen Share".localized, storyboard: "ScreenShare", controller: ""),
            MenuItem(name: "Video Process".localized, storyboard: "VideoProcess", controller: "VideoProcess"),
            MenuItem(name: "Rhythm Player".localized, storyboard: "RhythmPlayer", controller: "RhythmPlayer"),
//            MenuItem(name: "Screen Share + Camera".localized, storyboard: "ScreenShare+Socket", controller: ""),
            MenuItem(name: "Create Data Stream".localized, storyboard: "CreateDataStream", controller: ""),
            MenuItem(name: "Media Channel Relay".localized, storyboard: "MediaChannelRelay", controller: ""),
            MenuItem(name: "Spatial Audio".localized, storyboard: "SpatialAudio", controller: "SpatialAudio")
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
        settingsViewController.sectionNames = ["Video Configurations","Metadata"]
        settingsViewController.sections = [
            [
                SettingsSelectParam(key: "resolution", label:"Resolution".localized, settingItem: GlobalSettings.shared.getSetting(key: "resolution")!, context: self),
                SettingsSelectParam(key: "fps", label:"Frame Rate".localized, settingItem: GlobalSettings.shared.getSetting(key: "fps")!, context: self),
                SettingsSelectParam(key: "orientation", label:"Orientation".localized, settingItem: GlobalSettings.shared.getSetting(key: "orientation")!, context: self)
            ],
            [SettingsLabelParam(key: "sdk_ver", label: "SDK Version", value: "\(AgoraRtcEngineKit.getSdkVersion())")]
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
            self.navigationController?.pushViewController(entryViewController, animated: true)
        }
    }
}

extension ViewController: SettingsViewControllerDelegate {
    func didChangeValue(type: String, key: String, value: Any) {
        if(type == "SettingsSelectCell") {
            guard let setting = value as? SettingItem else {return}
            LogUtils.log(message: "select \(setting.selectedOption().label) for \(key)", level: .info)
        }
    }
}
