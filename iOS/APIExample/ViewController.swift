//
//  ViewController.swift
//  APIExample
//
//  Created by 张乾泽 on 2020/4/16.
//  Copyright © 2020 Agora Corp. All rights reserved.
//

#if os(iOS)
import UIKit
#else
import Cocoa
#endif

struct MenuSection {
    var name: String
    var rows:[MenuItem]
}

struct MenuItem {
    var name: String
    var controller: String
}

class ViewController: AGViewController {
    #if os(iOS)
    var menus:[MenuSection] = [
        MenuSection(name: "Basic", rows: [
            MenuItem(name: "Join a channel (Video)", controller: "JoinChannelVideo"),
            MenuItem(name: "Join a channel (Audio)", controller: "JoinChannelAudio")
        ]),
        MenuSection(name: "Anvanced", rows: [
            MenuItem(name: "RTMP Streaming", controller: "RTMPStreaming"),
            MenuItem(name: "RTMP Injection", controller: "RTMPInjection"),
            MenuItem(name: "Video metadata", controller: "VideoMetadata")
        ]),
    ]
    
    #else
    
    var menus:[MenuSection] = [
        MenuSection(name: "Basic", rows: [
            MenuItem(name: "Join a channel (Video)", controller: "JoinChannelVideoMain"),
            MenuItem(name: "Join a channel (Audio)", controller: "JoinChannelAudioMain")
        ])
    ]
    
    @IBOutlet weak var sectionTableView: NSTableView!
    @IBOutlet weak var subTableView: NSTableView!
    
    var sectionSelected = 0
    
    override func viewDidLoad() {
        super.viewDidLoad()
        sectionTableView.selectRowIndexes(IndexSet(integer: 0), byExtendingSelection: false)
    }
    
    override func prepare(for segue: NSStoryboardSegue, sender: Any?) {
        if let vc = segue.destinationController as? BaseViewController {
            vc.closeDelegate = self
        }
    }
    #endif
}

#if os(iOS)
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
        
        let name = "\(menus[indexPath.section].rows[indexPath.row].controller)"
        let storyBoard: UIStoryboard = UIStoryboard(name: "Main", bundle: nil)
        let newViewController = storyBoard.instantiateViewController(withIdentifier: name)
        self.navigationController?.pushViewController(newViewController, animated: true)
    }
}

#else
extension ViewController: NSTableViewDelegate, NSTableViewDataSource {
    func numberOfRows(in tableView: NSTableView) -> Int {
        if tableView == sectionTableView {
            return menus.count
        } else {
            return menus[sectionSelected].rows.count
        }
    }
    
    func tableView(_ tableView: NSTableView, shouldSelectRow row: Int) -> Bool {
        if tableView == sectionTableView {
            sectionSelected = row
            subTableView.reloadData()
            return true
        } else {
            let name = "\(menus[sectionSelected].rows[row].controller)"
            self.performSegue(withIdentifier: name, sender: nil)
            return false
        }
    }
    
    func tableView(_ tableView: NSTableView, viewFor tableColumn: NSTableColumn?, row: Int) -> NSView? {
        if tableView == sectionTableView {
            let cell = tableView.makeView(withIdentifier: NSUserInterfaceItemIdentifier(rawValue: "SectionCell"),
                                          owner: nil) as! NSTableCellView
            cell.textField?.text = menus[row].name
            return cell
        } else {
            let cell = tableView.makeView(withIdentifier: NSUserInterfaceItemIdentifier(rawValue: "SubCell"),
                                          owner: nil) as! NSTableCellView
            cell.textField?.text = menus[sectionSelected].rows[row].name
            return cell
        }
    }
    
    func tableView(_ tableView: NSTableView, heightOfRow row: Int) -> CGFloat {
        return 36
    }
}

extension ViewController: ViewControllerCloseDelegate {
    func viewControllerNeedClose(_ liveVC: AGViewController) {
        liveVC.view.window?.contentViewController = self
    }
}
#endif
