//
//  AgoraState.swift
//  APIExample
//
//  Created by XC on 2020/12/18.
//  Copyright © 2020 Agora Corp. All rights reserved.
//

import Foundation

protocol AgoraState {
    var isJoined: Bool { get set }
    var agoraKit: AgoraRtcEngineKit! { get set }
}
