//
//  Util.swift
//  APIExample
//
//  Created by zhaoyongqiang on 2022/6/27.
//  Copyright © 2022 Agora Corp. All rights reserved.
//

import Foundation

enum Util {
    static func dicValueString(_ dic: [String: Any]) ->String?{
        let data = try? JSONSerialization.data(withJSONObject: dic, options: [])
        let str = String(data: data!,encoding:String.Encoding.utf8)
        return str
    }
}
