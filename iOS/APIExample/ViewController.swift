//
//  ViewController.swift
//  APIExample
//
//  Created by 张乾泽 on 2020/4/16.
//  Copyright © 2020 Agora Corp. All rights reserved.
//

import UIKit

struct MenuSection {
    var name: String
    var rows:[MenuItem]
}

struct MenuItem {
    var name: String
    var entry: String = "EntryViewController"
    var controller: String
    var note: String = ""
}

class ViewController: AGViewController {
    var menus:[MenuSection] = [
        MenuSection(name: "Basic", rows: [
            MenuItem(name: "Join a channel (Video)", controller: "JoinChannelVideo"),
            MenuItem(name: "Join a channel (Audio)", controller: "JoinChannelAudio")
        ]),
        MenuSection(name: "Anvanced", rows: [
            MenuItem(name: "RTMP Streaming", controller: "RTMPStreaming", note: "Ensure that you enable the RTMP Converter service at Agora Dashboard before using this function."),
            MenuItem(name: "RTMP Injection", controller: "RTMPInjection"),
            MenuItem(name: "Video Metadata", controller: "VideoMetadata"),
            MenuItem(name: "Voice Changer", controller: "VoiceChanger"),
            MenuItem(name: "Custom Audio Source", controller: "CustomAudioSource"),
            MenuItem(name: "Custom Audio Render", controller: "CustomAudioRender"),
            MenuItem(name: "Custom Video Source(MediaIO)", controller: "CustomVideoSourceMediaIO"),
            MenuItem(name: "Custom Video Source(Push)", controller: "CustomVideoSourcePush"),
            MenuItem(name: "Custom Video Render", controller: "CustomVideoRender"),
            MenuItem(name: "Raw Media Data", controller: "RawMediaData"),
            MenuItem(name: "Quick Switch Channel", controller: "QuickSwitchChannel"),
            MenuItem(name: "Join Multiple Channels", controller: "JoinMultiChannel")
        ]),
    ]
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
        let storyBoard: UIStoryboard = UIStoryboard(name: "Main", bundle: nil)
        
        guard let entryViewController = storyBoard.instantiateViewController(withIdentifier: menuItem.entry) as? EntryViewController else { return }
        
        entryViewController.nextVCIdentifier = menuItem.controller
        entryViewController.title = menuItem.name
        entryViewController.note = menuItem.note
        self.navigationController?.pushViewController(entryViewController, animated: true)
    }
}
