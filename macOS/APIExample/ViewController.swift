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
    
    let settings = MenuItem(name: "Global settings".localized, identifier: "menuCell", controller: "Settings", storyboard: "Settings")
    
    var menus:[MenuItem] = [
        MenuItem(name: "Basic", identifier: "headerCell"),
        MenuItem(name: "Join a channel (Video)".localized, identifier: "menuCell", controller: "JoinChannelVideo", storyboard: "JoinChannelVideo"),
        MenuItem(name: "Join a channel (Audio)".localized, identifier: "menuCell", controller: "JoinChannelAudio", storyboard: "JoinChannelAudio"),
        MenuItem(name: "Anvanced", identifier: "headerCell"),
        MenuItem(name: "RTMP Streaming".localized, identifier: "menuCell", controller: "RTMPStreaming", storyboard: "RTMPStreaming"),
        MenuItem(name: "Custom Video Source(Push)".localized, identifier: "menuCell", controller: "CustomVideoSourcePush", storyboard: "CustomVideoSourcePush"),
//        MenuItem(name: "Custom Video Render".localized, identifier: "menuCell", controller: "CustomVideoRender", storyboard: "CustomVideoRender"),
        MenuItem(name: "Custom Audio Source".localized, identifier: "menuCell", controller: "CustomAudioSource", storyboard: "CustomAudioSource"),
        MenuItem(name: "Custom Audio Render".localized, identifier: "menuCell", controller: "CustomAudioRender", storyboard: "CustomAudioRender"),
//        MenuItem(name: "Raw Media Data".localized, identifier: "menuCell", controller: "RawMediaData", storyboard: "RawMediaData"),
        MenuItem(name: "Join Multiple Channels".localized, identifier: "menuCell", controller: "JoinMultipleChannel", storyboard: "JoinMultiChannel"),
        MenuItem(name: "Stream Encryption".localized, identifier: "menuCell", controller: "StreamEncryption", storyboard: "StreamEncryption"),
        MenuItem(name: "Screen Share".localized, identifier: "menuCell", controller: "ScreenShare", storyboard: "ScreenShare"),
        MenuItem(name: "Media Channel Relay".localized, identifier: "menuCell", controller: "ChannelMediaRelay", storyboard: "ChannelMediaRelay"),
        MenuItem(name: "Audio Mixing".localized, identifier: "menuCell", controller: "AudioMixing", storyboard: "AudioMixing"),
        MenuItem(name: "Voice Changer".localized, identifier: "menuCell", controller: "VoiceChanger", storyboard: "VoiceChanger"),
        MenuItem(name: "Precall Test".localized, identifier: "menuCell", controller: "PrecallTest", storyboard: "PrecallTest"),
        MenuItem(name: "Create Data Stream".localized, identifier: "menuCell", controller: "CreateDataStream", storyboard: "CreateDataStream"),
        MenuItem(name: "Simple Filter".localized, identifier: "menuCell", controller: "SimpleFilter", storyboard: "SimpleFilter"),
        MenuItem(name: "Raw Audio Data".localized, identifier: "menuCell", controller: "RawAudioData", storyboard: "RawAudioData"),
        MenuItem(name: "Raw Video Data".localized, identifier: "menuCell", controller: "RawVideoData", storyboard: "RawVideoData"),
        MenuItem(name: "Video Process".localized, identifier: "menuCell", controller: "Video Process", storyboard: "VideoProcess"),
        MenuItem(name: "Media Player".localized, identifier: "menuCell", controller: "MediaPlayer", storyboard: "MediaPlayer"),
        MenuItem(name: "Spatial Audio".localized, identifier: "menuCell", controller: "SpatialAudio", storyboard: "SpatialAudio")
    ]
    @IBOutlet weak var tableView:NSTableView!
    
    override func viewDidLoad() {
        super.viewDidLoad()
    }
    
    @IBAction func onClickSetting(_ sender: NSButton) {
        let selectedRow = tableView.selectedRow
        if (selectedRow >= 0) {
            tableView.deselectRow(selectedRow)
        }
        loadSplitViewItem(item: settings)
    }
    
    func loadSplitViewItem(item: MenuItem) {
        var storyboardName = ""
        
        if let name = item.storyboard {
            storyboardName = name
        } else {
            storyboardName = "Main"
        }
        let board: NSStoryboard = NSStoryboard(name: storyboardName, bundle: nil)
        
        guard let splitViewController = self.parent as? NSSplitViewController,
            let controllerIdentifier = item.controller,
            let viewController = board.instantiateController(withIdentifier: controllerIdentifier) as? BaseView else { return }
        
        let splititem = NSSplitViewItem(viewController: viewController as NSViewController)
        
        let detailItem = splitViewController.splitViewItems[1]
        if let detailViewController = detailItem.viewController as? BaseView {
            detailViewController.viewWillBeRemovedFromSplitView()
        }
        splitViewController.removeSplitViewItem(detailItem)
        splitViewController.addSplitViewItem(splititem)
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
        if tableView.selectedRow >= 0 && tableView.selectedRow < menus.count {
            loadSplitViewItem(item: menus[tableView.selectedRow])
        }
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

