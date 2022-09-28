//
//  KeyCenter.swift
//  OpenLive
//
//  Created by GongYuhua on 6/25/16.
//  Copyright © 2016 Agora. All rights reserved.
//

struct KeyCenter {
    
    /**
     Agora APP ID.
     Agora assigns App IDs to app developers to identify projects and organizations.
     If you have multiple completely separate apps in your organization, for example built by different teams,
     you should use different App IDs.
     If applications need to communicate with each other, they should use the same App ID.
     In order to get the APP ID, you can open the agora console (https://console.agora.io/) to create a project,
     then the APP ID can be found in the project detail page.
     声网APP ID
     Agora 给应用程序开发人员分配 App ID，以识别项目和组织。如果组织中有多个完全分开的应用程序，例如由不同的团队构建，
     则应使用不同的 App ID。如果应用程序需要相互通信，则应使用同一个App ID。
     进入声网控制台(https://console.agora.io/)，创建一个项目，进入项目配置页，即可看到APP ID。
     */
    static let AppId: String = <#YOUR APPID#>

    /**
     Certificate.
     Agora provides App certificate to generate Token. You can deploy and generate a token on your server,
     or use the console to generate a temporary token.
     In order to get the APP ID, you can open the agora console (https://console.agora.io/) to create a project with the App Certificate enabled,
     then the APP Certificate can be found in the project detail page.
     PS: If the project does not have certificates enabled, leave this field blank.
     声网APP证书
     Agora 提供 App certificate 用以生成 Token。您可以在您的服务器部署并生成 Token，或者使用控制台生成临时的 Token。
     进入声网控制台(https://console.agora.io/)，创建一个带证书鉴权的项目，进入项目配置页，即可看到APP证书。
     注意：如果项目没有开启证书鉴权，这个字段留空。
     */
    static let Certificate: String? = <#YOUR Certificate#>
    
    /**
     Token.
     Agora provides Temporary Access Token to join the spatial channel with APP ID which enable App Certificate.
     You can use it to test your project.
     You can generate the temporary access token in the project console with the App Certificate enabled.
     PS：If agora_app_certificate is configured, this field will be invalid.
     音视频临时Token
     Agora 提供 音视频临时Token 用以加入带证书鉴权的频道。您可以使用这个Token来做测试。
     进入控制台开启证书鉴权的项目配置页，在APP证书下方有"生成临时音视频token"的按钮，输入频道名即可生成一个临时token。
     注意：如果配置了agora_app_certificate，则这个字段会失效。
     */
    static var Token: String? = nil
}
