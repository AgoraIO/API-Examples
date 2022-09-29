//
//  LogViewController.swift
//  APIExample
//
//  Created by 张乾泽 on 2020/4/17.
//  Copyright © 2020 Agora Corp. All rights reserved.
//

import Foundation

enum LogLevel {
    case info, warning, error
    
    var description: String {
        switch self {
        case .info:    return "Info"
        case .warning: return "Warning"
        case .error:   return "Error"
        }
    }
}

struct LogItem {
    var message:String
    var level:LogLevel
    var dateTime:Date
}

class LogUtils {
    static var logs:[LogItem] = []
    
    static func log(message: String, level: LogLevel) {
        LogUtils.logs.append(LogItem(message: message, level: level, dateTime: Date()))
        print("\(level.description): \(message)")
    }
    
    static func removeAll() {
        LogUtils.logs.removeAll()
    }
}
