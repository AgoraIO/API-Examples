//
//  JoinChannelVideoRTC.swift
//  APIExample-SwiftUI
//
//  Created by zhaoyongqiang on 2024/3/19.
//

import AgoraRtcKit
import SwiftUI

class PrecallTestRTC: NSObject, ObservableObject {
    private var agoraKit: AgoraRtcEngineKit!
    private var isJoined: Bool = false
    private var timer: Timer?
    private var localView: VideoUIView?
    
    @Published var startActivityView: Bool = false
    @Published var isStartCameraTest: Bool = false
    @Published var countDown: Int = 10
    @Published var isRecordEnd: Bool = false
    @Published var isEchoTestEnd: Bool = true
    @Published var lastmileResultLabel: String = ""
    @Published var isStartLastmileTest: Bool = false
    
    func setupRTC(configs: [String: Any], localView: VideoUIView) {
        self.localView = localView
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
    
    func doLastmileTest() {
        agoraKit.stopEchoTest()
        if isStartCameraTest {
            doCameraTest()
        }
        startActivityView = true
        let config = AgoraLastmileProbeConfig()
        // do uplink testing
        config.probeUplink =  true
        // do downlink testing
        config.probeDownlink = true
        // expected uplink bitrate, range: [100000, 5000000]
        config.expectedUplinkBitrate = 100000
        // expected downlink bitrate, range: [100000, 5000000]
        config.expectedDownlinkBitrate = 100000
        agoraKit.startLastmileProbeTest(config)
        isStartLastmileTest = true
    }
    
    func doEchoTest() {
        isStartLastmileTest = false
        agoraKit.stopLastmileProbeTest()
        if isStartCameraTest {
            doCameraTest()
        }
        let testConfig = AgoraEchoTestConfiguration()
        testConfig.intervalInSeconds = 10
        testConfig.enableAudio = true
        testConfig.enableVideo = false
        testConfig.channelId = "AudioEchoTest" + "\(Int.random(in: 1...1000))"
        let ret = agoraKit.startEchoTest(withConfig: testConfig)
        if ret != 0 {
            // Usually happens with invalid parameters
            // Error code description can be found at:
            // en: https://api-ref.agora.io/en/video-sdk/ios/4.x/documentation/agorartckit/agoraerrorcode
            // cn: https://doc.shengwang.cn/api-ref/rtc/ios/error-code
            LogUtils.log(message: "startEchoTest call failed: \(ret), please check your params", level: .error)
        }
        isRecordEnd = false
        isEchoTestEnd = false
        showPopover(seconds: 10) { [weak self] in
            self?.isRecordEnd = true
            self?.showPopover(seconds: 10) { [weak self] in
                self?.agoraKit.stopEchoTest()
                self?.isEchoTestEnd = true
            }
        }
    }
    
    func doCameraTest() {
        isStartCameraTest.toggle()
        isStartLastmileTest = false
        agoraKit.stopEchoTest()
        if isStartCameraTest {
            let testConfig = AgoraEchoTestConfiguration()
            testConfig.intervalInSeconds = 2
            testConfig.enableAudio = true
            testConfig.enableVideo = true
            testConfig.channelId = "VideoEchoTest" + "\(Int.random(in: 1...1000))"
            testConfig.view = localView?.videoView
            let ret = agoraKit.startEchoTest(withConfig: testConfig)
            if ret != 0 {
                // Usually happens with invalid parameters
                // Error code description can be found at:
                // en: https://api-ref.agora.io/en/video-sdk/ios/4.x/documentation/agorartckit/agoraerrorcode
                // cn: https://doc.shengwang.cn/api-ref/rtc/ios/error-code
                LogUtils.log(message: "startEchoTest call failed: \(ret), please check your params", level: .error)
            }
        } else {
            agoraKit.stopEchoTest()
        }
    }
    
    // show popover and hide after seconds
    func showPopover(seconds: Int, callback: @escaping (() -> Void)) {
        countDown = seconds
        timer = Timer.scheduledTimer(withTimeInterval: 1, repeats: true) { [weak self] _ in
            guard let self = self else { return }
            self.countDown -= 1
            // swiftlint:disable empty_count
            if self.countDown == 0 {
                // swiftlint:enable empty_count
                self.timer?.invalidate()
                callback()
            }
        }
    }
    
    func onDestory() {
        timer?.invalidate()
        timer = nil
        agoraKit.stopEchoTest()
        agoraKit.stopLastmileProbeTest()
        AgoraRtcEngineKit.destroy()
    }
}

// agora rtc engine delegate events
extension PrecallTestRTC: AgoraRtcEngineDelegate {
    /// callback to get lastmile quality 2seconds after startLastmileProbeTest
    func rtcEngine(_ engine: AgoraRtcEngineKit, lastmileQuality quality: AgoraNetworkQuality) {
        lastmileResultLabel = "Quality: \(quality.description())"
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
        
        let result = [rtt,
                      downlinkBandwidth,
                      downlinkJitter,
                      downlinkLoss,
                      uplinkBandwidth,
                      uplinkJitter,
                      uplinkLoss].joined(separator: "\n")
        lastmileResultLabel += "\n" + result
        // stop testing after get last mile detail result
        engine.stopLastmileProbeTest()
        startActivityView = false
    }
}
