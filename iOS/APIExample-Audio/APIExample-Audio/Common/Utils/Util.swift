//
//  Util.swift
//  APIExample
//
//  Created by zhaoyongqiang on 2022/10/14.
//  Copyright © 2022 Agora Corp. All rights reserved.
//

import UIKit

enum Util {
    
    /// Configuring Privatization Parameters
    static func configPrivatization(agoraKit: AgoraRtcEngineKit) {
        if !GlobalSettings.shared.getCache(key: "ip").isEmpty {
            let localAccessPointConfig = AgoraLocalAccessPointConfiguration()
            localAccessPointConfig.ipList = ["\(GlobalSettings.shared.getCache(key: "ip"))"]
            localAccessPointConfig.mode = .localOnly
            if GlobalSettings.shared.getCache(key: "report") {
                let advancedConfig = AdvancedConfigInfo()
                let logUploadServer = LogUploadServerInfo()
                logUploadServer.serverDomain = GlobalSettings.shared.getCache(key: "log_server_domain")
                logUploadServer.serverPort = GlobalSettings.shared.getCache(key: "log_server_port")
                logUploadServer.serverPath = GlobalSettings.shared.getCache(key: "log_server_path")
                logUploadServer.serverHttps = GlobalSettings.shared.getCache(key: "log_server_https")
                advancedConfig.logUploadServer = logUploadServer
                localAccessPointConfig.advancedConfig = advancedConfig
            }
            agoraKit.setLocalAccessPoint(withConfig: localAccessPointConfig)
        }
    }
}
