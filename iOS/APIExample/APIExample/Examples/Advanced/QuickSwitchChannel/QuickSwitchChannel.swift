//
//  QuickSwitchChannel.swift
//  APIExample
//
//  Created by 张乾泽 on 2020/4/17.
//  Copyright © 2020 Agora Corp. All rights reserved.
//
import UIKit
import AGEVideoLayout
import AgoraRtcKit

struct ChannelInfo {
    let channelName: String
}

extension ChannelInfo {
    /// static function to generate 4 channels based on given channel name
    static func AllChannelList(_ channelName:String) -> [ChannelInfo] {
        var channels = [ChannelInfo]()
        for index in 1..<5 {
            let channel = ChannelInfo(
                channelName: "\(channelName)\(index)"
            )
            channels.append(channel)
        }
        return channels
    }
}

class QuickSwitchChannel: BaseViewController {
    var pageViewController: UIPageViewController!
    var channels = [ChannelInfo]()
    var currentIndex = 0
    var hostView: UIView?
    var hostChannel: ChannelInfo?
    var agoraKit: AgoraRtcEngineKit!
    
    // indicate if current instance has joined channel
    var isJoined: Bool = false
    
    /// setup page controller, it will auto generates 4 channels with corresponding view controllers for you to swipe, every time you swipe to a new viewcontroller it will switch to that channel
    func setupPageController(channelName: String) {
        // generate all channel infos
        channels = ChannelInfo.AllChannelList(channelName)
        pageViewController = UIPageViewController(transitionStyle: .scroll, navigationOrientation: .horizontal, options: nil)
        pageViewController.delegate = self
        
        if let vc = channelViewController(at: currentIndex) {
            pageViewController.setViewControllers([vc], direction: .forward, animated: false, completion: nil)
            // there is only 1 rtc engine instance
            // We will have to mark the current active view controller video renderer, so that we can use it in later didJoinedOfUser events
            // setHostViewController grabs the hostRenderView from current active view controller, and store it in hostView property
            setHostViewController(vc)
        }
        pageViewController.dataSource = self
        
        // add page view controller as child view controller
        addChild(pageViewController)
        view.addSubview(pageViewController.view)
        pageViewController!.view.frame = view.bounds
        pageViewController.didMove(toParent: self)
        
        // Add the page view controller's gesture recognizers to the view controller's view so that the gestures are started more easily.
        view.gestureRecognizers = pageViewController.gestureRecognizers
    }
    
    func setHostViewController(_ viewController: QuickSwitchChannelVCItem) {
        hostChannel = viewController.channel
        hostView = viewController.hostRenderView
        // every time we switch a view controller, we will rejoin the channel by calling switchChannel, so we always call showLoading
        viewController.showLoading(true)
    }
    
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
        
        // get channel name from configs
        guard let channelName = configs["channelName"] as? String else {return}
        
        // setup UIPageController for swipe changing vc
        setupPageController(channelName:channelName)
        
        // enable video module
        agoraKit.enableVideo()
        agoraKit.enableAudio()
        // make myself an audience
        agoraKit.setChannelProfile(.liveBroadcasting)
        agoraKit.setClientRole(.audience)
        
        // Set audio route to speaker
        agoraKit.setDefaultAudioRouteToSpeakerphone(true)
        
        // start joining channel
        // 1. Users can only see each other after they join the
        // same channel successfully using the same app id.
        // 2. If app certificate is turned on at dashboard, token is needed
        // when joining channel. The channel name and uid used to calculate
        // the token has to match the ones used for channel join
        NetworkManager.shared.generateToken(channelName: channels[currentIndex].channelName, success: { token in
            let result = self.agoraKit.joinChannel(byToken: token, channelId: self.channels[self.currentIndex].channelName, info: nil, uid: 0)
            if result != 0 {
                // Usually happens with invalid parameters
                // Error code description can be found at:
                // en: https://api-ref.agora.io/en/voice-sdk/macos/3.x/Constants/AgoraErrorCode.html#content
                // cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
                self.showAlert(title: "Error", message: "joinChannel call failed: \(result), please check your params")
            }
        })
    }
    
    override func willMove(toParent parent: UIViewController?) {
        if parent == nil {
            // leave channel when exiting the view
            if isJoined {
                agoraKit.disableAudio()
                agoraKit.disableVideo()
                agoraKit.leaveChannel { (stats) -> Void in
                    LogUtils.log(message: "left channel, duration: \(stats.duration)", level: .info)
                }
            }
        }
    }
}

/// agora rtc engine delegate events
extension QuickSwitchChannel: AgoraRtcEngineDelegate {
    /// callback when current user join channel successfully
    /// @param channel channel name
    /// @param uid my uid
    /// @param elapsed elapsed time since joinChannel is called
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinChannel channel: String, withUid uid: UInt, elapsed: Int) {
        self.isJoined = true
        LogUtils.log(message: "Join \(channel) with uid \(uid) elapsed \(elapsed)ms", level: .info)
        
        // check if the callback should be responding
        guard let currentVC = pageViewController.viewControllers?.first as? QuickSwitchChannelVCItem,
            currentVC.channel.channelName == channel else {
            return
        }
        
        // hide loading
        currentVC.showLoading(false)
    }
    
    /// callback when warning occured for agora sdk, warning can usually be ignored, still it's nice to check out
    /// what is happening
    /// Warning code description can be found at:
    /// en: https://api-ref.agora.io/en/voice-sdk/ios/3.x/Constants/AgoraWarningCode.html
    /// cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraWarningCode.html
    /// @param warningCode warning code of the problem
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurWarning warningCode: AgoraWarningCode) {
        LogUtils.log(message: "warning: \(warningCode.description)", level: .warning)
    }
    
    /// callback when error occured for agora sdk, you are recommended to display the error descriptions on demand
    /// to let user know something wrong is happening
    /// Error code description can be found at:
    /// en: https://api-ref.agora.io/en/voice-sdk/macos/3.x/Constants/AgoraErrorCode.html#content
    /// cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
    /// @param errorCode error code of the problem
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurError errorCode: AgoraErrorCode) {
        LogUtils.log(message: "error: \(errorCode)", level: .error)
        self.showAlert(title: "Error", message: "Error \(errorCode.description) occur")
    }
    
    /// callback when a remote user is joinning the channel, note audience in live broadcast mode will NOT trigger this event
    /// @param uid uid of remote joined user
    /// @param elapsed time elapse since current sdk instance join the channel in ms
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinedOfUid uid: UInt, elapsed: Int) {
        LogUtils.log(message: "remote user join: \(uid) \(elapsed)ms", level: .info)
        
        // check if the callback should be responding
        guard let currentVC = pageViewController.viewControllers?.first as? QuickSwitchChannelVCItem,
            currentVC.channel.channelName == hostChannel?.channelName else {
            return
        }
        
        // Only one remote video view is available for this
        // tutorial. Here we check if there exists a surface
        // view tagged as this uid.
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.uid = uid
        // the view to be binded
        videoCanvas.view = hostView
        videoCanvas.renderMode = .hidden
        agoraKit.setupRemoteVideo(videoCanvas)
    }
    
    /// callback when a remote user is leaving the channel, note audience in live broadcast mode will NOT trigger this event
    /// @param uid uid of remote joined user
    /// @param reason reason why this user left, note this event may be triggered when the remote user
    /// become an audience in live broadcasting profile
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOfflineOfUid uid: UInt, reason: AgoraUserOfflineReason) {
        LogUtils.log(message: "remote user left: \(uid) reason \(reason)", level: .info)
        
        // check if the callback should be responding
        guard let currentVC = pageViewController.viewControllers?.first as? QuickSwitchChannelVCItem,
            currentVC.channel.channelName == hostChannel?.channelName else {
            return
        }
        
        // to unlink your view from sdk, so that your view reference will be released
        // note the video will stay at its last frame, to completely remove it
        // you will need to remove the EAGL sublayer from your binded view
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.uid = uid
        // the view to be binded
        videoCanvas.view = nil
        videoCanvas.renderMode = .hidden
        agoraKit.setupRemoteVideo(videoCanvas)
    }
}

private extension QuickSwitchChannel {
    /// api to generate QuickSwitchChannelVCItem on demand
    func channelViewController(at index: Int) -> QuickSwitchChannelVCItem? {
        guard channels.count > 0 else {
            return nil
        }
        
        var vcIndex = index
        // if vcIndex exceeds maximum, reset to 0
        if vcIndex >= channels.count {
            vcIndex = 0
        // else if vcIndex < 0, reset to last value
        } else if vcIndex < 0 {
            vcIndex = channels.count - 1
        }
        
        // create the view controller with info
        let channelVC = QuickSwitchChannelVCItem(nibName: "QuickSwitchChannelVCItem", bundle: nil)
        channelVC.index = vcIndex
        channelVC.channel = channels[vcIndex]
        
        return channelVC
    }
}

/// Page View Controller DataSource
extension QuickSwitchChannel: UIPageViewControllerDataSource {
    func pageViewController(_ pageViewController: UIPageViewController, viewControllerAfter viewController: UIViewController) -> UIViewController? {
        guard let channelVC = viewController as? QuickSwitchChannelVCItem else {
            return nil
        }
        // return next VC
        return channelViewController(at: channelVC.index + 1)
    }
    
    func pageViewController(_ pageViewController: UIPageViewController, viewControllerBefore viewController: UIViewController) -> UIViewController? {
        guard let channelVC = viewController as? QuickSwitchChannelVCItem else {
            return nil
        }
        // return prev VC
        return channelViewController(at: channelVC.index - 1)
    }
}

/// Page View Controller Delegate
extension QuickSwitchChannel : UIPageViewControllerDelegate
{
    func pageViewController(_ pageViewController: UIPageViewController, didFinishAnimating finished: Bool, previousViewControllers: [UIViewController], transitionCompleted completed: Bool) {
        guard let previousVC = previousViewControllers.last as? QuickSwitchChannelVCItem,
            let currentVC = pageViewController.viewControllers?.first as? QuickSwitchChannelVCItem,
            previousVC != currentVC else {
            return
        }
        
        // switch to currentVC and its hosted channel
        setHostViewController(currentVC)
        // MIGRATED
        // leave and join new channel
        agoraKit.leaveChannel(nil)
        NetworkManager.shared.generateToken(channelName: currentVC.channel.channelName, success: { token in
            self.agoraKit.joinChannel(byToken: token, channelId: currentVC.channel.channelName, info: nil, uid: 0, joinSuccess: nil)
        })
    }
}
