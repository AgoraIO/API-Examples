//
//  LogUtil.swift
//  APIExample-SwiftUI
//
//  Created by zhaoyongqiang on 2024/3/22.
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
    var message: String
    var level: LogLevel
    var dateTime: Date
}

class LogUtils {
    static var logs: [LogItem] = []
    static var appLogPath: String = "\(logFolder())/app-\(Date().getFormattedDate(format: "yyyy-MM-dd")).log"
    
    static func log(message: String, level: LogLevel) {
        LogUtils.logs.append(LogItem(message: message, level: level, dateTime: Date()))
        print("\(level.description): \(message)")
    }
    
    static func logFolder() -> String {
        let folder = "\(NSSearchPathForDirectoriesInDomains(.documentDirectory, .userDomainMask, true)[0])/logs"
        try? FileManager.default.createDirectory(atPath: folder, withIntermediateDirectories: true, attributes: nil)
        return folder
    }
    static func sdkLogPath() -> String {
        let logPath = "\(logFolder())/agorasdk.log"
        return logPath
    }
    
    static func removeAll() {
        LogUtils.logs.removeAll()
    }
    
    static func writeAppLogsToDisk() {
        if let outputStream = OutputStream(url: URL(fileURLWithPath: LogUtils.appLogPath), append: true) {
            outputStream.open()
            for log in LogUtils.logs {
                let msg = "\(log.level.description) \(log.dateTime.getFormattedDate(format: "yyyy-MM-dd HH:mm:ss")) \(log.message)\n"
                let bytesWritten = outputStream.write(msg)
                if bytesWritten < 0 { print("write failure") }
            }
            outputStream.close()
            LogUtils.removeAll()
        } else {
            print("Unable to open file")
        }
    }
    
    static func cleanUp() {
        try? FileManager.default.removeItem(at: URL(fileURLWithPath: LogUtils.logFolder(), isDirectory: true))
    }
}
