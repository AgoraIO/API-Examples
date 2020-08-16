//
//  QuickSwitchChannelVCItem.swift
//  Quick-Switch
//
//  Created by GongYuhua on 2019/2/25.
//  Copyright © 2019 Agora. All rights reserved.
//

import UIKit

class QuickSwitchChannelVCItem: UIViewController {

    @IBOutlet weak var hostLabel: UILabel!
    @IBOutlet weak var channelLabel: UILabel!
    @IBOutlet weak var spinner: UIActivityIndicatorView!
    @IBOutlet weak var hostRenderView: UIView!
    
    var channel: ChannelInfo!
    var index = 0
    
    override func viewDidLoad() {
        super.viewDidLoad()
        updateView(with: channel)
    }
    
    /// update channel info
    func updateView(with channel: ChannelInfo) {
        channelLabel.text = "Channel: \(channel.channelName)"
    }
    
    /// show loading animation
    func showLoading(_ shouldShow: Bool) {
        if shouldShow {
            spinner.startAnimating()
        } else {
            spinner.stopAnimating()
        }
    }
}
