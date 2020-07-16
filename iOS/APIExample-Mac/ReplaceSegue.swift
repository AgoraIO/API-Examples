//
//  ReplaceSegue.swift
//  Agora-Rtm-Tutorial-Mac
//
//  Created by CavanSu on 2019/1/31.
//  Copyright © 2019 Agora. All rights reserved.
//

import Cocoa

class ReplaceSegue: NSStoryboardSegue {
    override func perform() {
        let sourceVC = self.sourceController as! NSViewController
        sourceVC.view.window?.contentViewController = self.destinationController as? NSViewController
    }
}
