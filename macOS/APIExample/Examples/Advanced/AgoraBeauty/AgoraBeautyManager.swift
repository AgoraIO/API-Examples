//
//  AgoraBeautyManager.swift
//  APIExample
//
//  Created by qinhui on 2025/5/27.
//  Copyright © 2025 Agora Corp. All rights reserved.
//

import Foundation
import AgoraRtcKit

class AgoraBeautyManager {
    weak var agoraKit: AgoraRtcEngineKit?
    private var videoEffectObject: AgoraVideoEffectObject?
    private lazy var faceshapeOption = AgoraFaceShapeBeautyOptions()
    private var styleParam: [String : Any] = ["enable_mu": false]
    private var m_bundle_copied = false;
    let beauty_material_path: String = NSHomeDirectory() + "/Documents/beauty_material.bundle";
    let m_current_material_name = "beauty_material_v2.0.0";

    init(agoraKit: AgoraRtcEngineKit? = nil) {
        self.agoraKit = agoraKit
        initBeauty()
    }
    
    func destory() {
        let result = agoraKit?.destroyVideoEffectObject(videoEffectObject)
        if result == 0 {
            videoEffectObject = nil
        }
    }
    
    private func initBeauty() {
        agoraKit?.enableExtension(withVendor: "agora_video_filters_clear_vision",
                                  extension: "clear_vision",
                                  enabled: true,
                                  sourceType: .primaryCamera)
        copyBeautyBundle()
        let path = beauty_material_path + "/" + m_current_material_name;
        videoEffectObject = agoraKit?.createVideoEffectObject(bundlePath: path, sourceType: AgoraMediaSourceType.primaryCamera)
        agoraKit?.setParameters("{\"rtc.video.yuvconverter_enable_hardware_buffer\":true}")

    }
    
    private func copyBeautyBundle() {
        if (m_bundle_copied) {
            return
        }
        let bundle_path = Bundle.main.path(forResource: "beauty_material", ofType: "bundle")
        if FileManager.default.fileExists(atPath: beauty_material_path) {
            try? FileManager.default.removeItem(atPath: beauty_material_path)
        }
        try? FileManager.default.copyItem(atPath: bundle_path!, toPath: beauty_material_path)
        m_bundle_copied = true
    }
    
    private func addEffect(node : UInt) {
        let ret = self.videoEffectObject?.addOrUpdateVideoEffect(nodeId: node, templateName: "") ?? -1;
        print("addEffect ret: \(ret)")
    }
    
    private func removeEffect(node : UInt) {
        let ret = self.videoEffectObject?.removeVideoEffect(nodeId: node) ?? -1;
        print("removeEffect ret: \(ret)")
    }
    
    private func updateMaterialConfig(node : UInt, selection : String!) {
        let ret = self.videoEffectObject?.addOrUpdateVideoEffect(nodeId: node, templateName: selection) ?? -1;
        print("updateMaterialConfig ret: \(ret)")
    }
    
    func setBeauty(key: String?, value: Float) {
        let areaOption = AgoraFaceShapeAreaOptions()
        switch key {
        case "intensity":
            faceshapeOption.styleIntensity = Int32(value)
            updateFaceShape()
            return
        case "headscale":
            areaOption.shapeArea = AgoraFaceShapeArea.headScale
        case "forehead":
            areaOption.shapeArea = AgoraFaceShapeArea.forehead
        case "facecontour":
            areaOption.shapeArea = AgoraFaceShapeArea.faceContour
        case "facewidth":
            areaOption.shapeArea = AgoraFaceShapeArea.faceWidth
        case "facelength":
            areaOption.shapeArea = AgoraFaceShapeArea.faceLength
        case "cheekbone":
            areaOption.shapeArea = AgoraFaceShapeArea.cheekbone
        case "cheek":
            areaOption.shapeArea = AgoraFaceShapeArea.cheek
        case "chin":
            areaOption.shapeArea = AgoraFaceShapeArea.chin
        case "eyescale":
            areaOption.shapeArea = AgoraFaceShapeArea.eyeScale
        case "noselength":
            areaOption.shapeArea = AgoraFaceShapeArea.noseLength
        case "nosewidth":
            areaOption.shapeArea = AgoraFaceShapeArea.noseWidth
        case "mouthscale":
            areaOption.shapeArea = AgoraFaceShapeArea.mouthScale
        default:
            break
        }
        areaOption.shapeIntensity = Int32(value)
        agoraKit?.setFaceShapeAreaOptions(areaOption)
        updateFaceShape()
    }
    
    func updateFaceShape() {
        agoraKit?.setFaceShapeBeautyOptions(beautyShapeEnable, options: faceshapeOption)
    }
    
    func updateMakeup(parameter: [String : Any]) {
        guard let json = try? JSONSerialization.data(withJSONObject: parameter, options: []),
              let jsonString = String(data: json, encoding: .utf8) else {
            print("updateMakeup fail")
            return
        }
        
        let ret = self.agoraKit?.setExtensionPropertyWithVendor("agora_video_filters_clear_vision",
                                                               extension: "clear_vision",
                                                               key: "makeup_options",
                                                               value: jsonString) ?? -1
        print("updateMakeup ret: \(ret) jsonString: \(jsonString)")
    }
    
    var basicBeautyEnable: Bool {
        get {
            self.videoEffectObject?.getVideoEffectBoolParam(option: "beauty_effect_option", key: "enable") ?? false
        }
        
        set {
            if newValue {
                if beautyShapeStyle == nil {
                    self.addEffect(node: AgoraVideoEffectNodeId.beauty.rawValue)
                    self.videoEffectObject?.setVideoEffectBoolParam(option: "face_shape_beauty_option", key: "enable", boolValue: false)
                }
            }
            
            self.videoEffectObject?.setVideoEffectBoolParam(option: "beauty_effect_option", key: "enable", boolValue: newValue)
        }
    }

    // Smoothness
    var smoothness: Float {
        get {
            return videoEffectObject?.getVideoEffectFloatParam(option: "beauty_effect_option", key: "smoothness") ?? 0.9
        }
        set {
            guard let effectObj = videoEffectObject else { return }
            effectObj.setVideoEffectFloatParam(option: "beauty_effect_option", key: "smoothness", floatValue: newValue)
        }
    }

    // Lightness
    var lightness: Float {
        get {
            return videoEffectObject?.getVideoEffectFloatParam(option: "beauty_effect_option", key: "lightness") ?? 0.9
        }
        set {
            guard let effectObj = videoEffectObject else { return }
            effectObj.setVideoEffectFloatParam(option: "beauty_effect_option", key: "lightness", floatValue: newValue)
        }
    }

    // Redness
    var redness: Float {
        get {
            return videoEffectObject?.getVideoEffectFloatParam(option: "beauty_effect_option", key: "redness") ?? 1.0
        }
        set {
            guard let effectObj = videoEffectObject else { return }
            effectObj.setVideoEffectFloatParam(option: "beauty_effect_option", key: "redness", floatValue: newValue)
        }
    }

    // Sharpness
    var sharpness: Float {
        get {
            return videoEffectObject?.getVideoEffectFloatParam(option: "beauty_effect_option", key: "sharpness") ?? 1.0
        }
        set {
            guard let effectObj = videoEffectObject else { return }
            effectObj.setVideoEffectFloatParam(option: "beauty_effect_option", key: "sharpness", floatValue: newValue)
        }
    }

    /**
     * 0 Low contrast
     * 1 Normal contrast  
     * 2 High contrast
     */
    var contrast: Int32 {
        get {
            return videoEffectObject?.getVideoEffectIntParam(option: "beauty_effect_option", key: "contrast") ?? 1
        }
        set {
            guard let effectObj = videoEffectObject else { return }
            effectObj.setVideoEffectIntParam(option: "beauty_effect_option", key: "contrast", intValue: newValue)
        }
    }

    // Contrast strength
    var contrastStrength: Float {
        get {
            return videoEffectObject?.getVideoEffectFloatParam(option: "beauty_effect_option", key: "contrast_strength") ?? 1.0
        }
        set {
            guard let effectObj = videoEffectObject else { return }
            effectObj.setVideoEffectFloatParam(option: "beauty_effect_option", key: "contrast_strength", floatValue: newValue)
        }
    }
    
    // MARK: - Extension Beauty Properties
    
    // Eye pouch
    var eyePouch: Float {
        get {
            return videoEffectObject?.getVideoEffectFloatParam(option: "face_buffing_option", key: "eye_pouch") ?? 0.5
        }
        set {
            guard let effectObj = videoEffectObject else { return }
            effectObj.setVideoEffectFloatParam(option: "face_buffing_option", key: "eye_pouch", floatValue: newValue)
        }
    }

    // Brighten eye
    var brightenEye: Float {
        get {
            return videoEffectObject?.getVideoEffectFloatParam(option: "face_buffing_option", key: "brighten_eye") ?? 0.9
        }
        set {
            guard let effectObj = videoEffectObject else { return }
            effectObj.setVideoEffectFloatParam(option: "face_buffing_option", key: "brighten_eye", floatValue: newValue)
        }
    }

    // Nasolabial fold
    var nasolabialFold: Float {
        get {
            return videoEffectObject?.getVideoEffectFloatParam(option: "face_buffing_option", key: "nasolabial_fold") ?? 0.7
        }
        set {
            guard let effectObj = videoEffectObject else { return }
            effectObj.setVideoEffectFloatParam(option: "face_buffing_option", key: "nasolabial_fold", floatValue: newValue)
        }
    }

    // Whiten teeth
    var whitenTeeth: Float {
        get {
            return videoEffectObject?.getVideoEffectFloatParam(option: "face_buffing_option", key: "whiten_teeth") ?? 0.7
        }
        set {
            guard let effectObj = videoEffectObject else { return }
            effectObj.setVideoEffectFloatParam(option: "face_buffing_option", key: "whiten_teeth", floatValue: newValue)
        }
    }
    
    // MARK: - Beauty Shape Properties
    
    // Face shape switch
    var beautyShapeEnable: Bool {
        get {
            return videoEffectObject?.getVideoEffectBoolParam(option: "face_shape_beauty_option", key: "enable") ?? false
        }
        set {
            guard let effectObj = videoEffectObject else { return }
            effectObj.setVideoEffectBoolParam(option: "face_shape_beauty_option", key: "enable", boolValue: newValue)
        }
    }
    
    var beautyShapeStyle: String? {
        get {
            return nil
        }
        set {
            if let value = newValue {
                self.updateMaterialConfig(node: AgoraVideoEffectNodeId.beauty.rawValue, selection: value)
            } else {
                self.removeEffect(node: AgoraVideoEffectNodeId.beauty.rawValue)
            }
        }
    }
    
    // Face shape style intensity
    var beautyShapeStrength: Int32 {
        get {
            return videoEffectObject?.getVideoEffectIntParam(option: "face_shape_beauty_option", key: "intensity") ?? 50
        }
        set {
            guard let effectObj = videoEffectObject else { return }
            effectObj.setVideoEffectIntParam(option: "face_shape_beauty_option", key: "intensity", intValue: newValue)
        }
    }
    
    // Makeup switch
    var makeUpEnable: Bool {
        get {
            return videoEffectObject?.getVideoEffectBoolParam(option: "makeup_options", key: "enable_mu") ?? false
        }
        set {
            guard let effectObj = videoEffectObject else { return }
            effectObj.setVideoEffectBoolParam(option: "makeup_options", key: "enable_mu", boolValue: newValue)
        }
    }
    
    // MARK: - Makeup Properties
    
    // Makeup
    var beautyMakeupStyle: String? {
        get {
            return nil
        }
        set {
            guard videoEffectObject != nil else { return }
            if let value = newValue {
                self.updateMaterialConfig(node: AgoraVideoEffectNodeId.styleMakeup.rawValue, selection: value)
            } else {
                self.removeEffect(node: AgoraVideoEffectNodeId.styleMakeup.rawValue)
            }
        }
    }
    
    // Makeup style intensity
    var beautyMakeupStrength: Float {
        get {
            return videoEffectObject?.getVideoEffectFloatParam(option: "style_makeup_option", key: "styleIntensity") ?? 0.95
        }
        set {
            guard let effectObj = videoEffectObject else { return }
            effectObj.setVideoEffectFloatParam(option: "style_makeup_option", key: "styleIntensity", floatValue: newValue)
        }
    }
    
    // Facial style
    var facialStyle: Int32 {
        get {
            return videoEffectObject?.getVideoEffectIntParam(option: "makeup_options", key: "facialStyle") ?? 1
        }
        set {
            guard let effectObj = videoEffectObject else { return }
            effectObj.setVideoEffectIntParam(option: "makeup_options", key: "facialStyle", intValue: newValue)
        }
    }
    
    // Facial intensity
    var facialStrength: Float {
        get {
            return videoEffectObject?.getVideoEffectFloatParam(option: "makeup_options", key: "facialStrength") ?? 1.0
        }
        set {
            guard let effectObj = videoEffectObject else { return }
            effectObj.setVideoEffectFloatParam(option: "makeup_options", key: "facialStrength", floatValue: newValue)
        }
    }
    
    // Wocan style
    var wocanStyle: Int32 {
        get {
            return videoEffectObject?.getVideoEffectIntParam(option: "makeup_options", key: "wocanStyle") ?? 1
        }
        set {
            guard let effectObj = videoEffectObject else { return }
            effectObj.setVideoEffectIntParam(option: "makeup_options", key: "wocanStyle", intValue: newValue)
        }
    }
    
    // Wocan intensity
    var wocanStrength: Float {
        get {
            return videoEffectObject?.getVideoEffectFloatParam(option: "makeup_options", key: "wocanStrength") ?? 1.0
        }
        set {
            guard let effectObj = videoEffectObject else { return }
            effectObj.setVideoEffectFloatParam(option: "makeup_options", key: "wocanStrength", floatValue: newValue)
        }
    }
    
    // MARK: - Eyebrow Properties
    
    // Eyebrow style
    var browStyle: Int32 {
        get {
            return videoEffectObject?.getVideoEffectIntParam(option: "makeup_options", key: "browStyle") ?? 1
        }
        set {
            guard let effectObj = videoEffectObject else { return }
            effectObj.setVideoEffectIntParam(option: "makeup_options", key: "browStyle", intValue: newValue)
        }
    }
    
    // Eyebrow color
    var browColor: Int32 {
        get {
            return videoEffectObject?.getVideoEffectIntParam(option: "makeup_options", key: "browColor") ?? 1
        }
        set {
            guard let effectObj = videoEffectObject else { return }
            effectObj.setVideoEffectIntParam(option: "makeup_options", key: "browColor", intValue: newValue)
        }
    }
    
    // Eyebrow intensity
    var browStrength: Float {
        get {
            return videoEffectObject?.getVideoEffectFloatParam(option: "makeup_options", key: "browStrength") ?? 1.0
        }
        set {
            guard let effectObj = videoEffectObject else { return }
            effectObj.setVideoEffectFloatParam(option: "makeup_options", key: "browStrength", floatValue: newValue)
        }
    }
    
    // MARK: - Eyelash Properties
    
    // Eyelash style
    var lashStyle: Int32 {
        get {
            return videoEffectObject?.getVideoEffectIntParam(option: "makeup_options", key: "lashStyle") ?? 1
        }
        set {
            guard let effectObj = videoEffectObject else { return }
            effectObj.setVideoEffectIntParam(option: "makeup_options", key: "lashStyle", intValue: newValue)
        }
    }
    
    // Eyelash color
    var lashColor: Int32 {
        get {
            return videoEffectObject?.getVideoEffectIntParam(option: "makeup_options", key: "lashColor") ?? 1
        }
        set {
            guard let effectObj = videoEffectObject else { return }
            effectObj.setVideoEffectIntParam(option: "makeup_options", key: "lashColor", intValue: newValue)
        }
    }
    
    // Eyelash intensity
    var lashStrength: Float {
        get {
            return videoEffectObject?.getVideoEffectFloatParam(option: "makeup_options", key: "lashStrength") ?? 1.0
        }
        set {
            guard let effectObj = videoEffectObject else { return }
            effectObj.setVideoEffectFloatParam(option: "makeup_options", key: "lashStrength", floatValue: newValue)
        }
    }
    
    // MARK: - Eyeshadow Properties
    
    // Eyeshadow style
    var shadowStyle: Int32 {
        get {
            return videoEffectObject?.getVideoEffectIntParam(option: "makeup_options", key: "shadowStyle") ?? 1
        }
        set {
            guard let effectObj = videoEffectObject else { return }
            effectObj.setVideoEffectIntParam(option: "makeup_options", key: "shadowStyle", intValue: newValue)
        }
    }
    
    // Eyeshadow intensity
    var shadowStrength: Float {
        get {
            return videoEffectObject?.getVideoEffectFloatParam(option: "makeup_options", key: "shadowStrength") ?? 1.0
        }
        set {
            guard let effectObj = videoEffectObject else { return }
            effectObj.setVideoEffectFloatParam(option: "makeup_options", key: "shadowStrength", floatValue: newValue)
        }
    }
    
    // MARK: - Pupil Properties
    
    // Pupil style
    var pupilStyle: Int32 {
        get {
            return videoEffectObject?.getVideoEffectIntParam(option: "makeup_options", key: "pupilStyle") ?? 1
        }
        set {
            guard let effectObj = videoEffectObject else { return }
            effectObj.setVideoEffectIntParam(option: "makeup_options", key: "pupilStyle", intValue: newValue)
        }
    }
    
    // Pupil intensity
    var pupilStrength: Float {
        get {
            return videoEffectObject?.getVideoEffectFloatParam(option: "makeup_options", key: "pupilStrength") ?? 1.0
        }
        set {
            guard let effectObj = videoEffectObject else { return }
            effectObj.setVideoEffectFloatParam(option: "makeup_options", key: "pupilStrength", floatValue: newValue)
        }
    }
    
    // MARK: - Blush Properties
    
    // Blush style
    var blushStyle: Int32 {
        get {
            return videoEffectObject?.getVideoEffectIntParam(option: "makeup_options", key: "blushStyle") ?? 1
        }
        set {
            guard let effectObj = videoEffectObject else { return }
            effectObj.setVideoEffectIntParam(option: "makeup_options", key: "blushStyle", intValue: newValue)
        }
    }
    
    // Blush color
    var blushColor: Int32 {
        get {
            return videoEffectObject?.getVideoEffectIntParam(option: "makeup_options", key: "blushColor") ?? 5
        }
        set {
            guard let effectObj = videoEffectObject else { return }
            effectObj.setVideoEffectIntParam(option: "makeup_options", key: "blushColor", intValue: newValue)
        }
    }
    
    // Blush intensity
    var blushStrength: Float {
        get {
            return videoEffectObject?.getVideoEffectFloatParam(option: "makeup_options", key: "blushStrength") ?? 1.0
        }
        set {
            guard let effectObj = videoEffectObject else { return }
            effectObj.setVideoEffectFloatParam(option: "makeup_options", key: "blushStrength", floatValue: newValue)
        }
    }
    
    // MARK: - Lipstick Properties
    
    // Lipstick style
    var lipStyle: Int32 {
        get {
            return videoEffectObject?.getVideoEffectIntParam(option: "makeup_options", key: "lipStyle") ?? 2
        }
        set {
            guard let effectObj = videoEffectObject else { return }
            effectObj.setVideoEffectIntParam(option: "makeup_options", key: "lipStyle", intValue: newValue)
        }
    }
    
    // Lipstick color
    var lipColor: Int32 {
        get {
            return videoEffectObject?.getVideoEffectIntParam(option: "makeup_options", key: "lipColor") ?? 5
        }
        set {
            guard let effectObj = videoEffectObject else { return }
            effectObj.setVideoEffectIntParam(option: "makeup_options", key: "lipColor", intValue: newValue)
        }
    }
    
    // Lipstick intensity
    var lipStrength: Float {
        get {
            return videoEffectObject?.getVideoEffectFloatParam(option: "makeup_options", key: "lipStrength") ?? 1.0
        }
        set {
            guard let effectObj = videoEffectObject else { return }
            effectObj.setVideoEffectFloatParam(option: "makeup_options", key: "lipStrength", floatValue: newValue)
        }
    }
    
    // MARK: - Filter Properties
    
    // Filter switch
    var filterEnable: Bool {
        get {
            return videoEffectObject?.getVideoEffectBoolParam(option: "filter_effect_option", key: "enable") ?? false
        }
        set {
            guard let effectObj = videoEffectObject else { return }
            effectObj.setVideoEffectBoolParam(option: "filter_effect_option", key: "enable", boolValue: newValue)
        }
    }
    
    // Filter
    var beautyFilter: String? {
        get {
            return nil 
        }
        set {
            guard videoEffectObject != nil else { return }
            if let value = newValue {
                self.updateMaterialConfig(node: AgoraVideoEffectNodeId.filter.rawValue, selection: value)
            } else {
                self.removeEffect(node: AgoraVideoEffectNodeId.filter.rawValue)
            }
        }
    }
    
    // Filter intensity
    var filterStrength: Float {
        get {
            return videoEffectObject?.getVideoEffectFloatParam(option: "filter_effect_option", key: "strength") ?? 0.5
        }
        set {
            guard let effectObj = videoEffectObject else { return }
            effectObj.setVideoEffectFloatParam(option: "filter_effect_option", key: "strength", floatValue: newValue)
        }
    }
    
    let beautyShapeList = [
        ["name": "Face Shape Enable".localized, "key": "enable", "type": "switch"],
        ["name": "Face Shape Gender".localized,
         "key": "gender",
         "type": "segment",
         "value": ["Face Shape Gender Female".localized, "Face Shape Gender Male".localized]],
        ["name": "Face Shape Intensity".localized, "key": "intensity", "type": "slider", "value": [0, 100]],
        
        ["name": "Face Shape Area Head Scale".localized, "key": "headscale", "type": "slider", "value": [0, 100]],
        ["name": "Face Shape Area Forehead".localized, "key": "forehead", "type": "slider", "value": [-100, 100]],
        ["name": "Face Shape Area Face Contour".localized, "key": "facecontour", "type": "slider", "value": [0, 100]],
        ["name": "Face Shape Area Face Length".localized, "key": "facelength", "type": "slider", "value": [-100, 100]],
        ["name": "Face Shape Area Face Width".localized, "key": "facewidth", "type": "slider", "value": [0, 100]],
        ["name": "Face Shape Area Cheek Bone".localized, "key": "cheekbone", "type": "slider", "value": [0, 100]],
        ["name": "Face Shape Area Cheek".localized, "key": "cheek", "type": "slider", "value": [0, 100]],
        ["name": "Face Shape Area Chin".localized, "key": "chin", "type": "slider", "value": [-100, 100]],
        ["name": "Face Shape Area Eye Scale".localized, "key": "eyescale", "type": "slider", "value": [0, 100]],
        ["name": "Face Shape Area Nose Length".localized, "key": "noselength", "type": "slider", "value": [-100, 100]],
        ["name": "Face Shape Area Nose Width".localized, "key": "nosewidth", "type": "slider", "value": [-100, 100]],
        ["name": "Face Shape Area Mouth Scale".localized, "key": "mouthscale", "type": "slider", "value": [-100, 100]]
    ]
    
    let makeupList = [
        [
            "name": "Makeup Enable".localized,
            "key": "enable_mu",
            "type": "switch"
        ], [
            "name": "Eyebrow Style".localized,
            "key": "browStyle",
            "type": "segment",
            "title": ["Eyebrow Style Off".localized, "Eyebrow Style Type 1".localized, "Eyebrow Style Type 2".localized],
            "value": [0,
                      1,
                      2,
                      3]
        ],[
            "name": "Eyebrow Color".localized,
            "key": "browColor",
            "type": "segment",
            "title": [
                "Eyebrow Color None".localized, "Eyebrow Color Black".localized, "Eyebrow Color Brown".localized
            ],
            "value": [0,
                     1,
                     2]
        ],  [
            "name": "Eyebrow Strength".localized,
            "key": "browStrength",
            "type": "slider",
            "value": [0, 1]
        ], [
            "name": "Eyelash Style".localized,
            "key": "lashStyle",
            "type": "segment",
            "title": ["Eyebrow Style Off".localized,
                      "Eyebrow Style Type 1".localized,
                      "Eyebrow Style Type 2".localized],
            "value": [0,
                      3,
                      5
                     ]
        ], [
            "name": "Eyelash Color".localized,
            "key": "lashColor",
            "type": "segment",
            "title": [
                "Eyebrow Color None".localized, "Eyebrow Color Black".localized, "Eyebrow Color Brown".localized
            ],
            "value": [0,
                     1,
                     2]
        ], [
            "name": "Eyelash Strength".localized,
            "key": "lashStrength",
            "type": "slider",
            "value": [0, 1]
        ], [
            "name": "Eyeshadow Style".localized,
            "key": "shadowStyle",
            "type": "segment",
            "title": [
                "Eyebrow Style Off".localized,
                          "Eyebrow Style Type 1".localized,
                          "Eyebrow Style Type 2".localized
            ],
            "value": [0,
                     1,
                     6]
        ], [
            "name": "Eyeshadow Strength".localized,
            "key": "shadowStrength",
            "type": "slider",
            "value": [0, 1]
        ], [
            "name": "Blush Style".localized,
            "key": "blushStyle",
            "type": "segment",
            "title": ["Eyebrow Style Off".localized,
                      "Eyebrow Style Type 1".localized,
                      "Eyebrow Style Type 2".localized,
                      "Eyebrow Style Type 3".localized,],
            "value": [
                0,
                1,
                2,
                4,
                9
            ]
        ], [
            "name": "Blush Color".localized,
            "key": "blushColor",
            "type": "segment",
            "title": [
                "Blush Color None".localized,
                "Blush Color Shade 1".localized,
                "Blush Color Shade 2".localized,
                "Blush Color Shade 3".localized,
                "Blush Color Shade 4".localized,
                "Blush Color Shade 5".localized
            ],
            "value": [
                0,
                1,
                2,
                3,
                4,
                5
            ]
        ], [
            "name": "Blush Strength".localized,
            "key": "blushStrength",
            "type": "slider",
            "value": [0, 1]
        ], [
            "name": "Lip Style".localized,
            "key": "lipStyle",
            "type": "segment",
            "title": [
                "Eyebrow Style Off".localized,
                "Eyebrow Style Type 1".localized,
                "Eyebrow Style Type 2".localized,
                "Eyebrow Style Type 3".localized,
                "Eyebrow Style Type 4".localized,
            ],
            "value": [0,
                     1,
                     2,
                     3,
                     6]
        ], [
            "name": "Lip Color".localized,
            "key": "lipColor",
            "type": "segment",
            "title": [
                "Blush Color None".localized,
                "Blush Color Shade 1".localized,
                "Blush Color Shade 2".localized,
                "Blush Color Shade 3".localized,
                "Blush Color Shade 4".localized,
                "Blush Color Shade 5".localized
            ],
            "value": [
                0,
                1,
                2,
                3,
                4,
                5
            ]
        ], [
            "name": "Lip Strength".localized,
            "key": "lipStrength",
            "type": "slider",
            "value": [0, 1]
        ],
        [
            "name": "Wocan".localized,
            "key": "wocanStyle",
            "type": "segment",
            "title": [
                "Eyebrow Style Off".localized,
                "Eyebrow Style Type 1".localized,
                "Eyebrow Style Type 2".localized,
                "Eyebrow Style Type 3".localized,
                "Eyebrow Style Type 4".localized,
            ],
            "value": [
                0,
                1,
                2,
                3,
                4
            ]
        ],
        [
            "name": "Wocan Strength".localized,
            "key": "wocanStrength",
            "type": "slider",
            "value": [0, 1]
        ],
    ]
}
