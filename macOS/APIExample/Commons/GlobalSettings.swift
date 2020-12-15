//
//  GlobalSettings.swift
//  APIExample
//
//  Created by 张乾泽 on 2020/9/25.
//  Copyright © 2020 Agora Corp. All rights reserved.
//

import Foundation
import AgoraRtcKit

struct SettingItemOption {
    var idx: Int
    var label: String
    var value: Any
}

class SettingItem {
    var selected: Int
    var options: [SettingItemOption]
    
    func selectedOption() -> SettingItemOption {
        return options[selected]
    }
    
    init(selected: Int, options: [SettingItemOption]) {
        self.selected = selected
        self.options = options
    }
}

class GlobalSettings {
    // The region for connection. This advanced feature applies to scenarios that have regional restrictions.
    // For the regions that Agora supports, see https://docs.agora.io/en/Interactive%20Broadcast/API%20Reference/oc/Constants/AgoraAreaCode.html. After specifying the region, the SDK connects to the Agora servers within that region.
    var area:AgoraAreaCode = .GLOB
    static let shared = GlobalSettings()
    var settings: [String: SettingItem] = [
        "resolution": SettingItem(selected: Configs.defaultResolutionIdx,
                                  options: Configs.Resolutions.enumerated()
                                      .map({ (element: EnumeratedSequence<[Resolution]>.Iterator.Element) -> SettingItemOption in
            return SettingItemOption(idx: element.offset, label: element.element.name(), value: element.offset)
        })),
        "fps": SettingItem(selected: Configs.defaultFpsIdx,
                           options: Configs.Fps.enumerated()
                               .map({ (element: EnumeratedSequence<[Int]>.Iterator.Element) -> SettingItemOption in
            return SettingItemOption(idx: element.offset, label: "\(element.element)fps", value: element.offset)
        }))
    ]
}
