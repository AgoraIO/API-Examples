//
//  SettingsViewController.swift
//  APIExample
//
//  Created by 张乾泽 on 2020/7/26.
//  Copyright © 2020 Agora Corp. All rights reserved.
//

import Foundation
import UIKit

protocol SettingsViewControllerDelegate: AnyObject {
    func didChangeValue(type:String, key:String, value: Any)
}

class SettingsViewController : UITableViewController
{
    var sections:[[SettingsBaseParam]] = []
    var sectionNames:[String] = []
    weak var settingsDelegate:SettingsViewControllerDelegate?
    override func numberOfSections(in tableView: UITableView) -> Int {
        return sections.count
    }
    
    override func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        let params = sections[section]
        return params.count
    }
    
    override func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        let param = sections[indexPath.section][indexPath.row]
        guard let cell = tableView.dequeueReusableCell(withIdentifier: param.type, for: indexPath) as? SettingsBaseCell else {return UITableViewCell()}
        cell.delegate = settingsDelegate
        cell.configure(configs: param)

        return cell
    }
    
    override func tableView(_ tableView: UITableView, titleForHeaderInSection section: Int) -> String? {
        return sectionNames[section]
    }
}
