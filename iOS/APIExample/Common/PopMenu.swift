//
//  PopMenu.swift
//  APIExample
//
//  Created by 张乾泽 on 2020/7/24.
//  Copyright © 2020 Agora Corp. All rights reserved.
//

import Foundation
import PopMenu

class PopMenuAGAction: NSObject, PopMenuAction {
    /// Title of action.
    public let title: String?
    
    public var value: AnyObject?
    
    /// Icon of action.
    public let image: UIImage?
    
    /// Image rendering option.
    public var imageRenderingMode: UIImage.RenderingMode = .alwaysTemplate
    
    /// Renderred view of action.
    public let view: UIView
    
    /// Color of action.
    public let color: Color?
    
    /// Handler of action when selected.
    public let didSelect: PopMenuActionHandler?
    
    /// Icon sizing.
    public var iconWidthHeight: CGFloat = 27
    
    // MARK: - Computed Properties
    
    /// Text color of the label.
    public var tintColor: Color {
        get {
            return titleLabel.textColor
        }
        set {
            titleLabel.textColor = newValue
            iconImageView.tintColor = newValue
            backgroundColor = newValue.blackOrWhiteContrastingColor()
        }
    }
    
    /// Font for the label.
    public var font: UIFont {
        get {
            return titleLabel.font
        }
        set {
            titleLabel.font = newValue
        }
    }
    
    /// Rounded corner radius for action view.
    public var cornerRadius: CGFloat {
        get {
            return view.layer.cornerRadius
        }
        set {
            view.layer.cornerRadius = newValue
        }
    }
    
    /// Inidcates if the action is being highlighted.
    public var highlighted: Bool = false {
        didSet {
            guard highlighted != oldValue else { return }
            
            highlightActionView(highlighted)
        }
    }
    
    /// Background color for highlighted state.
    private var backgroundColor: Color = .white

    // MARK: - Subviews
    
    /// Title label view instance.
    private lazy var titleLabel: UILabel = {
        let label = UILabel()
        label.translatesAutoresizingMaskIntoConstraints = false
        label.isUserInteractionEnabled = false
        label.text = title
        
        return label
    }()
    
    /// Icon image view instance.
    private lazy var iconImageView: UIImageView = {
        let imageView = UIImageView()
        imageView.translatesAutoresizingMaskIntoConstraints = false
        imageView.image = image?.withRenderingMode(imageRenderingMode)
        
        return imageView
    }()
    
    // MARK: - Constants
    
    public static let textLeftPadding: CGFloat = 25
    public static let iconLeftPadding: CGFloat = 18
    
    // MARK: - Initializer
    
    /// Initializer.
    public init(title: String? = nil, image: UIImage? = nil, color: Color? = nil, didSelect: PopMenuActionHandler? = nil) {
        self.title = title
        self.image = image
        self.color = color
        self.didSelect = didSelect
        
        view = UIView()
    }
    
    /// Setup necessary views.
    fileprivate func configureViews() {
        var hasImage = false

        if let _ = image {
            hasImage = true
            view.addSubview(iconImageView)
            
            NSLayoutConstraint.activate([
                iconImageView.widthAnchor.constraint(equalToConstant: iconWidthHeight),
                iconImageView.heightAnchor.constraint(equalTo: iconImageView.widthAnchor),
                iconImageView.leadingAnchor.constraint(equalTo: view.leadingAnchor, constant: PopMenuDefaultAction.iconLeftPadding),
                iconImageView.centerYAnchor.constraint(equalTo: view.centerYAnchor)
            ])
        }
        
        view.addSubview(titleLabel)
        
        NSLayoutConstraint.activate([
            titleLabel.leadingAnchor.constraint(equalTo: hasImage ? iconImageView.trailingAnchor : view.leadingAnchor, constant: hasImage ? 8 : PopMenuDefaultAction.textLeftPadding),
            titleLabel.trailingAnchor.constraint(equalTo: view.trailingAnchor, constant: 20),
            titleLabel.centerYAnchor.constraint(equalTo: view.centerYAnchor)
        ])
    }

    /// Load and configure the action view.
    public func renderActionView() {
        view.layer.cornerRadius = 14
        view.layer.masksToBounds = true
        
        configureViews()
    }
    
    /// Highlight the view when panned on top,
    /// unhighlight the view when pan gesture left.
    public func highlightActionView(_ highlight: Bool) {
        DispatchQueue.main.async {
            UIView.animate(withDuration: 0.26, delay: 0, usingSpringWithDamping: 0.6, initialSpringVelocity: 9, options: self.highlighted ? UIView.AnimationOptions.curveEaseIn : UIView.AnimationOptions.curveEaseOut, animations: {
                self.view.transform = self.highlighted ? CGAffineTransform.identity.scaledBy(x: 1.09, y: 1.09) : .identity
                self.view.backgroundColor = self.highlighted ? self.backgroundColor.withAlphaComponent(0.25) : .clear
            }, completion: nil)
        }
    }
    
    /// When the action is selected.
    public func actionSelected(animated: Bool) {
        // Trigger handler.
        didSelect?(self)
        
        // Animate selection
        guard animated else { return }
        
        DispatchQueue.main.async {
            UIView.animate(withDuration: 0.175, animations: {
                self.view.transform = CGAffineTransform.identity.scaledBy(x: 0.915, y: 0.915)
                self.view.backgroundColor = self.backgroundColor.withAlphaComponent(0.18)
            }, completion: { _ in
                UIView.animate(withDuration: 0.175, animations: {
                    self.view.transform = .identity
                    self.view.backgroundColor = .clear
                })
            })
        }
    }
    
    
}
