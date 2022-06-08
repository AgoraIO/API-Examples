//
//  NSData+Extension.swift
//  APIExample
//
//  Created by zhaoyongqiang on 2022/6/8.
//  Copyright © 2022 Agora Corp. All rights reserved.
//

import Foundation

extension NSBitmapImageRep {
    var png: Data? {
        return representation(using: .png, properties: [:])
    }
}
extension Data {
    var bitmap: NSBitmapImageRep? {
        return NSBitmapImageRep(data: self)
    }
}

extension NSImage {
    var png: Data? {
        return tiffRepresentation?.bitmap?.png
    }
    
    func savePNG(to url: URL) -> Bool {
        do {
            try png?.write(to: url, options: .atomic)
            return true
        } catch {
            print(error)
            return false
        }

    }
}
