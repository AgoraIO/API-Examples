//
//  SettingsViewController.swift
//  APIExample
//
//  Created by XC on 2020/12/15.
//  Copyright © 2020 Agora Corp. All rights reserved.
//

import Foundation

protocol SettingsViewControllerDelegate: AnyObject {
    func didChangeValue(type: String, key: String, value: Any)
}
