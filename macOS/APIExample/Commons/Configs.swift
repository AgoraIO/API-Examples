//
//  Configs.swift
//  APIExample
//
//  Created by 张乾泽 on 2020/8/29.
//  Copyright © 2020 Agora Corp. All rights reserved.
//

import Foundation

struct Resolution {
    var width: Int
    var height: Int
    func name() -> String {
        return "\(width)x\(height)"
    }
    func size() -> CGSize {
        return CGSize(width: width, height: height)
    }
}

class Configs {
    static var defaultResolutionIdx: Int = 2
    static var Resolutions:[Resolution] = [
        Resolution(width: 320, height: 240),
        Resolution(width: 640, height: 480),
        Resolution(width: 960, height: 720),
        Resolution(width: 1920, height: 1080)
    ]
    static var defaultFpsIdx: Int = 1
    static var Fps:[Int] = [
        15,
        30
    ]
    //TODO
//    static var VideoContentHints:[AgoraVideoContentHint] = [
//        AgoraVideoContentHint.none,
//        AgoraVideoContentHint.motion,
//        AgoraVideoContentHint.details
//    ]
}
