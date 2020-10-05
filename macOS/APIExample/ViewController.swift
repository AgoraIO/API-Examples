//
//  ViewController.swift
//  APIExample
//
//  Created by 张乾泽 on 2020/8/28.
//  Copyright © 2020 Agora Corp. All rights reserved.
//

import Cocoa

struct MenuItem {
    var name: String
    var identifier: String
    var controller: String?
    var storyboard: String?
}

class MenuController: NSViewController {
    var menus:[MenuItem] = [
        MenuItem(name: "Basic", identifier: "headerCell"),
        MenuItem(name: "Join a channel (Video)", identifier: "menuCell", controller: "JoinChannelVideo", storyboard: "JoinChannelVideo"),
        MenuItem(name: "Join a channel (Audio)", identifier: "menuCell", controller: "JoinChannelAudio", storyboard: "JoinChannelAudio"),
        MenuItem(name: "Anvanced", identifier: "headerCell"),
        MenuItem(name: "RTMP Streaming", identifier: "menuCell", controller: "RTMPStreaming"),
        MenuItem(name: "Custom Video Source(MediaIO)", identifier: "menuCell", controller: "CustomVideoSourceMediaIO"),
        MenuItem(name: "Custom Video Source(Push)", identifier: "menuCell", controller: "CustomVideoSourcePush"),
        MenuItem(name: "Custom Video Render", identifier: "menuCell", controller: "CustomVideoRender"),
        MenuItem(name: "Custom Audio Source", identifier: "menuCell", controller: "CustomAudioSource"),
        MenuItem(name: "Custom Audio Render", identifier: "menuCell", controller: "CustomAudioRender"),
        MenuItem(name: "Raw Media Data", identifier: "menuCell", controller: "RawMediaData"),
        MenuItem(name: "Join Multiple Channels", identifier: "menuCell", controller: "JoinMultipleChannel", storyboard: "JoinMultiChannel"),
        MenuItem(name: "Stream Encryption", identifier: "menuCell", controller: "StreamEncryption", storyboard: "StreamEncryption"),
        MenuItem(name: "Screen Share", identifier: "menuCell", controller: "ScreenShare", storyboard: "ScreenShare"),
        MenuItem(name: "Channel Media Relay", identifier: "menuCell", controller: "ChannelMediaRelay", storyboard: "ChannelMediaRelay"),
        MenuItem(name: "Audio Mixing", identifier: "menuCell", controller: "AudioMixing", storyboard: "AudioMixing"),
        MenuItem(name: "Voice Changer", identifier: "menuCell", controller: "VoiceChanger", storyboard: "VoiceChanger")
    ]
    @IBOutlet weak var tableView:NSTableView!
    
    override func viewDidLoad() {
        super.viewDidLoad()
    }
}

extension MenuController: NSTableViewDataSource, NSTableViewDelegate {
    func tableView(_ tableView: NSTableView, heightOfRow row: Int) -> CGFloat {
        let item = menus[row]
        return item.identifier == "menuCell" ? 32 : 18
    }
    
    func numberOfRows(in tableView: NSTableView) -> Int {
        return menus.count
    }
    
    func tableView(_ tableView: NSTableView, shouldSelectRow row: Int) -> Bool {
        let item = menus[row]
        return item.identifier != "headerCell"
    }
    
    func tableView(_ tableView: NSTableView, viewFor tableColumn: NSTableColumn?, row: Int) -> NSView? {
        let item = menus[row]
        // Get an existing cell with the MyView identifier if it exists
        let view = tableView.makeView(withIdentifier: NSUserInterfaceItemIdentifier(rawValue: item.identifier), owner: self) as? NSTableCellView

        view?.imageView?.image = nil
        view?.textField?.stringValue = item.name

        // Return the result
        return view;
    }
    
    
    func tableViewSelectionDidChange(_ notification: Notification) {
        let selectedRow = tableView.selectedRow
        let item = menus[selectedRow]
        var storyboardName = ""
        
        if let name = item.storyboard {
            storyboardName = name
        } else {
            storyboardName = "Main"
        }
        let board: NSStoryboard = NSStoryboard(name: storyboardName, bundle: nil)
        
        guard let splitViewController = self.parent as? NSSplitViewController,
            let controllerIdentifier = item.controller,
            let viewController = board.instantiateController(withIdentifier: controllerIdentifier) as? BaseViewController else {return}
        
        let splititem = NSSplitViewItem(viewController: viewController)
        
        let detailItem = splitViewController.splitViewItems[1]
        if let detailViewController = detailItem.viewController as? BaseViewController {
            detailViewController.viewWillBeRemovedFromSplitView()
        }
        splitViewController.removeSplitViewItem(detailItem)
        splitViewController.addSplitViewItem(splititem)
    }
}

class ViewController: NSViewController {

    override func viewDidLoad() {
        super.viewDidLoad()

        // Do any additional setup after loading the view.
    }

    override var representedObject: Any? {
        didSet {
        // Update the view, if already loaded.
        }
    }
}

