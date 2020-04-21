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
    var controller: String
}

class ViewController: UIViewController {
    var menus:[MenuSection] = [
        MenuSection(name: "Basic Video/Audio", rows: [
            MenuItem(name: "Join a channel", controller: "JoinChannel")
        ]),
        MenuSection(name: "Live Broadcasting", rows: [
            MenuItem(name: "RTMP Streaming", controller: "RTMPStreaming"),
            MenuItem(name: "RTMP Injection", controller: "RTMPInjection")
        ]),
        MenuSection(name: "Quality Metrics", rows: [
            MenuItem(name: "Lastmile Test", controller: "Lastmile"),
            MenuItem(name: "Realtime Stats", controller: "RealtimeStats")
        ])
    ]
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view.
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
        let name = "\(menus[indexPath.section].rows[indexPath.row].controller)"
        let storyBoard: UIStoryboard = UIStoryboard(name: "Main", bundle: nil)
        let newViewController = storyBoard.instantiateViewController(withIdentifier: name)
        self.navigationController?.pushViewController(newViewController, animated: true)
    }
}
