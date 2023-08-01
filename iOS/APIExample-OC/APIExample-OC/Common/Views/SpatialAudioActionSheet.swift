//
//  SpatialAudioActionSheet.swift
//  APIExample
//
//  Created by zhaoyongqiang on 2023/7/31.
//

import UIKit

@objcMembers
class SpatialAudioActionSheet: UIView {
    var onTapMuteSwitchClosure: ((Bool) -> Void)?
    var onTapVoiceBlurSwitchClosure: ((Bool) -> Void)?
    var onTapAirborneSwitchClosure: ((Bool) -> Void)?
    var onTapAttenuationSilderClosure: ((Float) -> Void)?
    private lazy var muteLabel: UILabel = {
        let label = UILabel()
        label.text = "静音".localized
        return label
    }()
    private lazy var voiceBlurLabel: UILabel = {
        let label = UILabel()
        label.text = "Voice Blur".localized
        return label
    }()
    private lazy var airborneLabel: UILabel = {
        let label = UILabel()
        label.text = "Airborne Simulation".localized
        return label
    }()
    private lazy var attenuationLabel: UILabel = {
        let label = UILabel()
        label.text = "Attenuation".localized
        return label
    }()
    private lazy var muteSwitch: UISwitch = {
        let view = UISwitch()
        view.addTarget(self, action: #selector(onTapMuteSwitch(sender:)), for: .valueChanged)
        return view
    }()
    private lazy var voiceBlurSwitch: UISwitch = {
        let view = UISwitch()
        view.addTarget(self, action: #selector(onTapVoiceBlurSwitch(sender:)), for: .valueChanged)
        return view
    }()
    private lazy var airborneSwitch: UISwitch = {
        let view = UISwitch()
        view.addTarget(self, action: #selector(onTapAirborneSwitch(sender:)), for: .valueChanged)
        return view
    }()
    private lazy var attenuationSilder: UISlider = {
        let view = UISlider()
        view.value = 0.2
        view.addTarget(self, action: #selector(onTapAttenuationSlider(sender:)), for: .valueChanged)
        return view
    }()
    override init(frame: CGRect) {
        super.init(frame: frame)
        setupUI()
    }
    
    required init?(coder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
    
    func show() {
        AlertManager.show(view: self, alertPostion: .bottom, didCoverDismiss: true)
    }
    
    private func setupUI() {
        if #available(iOS 13.0, *) {
            backgroundColor = .tertiarySystemBackground
        } else {
            backgroundColor = .white
        }
        addSubview(muteLabel)
        addSubview(voiceBlurLabel)
        addSubview(airborneLabel)
        addSubview(attenuationLabel)
        addSubview(muteSwitch)
        addSubview(voiceBlurSwitch)
        addSubview(airborneSwitch)
        addSubview(attenuationSilder)
        muteLabel.translatesAutoresizingMaskIntoConstraints = false
        voiceBlurLabel.translatesAutoresizingMaskIntoConstraints = false
        airborneLabel.translatesAutoresizingMaskIntoConstraints = false
        attenuationLabel.translatesAutoresizingMaskIntoConstraints = false
        muteSwitch.translatesAutoresizingMaskIntoConstraints = false
        voiceBlurSwitch.translatesAutoresizingMaskIntoConstraints = false
        airborneSwitch.translatesAutoresizingMaskIntoConstraints = false
        attenuationSilder.translatesAutoresizingMaskIntoConstraints = false
        
        widthAnchor.constraint(equalToConstant: UIScreen.main.bounds.width).isActive = true
        
        muteLabel.leadingAnchor.constraint(equalTo: leadingAnchor, constant: 15).isActive = true
        muteLabel.topAnchor.constraint(equalTo: topAnchor, constant: 20).isActive = true
        voiceBlurLabel.leadingAnchor.constraint(equalTo: muteLabel.leadingAnchor).isActive = true
        voiceBlurLabel.topAnchor.constraint(equalTo: muteLabel.bottomAnchor, constant: 15).isActive = true
        airborneLabel.leadingAnchor.constraint(equalTo: voiceBlurLabel.leadingAnchor).isActive = true
        airborneLabel.topAnchor.constraint(equalTo: voiceBlurLabel.bottomAnchor, constant: 15).isActive = true
        attenuationLabel.leadingAnchor.constraint(equalTo: airborneLabel.leadingAnchor).isActive = true
        attenuationLabel.topAnchor.constraint(equalTo: airborneLabel.bottomAnchor, constant: 15).isActive = true
        attenuationLabel.bottomAnchor.constraint(equalTo: bottomAnchor, constant: -25).isActive = true
        muteSwitch.trailingAnchor.constraint(equalTo: trailingAnchor, constant: -15).isActive = true
        muteSwitch.centerYAnchor.constraint(equalTo: muteLabel.centerYAnchor).isActive = true
        voiceBlurSwitch.trailingAnchor.constraint(equalTo: trailingAnchor, constant: -15).isActive = true
        voiceBlurSwitch.centerYAnchor.constraint(equalTo: voiceBlurLabel.centerYAnchor).isActive = true
        airborneSwitch.trailingAnchor.constraint(equalTo: trailingAnchor, constant: -15).isActive = true
        airborneSwitch.centerYAnchor.constraint(equalTo: airborneLabel.centerYAnchor).isActive = true
        attenuationSilder.trailingAnchor.constraint(equalTo: trailingAnchor, constant: -15).isActive = true
        attenuationSilder.centerYAnchor.constraint(equalTo: attenuationLabel.centerYAnchor).isActive = true
        attenuationSilder.leadingAnchor.constraint(equalTo: attenuationLabel.trailingAnchor, constant: 15).isActive = true
    }
    
    
    @objc
    private func onTapMuteSwitch(sender: UISwitch) {
        onTapMuteSwitchClosure?(sender.isOn)
    }
    @objc
    private func onTapVoiceBlurSwitch(sender: UISwitch) {
        onTapVoiceBlurSwitchClosure?(sender.isOn)
    }
    @objc
    private func onTapAirborneSwitch(sender: UISwitch) {
        onTapAirborneSwitchClosure?(sender.isOn)
    }
    @objc
    private func onTapAttenuationSlider(sender: UISlider) {
        onTapAttenuationSilderClosure?(sender.value)
    }
}
