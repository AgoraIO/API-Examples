//
//  VideoView.swift
//  OpenVideoCall
//
//  Created by GongYuhua on 2/14/16.
//  Copyright © 2016 Agora. All rights reserved.
//

import UIKit

class VideoView: AGView {
    
    fileprivate(set) var videoView: AGView!
    
    fileprivate var infoView: AGView!
    fileprivate var infoLabel: AGLabel!
    fileprivate var placeholder: AGLabel!
    
    var isVideoMuted = false {
        didSet {
            videoView?.isHidden = isVideoMuted
        }
    }
    
    override init(frame frameRect: CGRect) {
        super.init(frame: frameRect)
        translatesAutoresizingMaskIntoConstraints = false
        backgroundColor = UIColor.appColor(.videoBackground)
        
        addPlaceholder()
        addVideoView()
        addInfoView()
    }
    
    required init?(coder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
}

extension VideoView {
    func update(with info: StatisticsInfo) {
        infoLabel?.text = info.description()
    }
    
    func setPlaceholder(text:String) {
        placeholder.text = text
    }
}

private extension VideoView {
    func addVideoView() {
        videoView = AGView()
        videoView.translatesAutoresizingMaskIntoConstraints = false
        videoView.backgroundColor = AGColor.clear
        addSubview(videoView)
        
        let videoViewH = NSLayoutConstraint.constraints(withVisualFormat: "H:|[video]|", options: [], metrics: nil, views: ["video": videoView!])
        let videoViewV = NSLayoutConstraint.constraints(withVisualFormat: "V:|[video]|", options: [], metrics: nil, views: ["video": videoView!])
        NSLayoutConstraint.activate(videoViewH + videoViewV)
    }
    
    func addPlaceholder() {
        placeholder = AGLabel()
        placeholder.textAlignment = .center
        placeholder.font = UIFont.systemFont(ofSize: 14)
        placeholder.textColor = UIColor.appColor(.videoPlaceholder)
        placeholder.translatesAutoresizingMaskIntoConstraints = false
        placeholder.backgroundColor = AGColor.clear
        placeholder.numberOfLines = 0
        
        addSubview(placeholder)
        let labelH = NSLayoutConstraint.constraints(withVisualFormat: "H:|-[info]-|", options: [], metrics: nil, views: ["info": placeholder!])
        let labelV = NSLayoutConstraint.constraints(withVisualFormat: "V:|-[info]-|", options: [], metrics: nil, views: ["info": placeholder!])
        NSLayoutConstraint.activate(labelH + labelV)
    }
    
    func addInfoView() {
        infoView = AGView()
        infoView.translatesAutoresizingMaskIntoConstraints = false
        infoView.backgroundColor = AGColor.clear
        
        addSubview(infoView)
        let infoViewH = NSLayoutConstraint.constraints(withVisualFormat: "H:|[info]|", options: [], metrics: nil, views: ["info": infoView!])
        let infoViewV = NSLayoutConstraint.constraints(withVisualFormat: "V:[info(==140)]|", options: [], metrics: nil, views: ["info": infoView!])
        NSLayoutConstraint.activate(infoViewH + infoViewV)
        
        func createInfoLabel() -> AGLabel {
            let label = AGLabel()
            label.translatesAutoresizingMaskIntoConstraints = false
            
            label.text = " "
            #if os(iOS)
            label.shadowOffset = CGSize(width: 0, height: 1)
            label.shadowColor = AGColor.black
            label.numberOfLines = 0
            #endif
            
            label.font = AGFont.systemFont(ofSize: 12)
            label.textColor = AGColor.white
            
            return label
        }
        
        infoLabel = createInfoLabel()
        infoView.addSubview(infoLabel)
        
        let top: CGFloat = 20
        let left: CGFloat = 10
        
        let labelV = NSLayoutConstraint.constraints(withVisualFormat: "V:|-(\(top))-[info]", options: [], metrics: nil, views: ["info": infoLabel!])
        let labelH = NSLayoutConstraint.constraints(withVisualFormat: "H:|-(\(left))-[info]", options: [], metrics: nil, views: ["info": infoLabel!])
        NSLayoutConstraint.activate(labelV)
        NSLayoutConstraint.activate(labelH)
    }
}
