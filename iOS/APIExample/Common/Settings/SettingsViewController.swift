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
    func didChangeValue(key:String, value: AnyObject)
}

class SettingsBaseCell : UITableViewCell
{
    var configs:SettingsBaseParam?
    weak var delegate:SettingsViewControllerDelegate?
    func configure(configs:SettingsBaseParam){
        self.configs = configs
    }
}

class SettingsBaseParam: NSObject
{
    var key:String
    var label:String
    var type:String
    
    init(key:String, label:String, type:String) {
        self.key = key
        self.label = label
        self.type = type
    }
}

class SettingsSliderCell : SettingsBaseCell
{
    @IBOutlet weak var settingLabel: AGLabel!
    @IBOutlet weak var settingSlider: UISlider!
    @IBOutlet weak var settingValue: AGLabel!
    
    @IBAction func onSliderValueChanged(sender:UISlider){
        let val = (sender.value*100).rounded()/100
        settingValue.text = "\(val)"
        guard let configs = self.configs as? SettingsSliderParam else {return}
        delegate?.didChangeValue(key: configs.key, value: val as AnyObject)
    }
    
    override func configure(configs: SettingsBaseParam) {
        super.configure(configs: configs)
        
        guard let param = configs as? SettingsSliderParam else {return}
        settingLabel.text = param.label
        settingSlider.value = param.value
        settingSlider.minimumValue = param.minimumValue
        settingSlider.maximumValue = param.maximumValue
        settingValue.text = "\(settingSlider.value)"
    }
}

class SettingsSliderParam: SettingsBaseParam {
    var value:Float
    var minimumValue:Float
    var maximumValue:Float
    init(key:String, label:String, value:Float, minimumValue:Float, maximumValue:Float) {
        self.value = value
        self.minimumValue = minimumValue
        self.maximumValue = maximumValue
        super.init(key: key, label: label, type: "SliderCell")
    }
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
