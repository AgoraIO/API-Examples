//
//  PrecallTest.swift
//  APIExample
//
//  Created by 张乾泽 on 2020/4/17.
//  Copyright © 2020 Agora Corp. All rights reserved.
//
import UIKit
import AGEVideoLayout
import AgoraRtcKit

class PrecallTestEntry : BaseViewController
{
    var agoraKit: AgoraRtcEngineKit!
    var timer:Timer?
    @IBOutlet weak var lastmileBtn: UIButton!
    @IBOutlet weak var lastmileResultLabel: UILabel!
    @IBOutlet weak var lastmileProbResultLabel: UILabel!
    @IBOutlet weak var lastmileActivityView: UIActivityIndicatorView!
    @IBOutlet weak var echoTestCountDownLabel: UILabel!
    @IBOutlet weak var echoTestPopover: UIView!
    @IBOutlet weak var echoValidateCountDownLabel: UILabel!
    @IBOutlet weak var echoValidatePopover: UIView!
    override func viewDidLoad() {
        super.viewDidLoad()
        
        // set up agora instance when view loaded
        let config = AgoraRtcEngineConfig()
        config.appId = KeyCenter.AppId
        config.areaCode = GlobalSettings.shared.area
        config.channelProfile = .liveBroadcasting
        
        agoraKit = AgoraRtcEngineKit.sharedEngine(with: config, delegate: self)
        // Configuring Privatization Parameters
        Util.configPrivatization(agoraKit: agoraKit)
        agoraKit.setLogFile(LogUtils.sdkLogPath())
        
        // have to be a broadcaster for doing echo test
        agoraKit.setClientRole(GlobalSettings.shared.getUserRole())
    }
    
    
    @IBAction func doLastmileTest(sender: UIButton) {
        lastmileActivityView.startAnimating()
        let config = AgoraLastmileProbeConfig()
        // do uplink testing
        config.probeUplink =  true;
        // do downlink testing
        config.probeDownlink = true;
        // expected uplink bitrate, range: [100000, 5000000]
        config.expectedUplinkBitrate = 100000;
        // expected downlink bitrate, range: [100000, 5000000]
        config.expectedDownlinkBitrate = 100000;
        agoraKit.startLastmileProbeTest(config)
    }
    
    @IBAction func doEchoTest(sender: UIButton) {
        
        let ret = agoraKit.startEchoTest(withInterval: 10, successBlock: nil)
        if ret != 0 {
            // Usually happens with invalid parameters
            // Error code description can be found at:
            // en: https://api-ref.agora.io/en/voice-sdk/macos/3.x/Constants/AgoraErrorCode.html#content
            // cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
            showAlert(title: "Error", message: "startEchoTest call failed: \(ret), please check your params")
        }
        showPopover(isValidate: false, seconds: 10) {[unowned self] in
            self.showPopover(isValidate: true, seconds: 10) {[unowned self] in
                self.agoraKit.stopEchoTest()
            }
        }
    }
    
    // show popover and hide after seconds
    func showPopover(isValidate:Bool, seconds:Int, callback:@escaping (() -> Void)) {
        var count = seconds
        var countDownLabel:UILabel?
        var popover:UIView?
        if(isValidate) {
            countDownLabel = echoValidateCountDownLabel
            popover = echoValidatePopover
        } else {
            countDownLabel = echoTestCountDownLabel
            popover = echoTestPopover
        }
        
        countDownLabel?.text = "\(count)"
        popover?.isHidden = false
        timer = Timer.scheduledTimer(withTimeInterval: 1, repeats: true) {[unowned self] (timer) in
            count -= 1
            countDownLabel?.text = "\(count)"
            
            if(count == 0) {
                self.timer?.invalidate()
                popover?.isHidden = true
                callback()
            }
        }
    }
    
    override func willMove(toParent parent: UIViewController?) {
        if parent == nil {
            // clean up
            // important, you will not be able to join a channel
            // if you are in the middle of a testing
            timer?.invalidate()
            agoraKit.stopEchoTest()
            agoraKit.stopLastmileProbeTest()
        }
    }
}

extension PrecallTestEntry:AgoraRtcEngineDelegate
{
    /// callback to get lastmile quality 2seconds after startLastmileProbeTest
    func rtcEngine(_ engine: AgoraRtcEngineKit, lastmileQuality quality: AgoraNetworkQuality) {
        lastmileResultLabel.text = "Quality: \(quality.description())"
    }
    
    /// callback to get more detail lastmile quality after startLastmileProbeTest
    func rtcEngine(_ engine: AgoraRtcEngineKit, lastmileProbeTest result: AgoraLastmileProbeResult) {
        let rtt = "Rtt: \(result.rtt)ms"
        let downlinkBandwidth = "DownlinkAvailableBandwidth: \(result.downlinkReport.availableBandwidth)Kbps"
        let downlinkJitter = "DownlinkJitter: \(result.downlinkReport.jitter)ms"
        let downlinkLoss = "DownlinkLoss: \(result.downlinkReport.packetLossRate)%"
        
        let uplinkBandwidth = "UplinkAvailableBandwidth: \(result.uplinkReport.availableBandwidth)Kbps"
        let uplinkJitter = "UplinkJitter: \(result.uplinkReport.jitter)ms"
        let uplinkLoss = "UplinkLoss: \(result.uplinkReport.packetLossRate)%"
        
        lastmileProbResultLabel.text = [rtt, downlinkBandwidth, downlinkJitter, downlinkLoss, uplinkBandwidth, uplinkJitter, uplinkLoss].joined(separator: "\n")
        
        // stop testing after get last mile detail result
        engine.stopLastmileProbeTest()
        lastmileActivityView.stopAnimating()
    }
}
