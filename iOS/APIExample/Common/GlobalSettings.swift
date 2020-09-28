//
//  GlobalSettings.swift
//  APIExample
//
//  Created by 张乾泽 on 2020/9/25.
//  Copyright © 2020 Agora Corp. All rights reserved.
//

import Foundation


let SCREEN_SHARE_UID_MIN:UInt = 501
let SCREEN_SHARE_UID_MAX:UInt = 1000
let SCREEN_SHARE_BROADCASTER_UID_MIN:UInt = 1001
let SCREEN_SHARE_BROADCASTER_UID_MAX:UInt = 2000

let SCREEN_SHARE_UID = UInt.random(in: SCREEN_SHARE_UID_MIN...SCREEN_SHARE_UID_MAX)
let SCREEN_SHARE_BROADCASTER_UID = UInt.random(in: SCREEN_SHARE_BROADCASTER_UID_MIN...SCREEN_SHARE_BROADCASTER_UID_MAX)

class GlobalSettings {
    // The region for connection. This advanced feature applies to scenarios that have regional restrictions.
    // For the regions that Agora supports, see https://docs.agora.io/en/Interactive%20Broadcast/API%20Reference/oc/Constants/AgoraAreaCode.html. After specifying the region, the SDK connects to the Agora servers within that region.
    var area:AgoraAreaCode = .GLOB
    static let shared = GlobalSettings()
}
