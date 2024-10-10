//
//  KeyCenter.h
//  APIExample
//
//  Created by zhaoyongqiang on 2023/7/11.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface KeyCenter : NSObject

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
+ (nullable NSString *)AppId;

/**
 Certificate.
 Agora provides App certificate to generate Token. You can deploy and generate a token on your server,
 or use the console to generate a temporary token.
 In order to get the APP ID, you can open the agora console (https://console.agora.io/) to create a project with the App Certificate enabled,
 then the APP Certificate can be found in the project detail page.If the project does not have certificates enabled, leave this field blank.
 PS: It is unsafe to place the App Certificate on the client side, it is recommended to place it on the server side to ensure that the App Certificate is not leaked.
 声网APP证书
 Agora 提供 App certificate 用以生成 Token。您可以在您的服务器部署并生成 Token，或者使用控制台生成临时的 Token。
 进入声网控制台(https://console.agora.io/)，创建一个带证书鉴权的项目，进入项目配置页，即可看到APP证书。如果项目没有开启证书鉴权，这个字段留空。
 注意：App证书放在客户端不安全，推荐放在服务端以确保 App 证书不会泄露。
 */
+ (nullable NSString *)Certificate;

@end

NS_ASSUME_NONNULL_END
