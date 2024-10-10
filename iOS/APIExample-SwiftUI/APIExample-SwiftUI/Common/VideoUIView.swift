//
//  VideoUIView.swift
//  APIExample-SwiftUI
//
//  Created by zhaoyongqiang on 2024/3/19.
//

import UIKit
import SwiftUI

enum StreamType {
    case local
    case remote
    
    func isLocal() -> Bool {
        switch self {
        case .local:  return true
        case .remote: return false
        }
    }
    
    var value: String {
        switch self {
        case .local: "Local Host".localized
        case .remote: "Remote Host".localized
        }
    }
}

class VideoUIView: UIView, Identifiable {
    let id = UUID()
    lazy var videoView: UIView = {
        let view = UIView()
        view.translatesAutoresizingMaskIntoConstraints = false
        return view
    }()
    private lazy var placeHolderLabel: UILabel = {
        let label = UILabel()
        label.text = ""
        label.textColor = .black
        label.font = .systemFont(ofSize: 14)
        label.translatesAutoresizingMaskIntoConstraints = false
        label.numberOfLines = 0
        return label
    }()
    private lazy var statsLabel: UILabel = {
        let label = UILabel()
        label.text = ""
        label.textColor = .black
        label.font = .systemFont(ofSize: 10)
        label.numberOfLines = 0
        label.preferredMaxLayoutWidth = UIScreen.main.bounds.width * 0.5 * 0.7
        label.translatesAutoresizingMaskIntoConstraints = false
        return label
    }()
    
    override init(frame: CGRect) {
        super.init(frame: frame)
        setupUI()
    }
    
    required init?(coder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
    
    private func setupUI() {
        addSubview(placeHolderLabel)
        placeHolderLabel.centerXAnchor.constraint(equalTo: centerXAnchor).isActive = true
        placeHolderLabel.centerYAnchor.constraint(equalTo: centerYAnchor).isActive = true
        
        addSubview(videoView)
        videoView.leadingAnchor.constraint(equalTo: leadingAnchor).isActive = true
        videoView.topAnchor.constraint(equalTo: topAnchor).isActive = true
        videoView.trailingAnchor.constraint(equalTo: trailingAnchor).isActive = true
        videoView.bottomAnchor.constraint(equalTo: bottomAnchor).isActive = true
        
        addSubview(statsLabel)
        statsLabel.leadingAnchor.constraint(equalTo: leadingAnchor, constant: 5).isActive = true
        statsLabel.bottomAnchor.constraint(equalTo: bottomAnchor, constant: -5).isActive = true
    }
    
    var audioOnly: Bool = false
    var uid: UInt = 0 {
        didSet {
            switch type {
            case .local:
                statsInfo?.uid = uid
            case .remote:
                statsInfo?.remoteUid = uid
            }
        }
    }

    var statsInfo: StatisticsInfo? {
        didSet {
            if Thread.isMainThread {
                statsLabel.text = statsInfo?.description(audioOnly: audioOnly) ?? ""
            } else {
                DispatchQueue.main.async {
                    self.statsLabel.text = self.statsInfo?.description(audioOnly: self.audioOnly) ?? ""
                }
            }
        }
    }
    var type: StreamType = .local {
        didSet {
            if type.isLocal() {
                statsInfo = StatisticsInfo(type: .local(StatisticsInfo.LocalInfo()))
            } else {
                statsInfo = StatisticsInfo(type: .remote(StatisticsInfo.RemoteInfo()))
            }
            if audioOnly {
                placeHolderLabel.text = getAudioLabel(isLocal: type.isLocal())
            } else {
                placeHolderLabel.text = type.value
            }
        }
    }
    
    func setPlaceholder(text: String?) {
        placeHolderLabel.text = text
    }
    
    func setInfo(text: String?) {
        statsLabel.text = text
    }
    
    private func getAudioLabel(isLocal: Bool) -> String {
        return "AUDIO ONLY\n\(isLocal ? "Local" : "Remote")"
    }
}


class SampleBufferDisplayView: UIView {
    var videoView: AgoraSampleBufferRender = {
        let view = AgoraSampleBufferRender(frame: .zero)
        view.translatesAutoresizingMaskIntoConstraints = false
        return view
    }()
    private lazy var placeHolderLabel: UILabel = {
        let label = UILabel()
        label.text = ""
        label.textColor = .black
        label.font = .systemFont(ofSize: 14)
        label.translatesAutoresizingMaskIntoConstraints = false
        label.numberOfLines = 0
        return label
    }()
    private lazy var statsLabel: UILabel = {
        let label = UILabel()
        label.text = ""
        label.textColor = .black
        label.font = .systemFont(ofSize: 10)
        label.numberOfLines = 0
        label.preferredMaxLayoutWidth = UIScreen.main.bounds.width * 0.5 * 0.7
        label.translatesAutoresizingMaskIntoConstraints = false
        return label
    }()
    
    override init(frame: CGRect) {
        super.init(frame: frame)
        setupUI()
    }
    
    required init?(coder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
    
    private func setupUI() {
        addSubview(placeHolderLabel)
        placeHolderLabel.centerXAnchor.constraint(equalTo: centerXAnchor).isActive = true
        placeHolderLabel.centerYAnchor.constraint(equalTo: centerYAnchor).isActive = true
        
        addSubview(videoView)
        videoView.leadingAnchor.constraint(equalTo: leadingAnchor).isActive = true
        videoView.topAnchor.constraint(equalTo: topAnchor).isActive = true
        videoView.trailingAnchor.constraint(equalTo: trailingAnchor).isActive = true
        videoView.bottomAnchor.constraint(equalTo: bottomAnchor).isActive = true
        
        addSubview(statsLabel)
        statsLabel.leadingAnchor.constraint(equalTo: leadingAnchor, constant: 5).isActive = true
        statsLabel.bottomAnchor.constraint(equalTo: bottomAnchor, constant: -5).isActive = true
    }
    
    var audioOnly: Bool = false
    var uid: UInt = 0 {
        didSet {
            switch type {
            case .local:
                statsInfo?.uid = uid
            case .remote:
                statsInfo?.remoteUid = uid
            }
        }
    }
    
    var statsInfo: StatisticsInfo? {
        didSet {
            if Thread.isMainThread {
                statsLabel.text = statsInfo?.description(audioOnly: audioOnly) ?? ""
            } else {
                DispatchQueue.main.async {
                    self.statsLabel.text = self.statsInfo?.description(audioOnly: self.audioOnly) ?? ""
                }
            }
        }
    }
    var type: StreamType = .local {
        didSet {
            if type.isLocal() {
                statsInfo = StatisticsInfo(type: .local(StatisticsInfo.LocalInfo()))
            } else {
                statsInfo = StatisticsInfo(type: .remote(StatisticsInfo.RemoteInfo()))
            }
            if audioOnly {
                placeHolderLabel.text = getAudioLabel(isLocal: type.isLocal())
            } else {
                placeHolderLabel.text = type.value
            }
        }
    }
    
    func setPlaceholder(text: String?) {
        placeHolderLabel.text = text
    }
    
    func setInfo(text: String?) {
        statsLabel.text = text
    }
    
    private func getAudioLabel(isLocal: Bool) -> String {
        return "AUDIO ONLY\n\(isLocal ? "Local" : "Remote")"
    }
}


class MetalVideoView: UIView {
    lazy var videoView: AgoraMetalRender = {
        let view = AgoraMetalRender(frame: .zero)
        view.translatesAutoresizingMaskIntoConstraints = false
        return view
    }()
    private lazy var placeHolderLabel: UILabel = {
        let label = UILabel()
        label.text = ""
        label.textColor = .black
        label.font = .systemFont(ofSize: 14)
        label.translatesAutoresizingMaskIntoConstraints = false
        label.numberOfLines = 0
        return label
    }()
    private lazy var statsLabel: UILabel = {
        let label = UILabel()
        label.text = ""
        label.textColor = .black
        label.font = .systemFont(ofSize: 10)
        label.numberOfLines = 0
        label.preferredMaxLayoutWidth = UIScreen.main.bounds.width * 0.5 * 0.7
        label.translatesAutoresizingMaskIntoConstraints = false
        return label
    }()
    
    override init(frame: CGRect) {
        super.init(frame: frame)
        setupUI()
    }
    
    required init?(coder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
    
    private func setupUI() {
        addSubview(placeHolderLabel)
        placeHolderLabel.centerXAnchor.constraint(equalTo: centerXAnchor).isActive = true
        placeHolderLabel.centerYAnchor.constraint(equalTo: centerYAnchor).isActive = true
        
        addSubview(videoView)
        videoView.leadingAnchor.constraint(equalTo: leadingAnchor).isActive = true
        videoView.topAnchor.constraint(equalTo: topAnchor).isActive = true
        videoView.trailingAnchor.constraint(equalTo: trailingAnchor).isActive = true
        videoView.bottomAnchor.constraint(equalTo: bottomAnchor).isActive = true
        
        addSubview(statsLabel)
        statsLabel.leadingAnchor.constraint(equalTo: leadingAnchor, constant: 5).isActive = true
        statsLabel.bottomAnchor.constraint(equalTo: bottomAnchor, constant: -5).isActive = true
    }
    
    var audioOnly: Bool = false
    var uid: UInt = 0 {
        didSet {
            switch type {
            case .local:
                statsInfo?.uid = uid
            case .remote:
                statsInfo?.remoteUid = uid
            }
        }
    }
    
    var statsInfo: StatisticsInfo? {
        didSet {
            if Thread.isMainThread {
                statsLabel.text = statsInfo?.description(audioOnly: audioOnly) ?? ""
            } else {
                DispatchQueue.main.async {
                    self.statsLabel.text = self.statsInfo?.description(audioOnly: self.audioOnly) ?? ""
                }
            }
        }
    }
    var type: StreamType = .local {
        didSet {
            if type.isLocal() {
                statsInfo = StatisticsInfo(type: .local(StatisticsInfo.LocalInfo()))
            } else {
                statsInfo = StatisticsInfo(type: .remote(StatisticsInfo.RemoteInfo()))
            }
            if audioOnly {
                placeHolderLabel.text = getAudioLabel(isLocal: type.isLocal())
            } else {
                placeHolderLabel.text = type.value
            }
        }
    }
    
    func setPlaceholder(text: String?) {
        placeHolderLabel.text = text
    }
    
    func setInfo(text: String?) {
        statsLabel.text = text
    }
    
    private func getAudioLabel(isLocal: Bool) -> String {
        return "AUDIO ONLY\n\(isLocal ? "Local" : "Remote")"
    }
}
