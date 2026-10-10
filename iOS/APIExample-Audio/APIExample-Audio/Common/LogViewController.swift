//
//  LogViewController.swift
//  APIExample
//
//  Created by 张乾泽 on 2020/4/17.
//  Copyright © 2020 Agora Corp. All rights reserved.
//

import UIKit
import Foundation
import Floaty

// Floaty.global creates a window without a scene. Keep this button in the app's view hierarchy.
final class LogFloatingButton: Floaty {
    override init() {
        super.init()
        configure()
    }

    required init?(coder: NSCoder) {
        super.init(coder: coder)
        configure()
    }

    func show(in container: UIView) {
        if superview !== container {
            container.addSubview(self)
        }
        container.bringSubviewToFront(self)
    }

    override func didMoveToSuperview() {
        super.didMoveToSuperview()
        updateSafeAreaPadding()
    }

    override func safeAreaInsetsDidChange() {
        super.safeAreaInsetsDidChange()
        updateSafeAreaPadding()
    }

    private func configure() {
        // Floaty 4.2 reads AppDelegate.window for safe-area placement, which is nil with scenes.
        relativeToSafeArea = false
        isDraggable = true
        addItem(title: "Send Logs") { [weak self] _ in
            self?.shareLogs()
        }
        addItem(title: "Clean Up") { _ in
            LogUtils.cleanUp()
        }
    }

    private func updateSafeAreaPadding() {
        let insets = superview?.safeAreaInsets ?? .zero
        let horizontal = 14 + insets.right
        let vertical = 14 + insets.bottom
        if paddingX != horizontal { paddingX = horizontal }
        if paddingY != vertical { paddingY = vertical }
    }

    private func shareLogs() {
        guard var presenter = window?.rootViewController else { return }
        while let presented = presenter.presentedViewController {
            presenter = presented
        }
        guard !(presenter is UIActivityViewController), !presenter.isBeingDismissed else { return }

        LogUtils.writeAppLogsToDisk()
        let activity = UIActivityViewController(
            activityItems: [URL(fileURLWithPath: LogUtils.logFolder(), isDirectory: true)],
            applicationActivities: nil
        )
        activity.modalPresentationStyle = .popover
        activity.popoverPresentationController?.sourceView = self
        activity.popoverPresentationController?.sourceRect = bounds
        presenter.present(activity, animated: true)
    }
}


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
