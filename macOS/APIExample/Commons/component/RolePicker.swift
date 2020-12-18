//
//  RolePicker.swift
//  APIExample
//
//  Created by XC on 2020/12/18.
//  Copyright © 2020 Agora Corp. All rights reserved.
//

import Cocoa
import AgoraRtcKit

class RolePicker: NSPopUpButton {
    
    private let roles = AgoraClientRole.allValues()
    var agoraState: AgoraState?
    
    var role: AgoraClientRole {
        get {
            roles[indexOfSelectedItem >= 0 && indexOfSelectedItem < roles.count ? indexOfSelectedItem : 0]
        }
    }
    
    @IBAction func onSelect(_ sender: RolePicker) {
        guard let join = agoraState?.isJoined else { return }
        if join {
            agoraState?.agoraKit?.setClientRole(roles[sender.indexOfSelectedItem])
        }
    }
    
    required init?(coder: NSCoder) {
        super.init(coder: coder)
        addItems(withTitles: roles.map { $0.description() })
        target = self
        action = #selector(onSelect)
    }
}

