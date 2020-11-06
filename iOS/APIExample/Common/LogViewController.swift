//
//  LogViewController.swift
//  APIExample
//
//  Created by 张乾泽 on 2020/4/17.
//  Copyright © 2020 Agora Corp. All rights reserved.
//

import UIKit
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
    static var appLogPath:String = "\(logFolder())/app-\(Date().getFormattedDate(format: "yyyy-MM-dd")).log"
    
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

class LogViewController: AGViewController {
    
}

extension LogViewController: UITableViewDataSource {
    func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        return LogUtils.logs.count
    }
    
    func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        let cellIdentifier = "logCell"
        var cell = tableView.dequeueReusableCell(withIdentifier: cellIdentifier)
        if cell == nil {
            cell = UITableViewCell(style: .default, reuseIdentifier: cellIdentifier)
        }
        let logitem = LogUtils.logs[indexPath.row]
        cell?.textLabel?.font = UIFont.systemFont(ofSize: 12)
        cell?.textLabel?.numberOfLines = 0;
        cell?.textLabel?.lineBreakMode = .byWordWrapping;
        let dateFormatterPrint = DateFormatter()
        dateFormatterPrint.dateFormat = "yyyy-MM-dd HH:mm:ss"
        cell?.textLabel?.text = "\(dateFormatterPrint.string(from: logitem.dateTime)) - \(logitem.level.description): \(logitem.message)"
        return cell!
    }
}
