//
//  SettingCells.swift
//  APIExample
//
//  Created by XC on 2020/12/15.
//  Copyright © 2020 Agora Corp. All rights reserved.
//

import Foundation

class SettingsBaseParam: NSObject {
    var key: String
    var label: String
    var type: String
    
    init(type: String, key: String, label: String) {
        self.type = type
        self.key = key
        self.label = label
    }
}

class SettingBaseCell<T: SettingsBaseParam>: NSTableCellView {
    var configs: T?
    weak var delegate: SettingsViewControllerDelegate?
    
    func configure(config: T) {
        self.configs = config
    }
}

class SettingsSelectParam<T>: SettingsBaseParam {
    var value: String
    var settingItem: SettingItem<T>
    weak var context: NSViewController?
    
    init(key: String, label: String, settingItem: SettingItem<T>, context: NSViewController) {
        self.settingItem = settingItem
        self.context = context
        self.value = settingItem.selectedOption().label
        super.init(type: "SelectCell", key: key, label: label)
    }
}

class SettingSelectCell<T>: SettingBaseCell<SettingsSelectParam<T>> {
    @IBOutlet weak var label: NSTextField?
    @IBOutlet weak var picker: NSPopUpButton!
    
    override func configure(config: SettingsSelectParam<T>) {
        super.configure(config: config)
        self.label?.cell?.title = config.label
        self.picker?.addItems(withTitles: config.settingItem.options.map({ (option: SettingItemOption<T>) -> String in
            return option.label
        }))
        self.picker?.selectItem(at: config.settingItem.selected)
    }
}
