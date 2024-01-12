//
//  JoinChannelVC.swift
//  APIExample
//
//  Created by 张乾泽 on 2020/4/17.
//  Copyright © 2020 Agora Corp. All rights reserved.
//
import Cocoa
import AgoraRtcKit
import AGEVideoLayout

class PrecallTest: BaseViewController {
    var videos: [VideoView] = []
    var timer:Timer?
    
    @IBOutlet weak var cameraPicker: NSPopUpButton!
    @IBOutlet weak var micPicker: NSPopUpButton!
    @IBOutlet weak var speakerPicker: NSPopUpButton!
    @IBOutlet weak var startCameraTestBtn: NSButton!
    @IBOutlet weak var stopCameraTestBtn: NSButton!
    @IBOutlet weak var startMicTestBtn: NSButton!
    @IBOutlet weak var stopMicTestBtn: NSButton!
    @IBOutlet weak var startSpeakerTestBtn: NSButton!
    @IBOutlet weak var stopSpeakerTestBtn: NSButton!
    @IBOutlet weak var startLoopbackTestBtn: NSButton!
    @IBOutlet weak var stopLoopbackTestBtn: NSButton!
    @IBOutlet weak var startLastmileTestBtn: NSButton!
    @IBOutlet weak var lastmileResultLabel: NSTextField!
    @IBOutlet weak var lastmileProbResultLabel: NSTextField!
    @IBOutlet weak var lastmileActivityView: NSProgressIndicator!
    @IBOutlet weak var micTestingVolumeIndicator: NSProgressIndicator!
    @IBOutlet weak var echoTestCountDownLabel: NSTextField!
    @IBOutlet weak var echoTestPopover: NSView!
    @IBOutlet weak var echoValidateCountDownLabel: NSTextField!
    @IBOutlet weak var echoValidatePopover: NSView!
    var cameras:[AgoraRtcDeviceInfo] = [] {
        didSet {
            DispatchQueue.main.async {
                self.cameraPicker.addItems(withTitles: self.cameras.map({ (device: AgoraRtcDeviceInfo) -> String in
                    return (device.deviceName ?? "")
                }))
            }
        }
    }
    var mics:[AgoraRtcDeviceInfo] = [] {
        didSet {
            DispatchQueue.main.async {
                self.micPicker.addItems(withTitles: self.mics.map({ (device: AgoraRtcDeviceInfo) -> String in
                    return (device.deviceName ?? "")
                }))
            }
        }
    }
    var speakers:[AgoraRtcDeviceInfo] = [] {
        didSet {
            DispatchQueue.main.async {
                self.speakerPicker.addItems(withTitles: self.speakers.map({ (device: AgoraRtcDeviceInfo) -> String in
                    return (device.deviceName ?? "")
                }))
            }
        }
    }
    
    // indicate if camera testing is going on
    var isTestingCamera: Bool = false {
        didSet {
            startCameraTestBtn.isHidden = isTestingCamera
            stopCameraTestBtn.isHidden = !isTestingCamera
        }
    }
    
    // indicate if mic testing is going on
    var isTestingMic: Bool = false {
        didSet {
            startMicTestBtn.isHidden = isTestingMic
            stopMicTestBtn.isHidden = !isTestingMic
            startLoopbackTestBtn.isEnabled = !isTestingMic
        }
    }
    
    // indicate if speaker testing is going on
    var isTestingSpeaker: Bool = false {
        didSet {
            startSpeakerTestBtn.isHidden = isTestingSpeaker
            stopSpeakerTestBtn.isHidden = !isTestingSpeaker
            startLoopbackTestBtn.isEnabled = !isTestingSpeaker
        }
    }
    
    // indicate if speaker testing is going on
    var isTestingLoopback: Bool = false {
        didSet {
            startLoopbackTestBtn.isHidden = isTestingLoopback
            stopLoopbackTestBtn.isHidden = !isTestingLoopback
            
            startMicTestBtn.isEnabled = !isTestingLoopback
            startSpeakerTestBtn.isEnabled = !isTestingLoopback
        }
    }
    
    @IBOutlet weak var container: AGEVideoContainer!
    var agoraKit: AgoraRtcEngineKit!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        layoutVideos()
        
        // set up agora instance when view loaded
        let config = AgoraRtcEngineConfig()
        config.appId = KeyCenter.AppId
        config.areaCode = GlobalSettings.shared.area
        agoraKit = AgoraRtcEngineKit.sharedEngine(with: config, delegate: self)
        // Configuring Privatization Parameters
        Util.configPrivatization(agoraKit: agoraKit)
        // this is mandatory to get camera list
        agoraKit.enableVideo()
        
        //find device in a separate thread to avoid blocking main thread
        let queue = DispatchQueue(label: "device.enumerateDevices")
        queue.async {[unowned self] in
            self.cameras = self.agoraKit.enumerateDevices(.videoCapture) ?? []
            self.mics = self.agoraKit.enumerateDevices(.audioRecording) ?? []
            self.speakers = self.agoraKit.enumerateDevices(.audioPlayout) ?? []
        }
    }
    
    override func viewWillBeRemovedFromSplitView() {
        timer?.invalidate()
        agoraKit.stopEchoTest()
        agoraKit.stopLastmileProbeTest()
        AgoraRtcEngineKit.destroy()
    }
    
    @IBAction func onStartCameraTest(_ sender:NSButton) {
        // use selected devices
        if let cameraId = cameras[cameraPicker.indexOfSelectedItem].deviceId {
            agoraKit.setDevice(.videoCapture, deviceId: cameraId)
        }
        agoraKit.startCaptureDeviceTest(videos[0])
        isTestingCamera = true
    }
    
    @IBAction func onStopCameraTest(_ sender:NSButton) {
        agoraKit.stopCaptureDeviceTest()
        isTestingCamera = false
    }
    
    @IBAction func onStartMicTest(_ sender:NSButton) {
        // use selected devices
        if let micId = mics[micPicker.indexOfSelectedItem].deviceId {
            agoraKit.setDevice(.audioRecording, deviceId: micId)
        }
        agoraKit.startRecordingDeviceTest(50)
        isTestingMic = true
    }
    
    @IBAction func onStopMicTest(_ sender:NSButton) {
        agoraKit.stopRecordingDeviceTest()
        isTestingMic = false
    }
    
    @IBAction func onStartSpeakerTest(_ sender:NSButton) {
        // use selected devices
        if let speakerId = speakers[speakerPicker.indexOfSelectedItem].deviceId {
            agoraKit.setDevice(.audioPlayout, deviceId: speakerId)
        }
        
        if let filepath = Bundle.main.path(forResource: "audiomixing", ofType: "mp3") {
            let result = agoraKit.startPlaybackDeviceTest(filepath)
            if result != 0 {
                self.showAlert(title: "Error", message: "startAudioMixing call failed: \(result), please check your params")
            }
            isTestingSpeaker = true
        }
    }
    
    @IBAction func onStopSpeakerTest(_ sender:NSButton) {
        agoraKit.stopPlaybackDeviceTest()
        isTestingSpeaker = false
    }
    
    @IBAction func onStartLoopbackTest(_ sender:NSButton) {
        // use selected devices
        if let micId = mics[micPicker.indexOfSelectedItem].deviceId {
            agoraKit.setDevice(.audioRecording, deviceId: micId)
        }
        if let speakerId = speakers[speakerPicker.indexOfSelectedItem].deviceId {
            agoraKit.setDevice(.audioPlayout, deviceId: speakerId)
        }
        agoraKit.startAudioDeviceLoopbackTest(50)
        isTestingLoopback = true
    }
    
    @IBAction func onStopLoopbackTest(_ sender:NSButton) {
        agoraKit.stopAudioDeviceLoopbackTest()
        isTestingLoopback = false
    }
    
    @IBAction func onStartLastmileTest(_ sender:NSButton) {
        lastmileActivityView.startAnimation(nil)
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
    
    @IBAction func doEchoTest(sender: NSButton) {
        let testConfig = AgoraEchoTestConfiguration()
        testConfig.intervalInSeconds = 10
        testConfig.enableAudio = true
        testConfig.enableVideo = false
        testConfig.channelId = "AudioEchoTest" + "\(Int.random(in: 1...1000))"
        agoraKit.startEchoTest(withConfig: testConfig)
        showPopover(isValidate: false, seconds: 10) {[unowned self] in
            self.showPopover(isValidate: true, seconds: 10) {[unowned self] in
                self.agoraKit.stopEchoTest()
            }
        }
    }
    
    private var isStartVideoEchoTest: Bool = false
    @IBAction func onStartVideoEchoTest(_ sender: NSButton) {
        isStartVideoEchoTest = !isStartVideoEchoTest
        if let cameraId = cameras[cameraPicker.indexOfSelectedItem].deviceId {
            agoraKit.setDevice(.videoCapture, deviceId: cameraId)
        }
        let testConfig = AgoraEchoTestConfiguration()
        testConfig.intervalInSeconds = 2
        testConfig.enableAudio = false
        testConfig.enableVideo = isStartVideoEchoTest
        testConfig.channelId = "VideoEchoTest" + "\(Int.random(in: 1...1000))"
        testConfig.view = videos[0]
        if isStartVideoEchoTest {
            let ret = agoraKit.startEchoTest(withConfig: testConfig)
            if ret != 0 {
                // Usually happens with invalid parameters
                // Error code description can be found at:
                // en: https://api-ref.agora.io/en/video-sdk/ios/4.x/documentation/agorartckit/agoraerrorcode
                // cn: https://doc.shengwang.cn/api-ref/rtc/ios/error-code
                showAlert(title: "Error", message: "startEchoTest call failed: \(ret), please check your params")
            }
        } else {
            agoraKit.stopEchoTest()
        }
        sender.title = isStartVideoEchoTest ? "Stop Video Echo Test".localized : "Start Video Echo Test".localized
        startCameraTestBtn.isHidden = isStartVideoEchoTest
        stopCameraTestBtn.isHidden = true
    }
    
    
    // show popover and hide after seconds
    func showPopover(isValidate:Bool, seconds:Int, callback:@escaping (() -> Void)) {
        var count = seconds
        var countDownLabel:NSTextField?
        var popover:NSView?
        if(isValidate) {
            countDownLabel = echoValidateCountDownLabel
            popover = echoValidatePopover
        } else {
            countDownLabel = echoTestCountDownLabel
            popover = echoTestPopover
        }
        
        countDownLabel?.stringValue = "\(count)"
        popover?.isHidden = false
        timer = Timer.scheduledTimer(withTimeInterval: 1, repeats: true) {[unowned self] (timer) in
            count -= 1
            countDownLabel?.stringValue = "\(count)"
            
            if(count == 0) {
                self.timer?.invalidate()
                popover?.isHidden = true
                callback()
            }
        }
    }
    
    func layoutVideos() {
        let view = VideoView.createFromNib()!
        view.placeholder.stringValue = "Camera Test Preview"
        videos = [view]
        // layout render view
        container.layoutStream(views: videos)
    }
}

/// agora rtc engine delegate events
extension PrecallTest: AgoraRtcEngineDelegate {
    /// callback when warning occured for agora sdk, warning can usually be ignored, still it's nice to check out
    /// what is happening
    /// Warning code description can be found at:
    /// en: https://api-ref.agora.io/en/voice-sdk/ios/3.x/Constants/AgoraWarningCode.html
    /// cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraWarningCode.html
    /// @param warningCode warning code of the problem
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurWarning warningCode: AgoraWarningCode) {
        LogUtils.log(message: "warning: \(warningCode.rawValue)", level: .warning)
    }
    
    /// callback when error occured for agora sdk, you are recommended to display the error descriptions on demand
    /// to let user know something wrong is happening
    /// Error code description can be found at:
    /// en: https://api-ref.agora.io/en/video-sdk/ios/4.x/documentation/agorartckit/agoraerrorcode
    /// cn: https://doc.shengwang.cn/api-ref/rtc/ios/error-code
    /// @param errorCode error code of the problem
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurError errorCode: AgoraErrorCode) {
        LogUtils.log(message: "error: \(errorCode)", level: .error)
        self.showAlert(title: "Error", message: "Error \(errorCode.rawValue) occur")
    }
    
    /// Reports which users are speaking, the speakers' volumes, and whether the local user is speaking.
    /// @params speakers volume info for all speakers
    /// @params totalVolume Total volume after audio mixing. The value range is [0,255].
    func rtcEngine(_ engine: AgoraRtcEngineKit, reportAudioVolumeIndicationOfSpeakers speakers: [AgoraRtcAudioVolumeInfo], totalVolume: Int) {
        for speaker in speakers {
            print("reportAudioVolumeIndicationOfSpeakers:\(speaker.uid), \(speaker.volume)")
            if(speaker.uid == 0) {
                micTestingVolumeIndicator.doubleValue = Double(speaker.volume)
            }
        }
    }
    
    /// callback to get lastmile quality 2seconds after startLastmileProbeTest
    func rtcEngine(_ engine: AgoraRtcEngineKit, lastmileQuality quality: AgoraNetworkQuality) {
        lastmileResultLabel.stringValue = "Quality: \(quality.description())"
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
        
        lastmileProbResultLabel.stringValue = [rtt, downlinkBandwidth, downlinkJitter, downlinkLoss, uplinkBandwidth, uplinkJitter, uplinkLoss].joined(separator: "\n")
        
        // stop testing after get last mile detail result
        engine.stopLastmileProbeTest()
        lastmileActivityView.stopAnimation(nil)
    }
}
