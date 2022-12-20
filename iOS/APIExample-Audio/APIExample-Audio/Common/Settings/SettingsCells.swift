//
//  SettingsCells.swift
//  APIExample
//
//  Created by ZQZ on 2020/11/28.
//  Copyright © 2020 Agora Corp. All rights reserved.
//

import Foundation

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
    @IBOutlet weak var settingLabel: UILabel!
    @IBOutlet weak var settingSlider: UISlider!
    @IBOutlet weak var settingValue: UILabel!
    
    @IBAction func onSliderValueChanged(sender:UISlider){
        let val = (sender.value*100).rounded()/100
        settingValue.text = "\(val)"
        guard let configs = self.configs as? SettingsSliderParam else {return}
        delegate?.didChangeValue(type: "SettingsSliderCell", key: configs.key, value: val)
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


class SettingsLabelCell : SettingsBaseCell
{
    @IBOutlet weak var settingLabel: UILabel!
    @IBOutlet weak var settingValue: UILabel!
    
    override func configure(configs: SettingsBaseParam) {
        super.configure(configs: configs)
        
        guard let param = configs as? SettingsLabelParam else {return}
        settingLabel.text = param.label
        settingValue.text = param.value
    }
}

class SettingsLabelParam: SettingsBaseParam {
    var value:String
    init(key:String, label:String, value:String) {
        self.value = value
        super.init(key: key, label: label, type: "LabelCell")
    }
}

class SettingsSelectCell : SettingsBaseCell
{
    @IBOutlet weak var settingLabel: UILabel!
    @IBOutlet weak var settingBtn: UIButton!
    
    override func configure(configs: SettingsBaseParam) {
        super.configure(configs: configs)
        
        guard let param = configs as? SettingsSelectParam else {return}
        settingLabel.text = param.label
        settingBtn.setTitle(param.value, for: .normal)
    }
    
    func getSelectAction(_ option:SettingItemOption) -> UIAlertAction {
        return UIAlertAction(title: "\(option.label)", style: .default, handler: {[unowned self] action in
            guard let param = self.configs as? SettingsSelectParam else {return}
            self.settingBtn.setTitle(option.label, for: .normal)
            param.settingItem.selected = option.idx
            self.delegate?.didChangeValue(type: "SettingsSelectCell", key: param.key, value: param.settingItem)
        })
    }
    
    @IBAction func onSelect(_ sender:UIButton) {
        let alert = UIAlertController(title: nil, message: nil, preferredStyle: UIDevice.current.userInterfaceIdiom == .pad ? UIAlertController.Style.alert : UIAlertController.Style.actionSheet)
        guard let param = configs as? SettingsSelectParam else {return}
        for option in param.settingItem.options {
            alert.addAction(getSelectAction(option))
        }
        alert.addCancelAction()
        param.context?.present(alert, animated: true, completion: nil)
    }
}

class SettingsSelectParam: SettingsBaseParam {
    var value:String
    var settingItem:SettingItem
    weak var context:UIViewController?;
    init(key:String, label:String, settingItem:SettingItem, context:UIViewController) {
        self.settingItem = settingItem
        self.context = context
        self.value = settingItem.selectedOption().label
        super.init(key: key, label: label, type: "SelectCell")
    }
}

class SettingsTextFieldCell : SettingsBaseCell
{
    @IBOutlet weak var settingLabel: UILabel!
    @IBOutlet weak var textField: UITextField!
    
    override func configure(configs: SettingsBaseParam) {
        super.configure(configs: configs)
        
        guard let param = configs as? SettingsTextFieldParam else {return}
        settingLabel.text = param.label
        textField.placeholder = param.placeholder
        textField.text = param.text
    }
    
    @IBAction func onTextFieldChanged(_ sender: UITextField) {
        guard let configs = self.configs as? SettingsTextFieldParam else {return}
        delegate?.didChangeValue(type: "SettingsTextFieldCell", key: configs.key, value: sender.text ?? "")
    }
}

class SettingsTextFieldParam: SettingsBaseParam {
    var text: String?
    var placeholder: String?
    init(key: String, label: String, text: String? = nil, placeHolder: String? = nil) {
        super.init(key: key, label: label, type: "TextFieldCell")
        self.text = text
        self.placeholder = placeHolder
    }
}

class SettingsSwitchCell : SettingsBaseCell
{
    @IBOutlet weak var settingLabel: UILabel!
    @IBOutlet weak var uiSwitch: UISwitch!
    
    override func configure(configs: SettingsBaseParam) {
        super.configure(configs: configs)
        
        guard let param = configs as? SettingsSwitchParam else {return}
        settingLabel.text = param.label
        uiSwitch.isOn = param.isOn
    }
    
    @IBAction func onSwitchChanged(sender: UISwitch){
        guard let configs = self.configs as? SettingsSwitchParam else {return}
        delegate?.didChangeValue(type: "SettingsSwitchCell", key: configs.key, value:  "\(uiSwitch.isOn ? 1 : 0)")
    }
}

class SettingsSwitchParam: SettingsBaseParam {
    var isOn: Bool = false
    init(key: String, label: String, isOn: Bool = false) {
        super.init(key: key, label: label, type: "SwitchCell")
        self.isOn = isOn
    }
}
