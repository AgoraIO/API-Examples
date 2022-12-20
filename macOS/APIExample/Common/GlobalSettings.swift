//
//  GlobalSettings.swift
//  APIExample
//
//  Created by 张乾泽 on 2020/9/25.
//  Copyright © 2020 Agora Corp. All rights reserved.
//

import Foundation
import AgoraRtcKit

struct SettingItemOption<T> {
    var idx: Int
    var label: String
    var value: T
}

class SettingItem<T> {
    var selected: Int
    var options: [SettingItemOption<T>]
    
    func selectedOption() -> SettingItemOption<T> {
        return options[selected]
    }
    
    init(selected: Int, options: [SettingItemOption<T>]) {
        self.selected = selected
        self.options = options
    }
}

class GlobalSettings {
    // The region for connection. This advanced feature applies to scenarios that have regional restrictions.
    // For the regions that Agora supports, see https://docs.agora.io/en/Interactive%20Broadcast/API%20Reference/oc/Constants/AgoraAreaCode.html. After specifying the region, the SDK connects to the Agora servers within that region.
    var area:AgoraAreaCodeType = .global
    static let shared = GlobalSettings()
    let resolutionSetting: SettingItem<Int> = SettingItem(
        selected: Configs.defaultResolutionIdx,
        options: Configs.Resolutions.enumerated().map {
            SettingItemOption(idx: $0.offset, label: $0.element.name(), value: $0.offset)
        }
    )
    let fpsSetting: SettingItem<Int> = SettingItem(
        selected: Configs.defaultFpsIdx,
        options: Configs.Fps.enumerated().map {
            SettingItemOption(idx: $0.offset, label: "\($0.element)fps", value: $0.offset)
        }
    )
    let proxySetting: SettingItem<Int> = SettingItem(
        selected: Configs.defaultProxySettingIdx,
        options: Configs.Proxy.enumerated().map{
            SettingItemOption(idx: $0.offset, label: String($0.element), value: $0.offset)
    })
    
   var cache: [String: Any] = [:]
    
    func getCache(key: String) -> String {
        (cache[key] as? String) ?? ""
    }
    func getCache(key: String) -> Bool {
        guard let value = cache[key] as? Bool else { return false }
        return value
    }
    func getCache(key: String) -> Int {
        guard let cache = cache[key] as? String,
                let value = Int(cache) else { return 0 }
        return value
    }
}
