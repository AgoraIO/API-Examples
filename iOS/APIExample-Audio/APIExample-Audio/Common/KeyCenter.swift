//
//  KeyCenter.swift
//  OpenLive
//
//  Created by GongYuhua on 6/25/16.
//  Copyright © 2016 Agora. All rights reserved.
//

struct KeyCenter {
    static let AppId: String = <#YOUR APPID#>

    // assign token and certificate to null if you have not enabled app certificate
    static let Certificate: String? = <#YOUR Certificate#>
    // assign token and certificate to null if you have not enabled app certificate or you have set the certificate above.
    static var Token: String? = nil
}
