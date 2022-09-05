//
//  GlobalSettings.swift
//  APIExample
//
//  Created by 张乾泽 on 2020/9/25.
//  Copyright © 2020 Agora Corp. All rights reserved.
//

import Foundation
import AgoraRtcKit


let SCREEN_SHARE_UID_MIN:UInt = 501
let SCREEN_SHARE_UID_MAX:UInt = 1000
let SCREEN_SHARE_BROADCASTER_UID_MIN:UInt = 1001
let SCREEN_SHARE_BROADCASTER_UID_MAX:UInt = 2000

let SCREEN_SHARE_UID = UInt.random(in: SCREEN_SHARE_UID_MIN...SCREEN_SHARE_UID_MAX)
let SCREEN_SHARE_BROADCASTER_UID = UInt.random(in: SCREEN_SHARE_BROADCASTER_UID_MIN...SCREEN_SHARE_BROADCASTER_UID_MAX)
//let SCREEN_SHARE_BROADCASTER_UID = 2000 // As per app group didn't enable in this demo, harded code screen share broadcaster uid

struct SettingItemOption {
    var idx: Int
    var label:String
    var value:Any
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
    var area:AgoraAreaCodeType = .global
    static let shared = GlobalSettings()
    var settings:[String:SettingItem] = [
        "resolution": SettingItem(selected: 3, options: [
            SettingItemOption(idx: 0, label: "90x90", value: CGSize(width: 90, height: 90)),
            SettingItemOption(idx: 1, label: "160x120", value: CGSize(width: 160, height: 120)),
            SettingItemOption(idx: 2, label: "320x240", value: CGSize(width: 320, height: 240)),
            SettingItemOption(idx: 3, label: "640x360", value: CGSize(width: 640, height: 360)),
            SettingItemOption(idx: 4, label: "1280x720", value: CGSize(width: 1280, height: 720))
        ]),
        "fps": SettingItem(selected: 3, options: [
            SettingItemOption(idx: 0, label: "10fps", value: AgoraVideoFrameRate.fps10),
            SettingItemOption(idx: 1, label: "15fps", value: AgoraVideoFrameRate.fps15),
            SettingItemOption(idx: 2, label: "24fps", value: AgoraVideoFrameRate.fps24),
            SettingItemOption(idx: 3, label: "30fps", value: AgoraVideoFrameRate.fps30),
            SettingItemOption(idx: 4, label: "60fps", value: AgoraVideoFrameRate.fps60)
        ]),
        "orientation": SettingItem(selected: 0, options: [
            SettingItemOption(idx: 0, label: "adaptive", value: AgoraVideoOutputOrientationMode.adaptative),
            SettingItemOption(idx: 1, label: "fixed portrait", value: AgoraVideoOutputOrientationMode.fixedPortrait),
            SettingItemOption(idx: 2, label: "fixed landscape", value: AgoraVideoOutputOrientationMode.fixedLandscape)
        ]),
        "role": SettingItem(selected: 0, options: [
            SettingItemOption(idx: 0, label: "broadcaster", value: AgoraClientRole.broadcaster),
            SettingItemOption(idx: 1, label: "audience", value: AgoraClientRole.audience)
        ]),
    ]
    
    func getSetting(key:String) -> SettingItem? {
        return settings[key]
    }
    
    func getUserRole() -> AgoraClientRole {
        let item = settings["role"]
        return (item?.selectedOption().value as? AgoraClientRole) ?? .broadcaster
    }
}
