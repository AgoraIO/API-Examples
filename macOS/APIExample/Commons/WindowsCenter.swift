//
//  WindowsCenter.swift
//  AgoraVideoCall
//
//  Created by GongYuhua on 6/14/16.
//  Copyright © 2016 Agora. All rights reserved.
//

import CoreGraphics

#if os(iOS)
import UIKit
#else
import Cocoa
#endif

enum WindowType: Int {
    case window, screen
}

enum ApplicationType {
    case web, ppt, keynote, word, pages, preview, other
}

class Window {
    fileprivate(set) var type: WindowType = .window
    fileprivate(set) var id: UInt32 = 0
    fileprivate(set) var name: String!
    fileprivate(set) var image: NSImage!
    fileprivate(set) var width: CGFloat = 0
    fileprivate(set) var height: CGFloat = 0
    var size: CGSize {
        return CGSize(width: width, height: height)
    }

    init?(windowDic: NSDictionary) {
        if let layerNumber = windowDic[Window.convertCFString(kCGWindowLayer)] {
            let cfNumber = layerNumber as! CFNumber
            let layer = Window.convertCFNumber(cfNumber)
            guard layer == 0 else {
                return nil
            }
        }

        if let alphaNumber = windowDic[Window.convertCFString(kCGWindowAlpha)] {
            let cfNumber = alphaNumber as! CFNumber
            let alpha = Window.convertCFNumber(cfNumber)
            if alpha == 0 {
                return nil
            }
        }

        if windowDic[Window.convertCFString(kCGWindowName)] == nil {
            return nil
        }

        guard let idNumber = windowDic[Window.convertCFString(kCGWindowNumber)]  else {
             return nil
        }

        let cfNumber = idNumber as! CFNumber
        let id = Window.convertCFNumber(cfNumber)

        var name: String?
        if let ownerName = windowDic[Window.convertCFString(kCGWindowOwnerName)] {
            let cfName: CFString = ownerName as! CFString
            name = Window.convertCFString(cfName)
            if name == "Agora Video Call" {
                return nil
            }
        }

        guard let image = Window.image(of: id) else {
            return nil
        }

        self.id = id
        self.name = name ?? "Unknown"
        self.image = image
        self.width = image.size.width
        self.height = image.size.height
        self.type = .window
    }

    init?(screenId: CGDirectDisplayID, name: String) {
        self.name = name
        self.id = screenId
        self.type = .screen
        guard let image = Window.imageOfScreenId(self.id) else {
            return
        }
        self.image = image
        self.width = image.size.width
        self.height = image.size.height
    }

    fileprivate init() {}

    static func fullScreenWindow() -> Window {
        let window = Window()
        window.name = "Full Screen"
        window.image = imageOfFullScreen()
        if let main = NSScreen.screens.first {
            let scale = main.backingScaleFactor
            window.width = main.frame.size.width * scale
            window.height = main.frame.size.height * scale
        }

        return window
    }

    static func image(of windowId: CGWindowID) -> NSImage? {
        if let screenShot = CGWindowListCreateImage(CGRect.null, .optionIncludingWindow, CGWindowID(windowId), CGWindowImageOption.boundsIgnoreFraming) {
            let bitmapRep = NSBitmapImageRep(cgImage: screenShot)
            let image = NSImage()
            image.addRepresentation(bitmapRep)

            if image.size.width == 1 {
                return nil
            } else {
               return image
            }
        } else {
            return nil
        }
    }

    fileprivate static func imageOfScreenId(_ screenId: CGDirectDisplayID) -> NSImage? {
        if let screenShot = CGDisplayCreateImage(screenId) {
            let bitmapRep = NSBitmapImageRep(cgImage: screenShot)
            let image = NSImage()
            image.addRepresentation(bitmapRep)

            if image.size.width == 1 {
                return nil
            } else {
                return image
            }
        } else {
            return nil
        }
    }

    fileprivate static func imageOfFullScreen() -> NSImage {
        if let screenShot = CGWindowListCreateImage(CGRect.infinite, .optionOnScreenOnly, CGWindowID(0), CGWindowImageOption()) {
            let bitmapRep = NSBitmapImageRep(cgImage: screenShot)
            let image = NSImage()
            image.addRepresentation(bitmapRep)
            return image
        } else {
            return NSImage()
        }
    }
}

class WindowList {
    var items = [Window]()

    func getList() {
        var list = [Window]()

        var webList = [Window]()
        var pptList = [Window]()
        var keynoteList = [Window]()
        var wordList = [Window]()
        var pagesList = [Window]()
        var previewList = [Window]()
        var otherList = [Window]()

        // add screens
        let screens = NSScreen.screens
        for (index, screen) in screens.enumerated() {
            guard let screenId = screen.deviceDescription[NSDeviceDescriptionKey(rawValue: "NSScreenNumber")] as? CGDirectDisplayID else {
                continue
            }
            if let window = Window(screenId: screenId, name: "Screen \(index + 1)") {
                list.append(window)
            }
        }

        // add windows
        if let windowDicCFArray = CGWindowListCopyWindowInfo([.optionAll, .excludeDesktopElements], 0) {
            let windowDicList = windowDicCFArray as NSArray

            for windowElement in windowDicList {
                let windowDic = windowElement
                if let windowDic = windowDic as? NSDictionary {
                    if let window = Window(windowDic: windowDic) {
                        let appType = typeOfApplication(with: window.name)
                        switch appType {
                        case .web:
                            webList.append(window)
                        case .ppt:
                            pptList.append(window)
                        case .keynote:
                            keynoteList.append(window)
                        case .word:
                            wordList.append(window)
                        case .pages:
                            pagesList.append(window)
                        case .preview:
                            previewList.append(window)
                        case .other:
                            otherList.append(window)
                        }
                    }
                }
            }
        }
        let temp =  webList + pptList + keynoteList + wordList
        list += temp + pagesList + previewList + otherList

        self.items = list
    }

    private func typeOfApplication(with name: String) -> ApplicationType {
        if name.contains("Google Chrome") || name.contains("Safari") {
            return .web
        } else if name.contains("PowerPoint") {
            return .ppt
        } else if name.contains("Microsoft") {
            return .word
        } else if name.contains("Keynote") {
            return .keynote
        } else if name.contains("Pages") {
            return .pages
        } else if name.contains("Preview") {
            return .preview
        } else {
            return .other
        }
    }

    private func isHighPriortyWindow(with name: String) -> Bool {
        return (name.contains("Microsoft") && !name.contains("Outlook") && !name.contains("Teams"))
                || name.contains("Google Chrome")
    }
}

extension Window {
    class func convertCFString(_ cfString: CFString) -> String {
        let string = cfString as NSString
        return string as String

    }

    class func convertCFNumber(_ cfNumber: CFNumber) -> UInt32 {
        let number = cfNumber as NSNumber
        return number.uint32Value
    }
}
