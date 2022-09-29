//
//  UITypeAlias.swift
//  APIExample
//
//  Created by CavanSu on 2020/5/26.
//  Copyright © 2020 Agora Corp. All rights reserved.
//

#if os(iOS)
import UIKit
import WebKit
#else
import Cocoa
#endif

typealias Color = UIColor

typealias MainFont = Font.HelveticaNeue

extension String {
    var localized: String { NSLocalizedString(self, comment: "") }
}

enum Font {
    enum HelveticaNeue: String {
        case ultraLightItalic = "UltraLightItalic"
        case medium = "Medium"
        case mediumItalic = "MediumItalic"
        case ultraLight = "UltraLight"
        case italic = "Italic"
        case light = "Light"
        case thinItalic = "ThinItalic"
        case lightItalic = "LightItalic"
        case bold = "Bold"
        case thin = "Thin"
        case condensedBlack = "CondensedBlack"
        case condensedBold = "CondensedBold"
        case boldItalic = "BoldItalic"
        
        func with(size: CGFloat) -> UIFont {
            return UIFont(name: "HelveticaNeue-\(rawValue)", size: size)!
        }
    }
}

extension UIColor {
    
    /// Get color rgba components in order.
    func rgba() -> (r: CGFloat, g: CGFloat, b: CGFloat, a: CGFloat) {
        let components = self.cgColor.components
        let numberOfComponents = self.cgColor.numberOfComponents
        
        switch numberOfComponents {
        case 4:
            return (components![0], components![1], components![2], components![3])
        case 2:
            return (components![0], components![0], components![0], components![1])
        default:
            // FIXME: Fallback to black
            return (0, 0, 0, 1)
        }
    }
    
    /// Check the black or white contrast on given color.
    func blackOrWhiteContrastingColor() -> Color {
        let rgbaT = rgba()
        let value = 1 - ((0.299 * rgbaT.r) + (0.587 * rgbaT.g) + (0.114 * rgbaT.b));
        return value < 0.5 ? Color.black : Color.white
    }
    
}

enum AssetsColor : String {
  case videoBackground
  case videoPlaceholder
  case textShadow
}

extension UIColor {
  static func appColor(_ name: AssetsColor) -> UIColor? {
     return UIColor(named: name.rawValue)
  }
}

extension UIView {

    /// Adds constraints to this `UIView` instances `superview` object to make sure this always has the same size as the superview.
    /// Please note that this has no effect if its `superview` is `nil` – add this `UIView` instance as a subview before calling this.
    func bindFrameToSuperviewBounds() {
        guard let superview = self.superview else {
            print("Error! `superview` was nil – call `addSubview(view: UIView)` before calling `bindFrameToSuperviewBounds()` to fix this.")
            return
        }

        self.translatesAutoresizingMaskIntoConstraints = false
        self.topAnchor.constraint(equalTo: superview.topAnchor, constant: 0).isActive = true
        self.bottomAnchor.constraint(equalTo: superview.bottomAnchor, constant: 0).isActive = true
        self.leadingAnchor.constraint(equalTo: superview.leadingAnchor, constant: 0).isActive = true
        self.trailingAnchor.constraint(equalTo: superview.trailingAnchor, constant: 0).isActive = true

    }
}

//MARK: - Color
#if os(iOS)
typealias AGColor = UIColor
#else
typealias AGColor = NSColor
#endif
extension AGColor {
    convenience init(hex: Int, alpha: CGFloat = 1) {
        func transform(_ input: Int, offset: Int = 0) -> CGFloat {
            let value = (input >> offset) & 0xff
            return CGFloat(value) / 255
        }
        
        self.init(red: transform(hex, offset: 16),
                  green: transform(hex, offset: 8),
                  blue: transform(hex),
                  alpha: alpha)
    }
    
    func rgbValue() -> (red: CGFloat, green: CGFloat, blue: CGFloat) {
        var red: CGFloat = 0
        var green: CGFloat = 0
        var blue: CGFloat = 0
        
        getRed(&red, green: &green, blue: &blue, alpha: nil)
        
        return (red * 255, green * 255, blue * 255)
    }
    
    convenience init(hex: String, alpha: CGFloat = 1) {
        var cString: String = hex.trimmingCharacters(in: .whitespacesAndNewlines).uppercased()
        
        if (cString.hasPrefix("#")) {
            let range = cString.index(after: cString.startIndex) ..< cString.endIndex
            cString = String(cString[range])
        }
        if (cString.hasPrefix("0X")) {
            let range = cString.index(cString.startIndex, offsetBy: 2) ..< cString.endIndex
            cString = String(cString[range])
        }
        
        
        if (cString.count != 6) {
            self.init()
            return
        }
        
        let scanner = Scanner(string: cString)
        var hexValue: UInt64 = 0
        scanner.scanHexInt64(&hexValue)
        self.init(hex: Int(hexValue), alpha: alpha)
    }
    
    static func randomColor() -> AGColor {
        let randomHex = Int(arc4random_uniform(0xCCCCCC) + 0x555555)
        return AGColor(hex: randomHex)
    }
}

//MARK: - Font
#if os(iOS)
typealias AGFont = UIFont
#else
typealias AGFont = NSFont
#endif

//MARK: - Image
#if os(iOS)
typealias AGImage = UIImage
#else
typealias AGImage = NSImage
#endif

// MARK: - Label
#if os(iOS)
typealias AGLabel = UILabel
#else
typealias AGLabel = NSTextField
#endif
extension AGLabel {
    var formattedFloatValue: Float {
        get {
            #if os(iOS)
            if let text = text, let value = Double(text) {
                return Float(value)
            } else {
                return 0
            }
            #else
            return floatValue
            #endif
        }
        set {
            #if os(iOS)
            text = NSString(format: "%.1f", newValue) as String
            #else
            stringValue = NSString(format: "%.1f", newValue) as String
            #endif
        }
    }
    
    var formattedCGFloatValue: CGFloat {
        get {
            #if os(iOS)
            if let text = text, let value = Double(text) {
                return CGFloat(value)
            } else {
                return 0
            }
            #else
            return CGFloat(floatValue)
            #endif
        }
        set {
            #if os(iOS)
            text = NSString(format: "%.1f", newValue) as String
            #else
            stringValue = NSString(format: "%.1f", newValue) as String
            #endif
        }
    }
    
    var formattedIntValue: Int {
        get {
            #if os(iOS)
            if let text = text, let value = Int(text) {
                return value
            } else {
                return 0
            }
            #else
            return integerValue
            #endif
        }
        set {
            #if os(iOS)
            text = "\(newValue)"
            #else
            stringValue = "\(newValue)"
            #endif
        }
    }
    
    #if os(macOS)
    var text: String? {
        get {
            return stringValue
        }
        set {
            if let newValue = newValue {
                stringValue = newValue
            }
        }
    }
    #endif
}

//MARK: - TextField
#if os(iOS)
typealias AGTextField = UITextField
#else
typealias AGTextField = NSTextField
#endif

extension AGTextField {
    #if os(iOS)
    var integerValue: Int {
        get {
            if let text = text, let value = Int(text) {
                return value
            } else {
                return 0
            }
        }
        set {
            text = "\(newValue)"
        }
    }
    
    var formattedIntValue: Int {
        get {
            return integerValue
        }
        set {
            integerValue = newValue
        }
    }
    
    var cgFloatValue: CGFloat {
        get {
            if let text = text, let value = Double(text) {
                return CGFloat(value)
            } else {
                return 0
            }
        }
        set {
            text = "\(newValue)"
        }
    }
    
    var formattedCGFloatValue: CGFloat {
        get {
            return CGFloat(cgFloatValue)
        }
        set {
            cgFloatValue = newValue
        }
    }
    
    var formattedFloatValue: Float {
        get {
            if let text = text, let value = Double(text) {
                return Float(value)
            } else {
                return 0
            }
        }
        set {
            text = NSString(format: "%.1f", newValue) as String
        }
    }
    
    var stringValue: String {
        get {
            return text!
        }
        set {
            text = newValue
        }
    }
    #endif
    var placeholderAGString: String? {
        get {
        #if os(iOS)
            return placeholder
        #else
            return placeholderString
        #endif
        }
        set {
        #if os(iOS)
            placeholder = newValue
        #else
            placeholderString = placeholderAGString
        #endif
        }
    }
}

//MARK: - Indicator
#if os(iOS)
typealias AGIndicator = UIActivityIndicatorView
#else
typealias AGIndicator = NSProgressIndicator
#endif

extension AGIndicator {
    
    func startAnimation() {
    #if os(iOS)
        self.startAnimating()
    #else
        self.startAnimation(nil)
    #endif
    }
    
    func stopAnimation() {
    #if os(iOS)
        self.stopAnimating()
    #else
        self.stopAnimation(nil)
    #endif
    }
    
}

//MARK: - View
#if os(iOS)
typealias AGView = UIView
#else
typealias AGView = NSView
#endif
extension AGView {
    var cornerRadius: CGFloat? {
        get {
            #if os(iOS)
            return layer.cornerRadius
            #else
            return layer?.cornerRadius
            #endif
        }
        set {
            guard let newValue = newValue else {
                return
            }
            #if os(iOS)
            layer.cornerRadius = newValue
            #else
            wantsLayer = true
            layer?.cornerRadius = newValue
            #endif
        }
    }
    
    var masksToBounds: Bool? {
        get {
            #if os(iOS)
            return layer.masksToBounds
            #else
            return layer?.masksToBounds
            #endif
        }
        set {
            guard let newValue = newValue else {
                return
            }
            #if os(iOS)
            layer.masksToBounds = newValue
            #else
            wantsLayer = true
            layer?.masksToBounds = newValue
            #endif
        }
    }
    
    var borderWidth: CGFloat {
        get {
            #if os(iOS)
            return layer.borderWidth
            #else
            guard let borderWidth = layer?.borderWidth else {
                return 0
            }
            return borderWidth
            #endif
        }
        set {
            #if os(iOS)
            layer.borderWidth = newValue
            #else
            wantsLayer = true
            layer?.borderWidth = newValue
            #endif
        }
    }
    
    var borderColor: CGColor {
        get {
            #if os(iOS)
            guard let borderColor = layer.borderColor else {
                return AGColor.clear.cgColor
            }
            return borderColor
            #else
            guard let borderColor = layer?.borderColor else {
                return AGColor.clear.cgColor
            }
            return borderColor
            #endif
        }
        set {
            #if os(iOS)
            layer.borderColor = newValue
            #else
            wantsLayer = true
            layer?.borderColor = newValue
            #endif
        }
    }
    
    #if os(macOS)
    var backgroundColor: AGColor? {
        get {
            if let cgColor = layer?.backgroundColor {
                return AGColor(cgColor: cgColor)
            } else {
                return nil
            }
        }
        set {
            if let newValue = newValue {
                wantsLayer = true
                layer?.backgroundColor = newValue.cgColor
            }
        }
    }
    
    var center: CGPoint {
        get {
            return CGPoint(x: self.frame.width / 2, y: self.frame.height / 2)
        }
        set {
            self.frame.origin = CGPoint(x: newValue.x - self.frame.width / 2, y: newValue.y - self.frame.height / 2)
        }
    }
    #endif
}


#if os(iOS)
typealias AGVisualEffectView = UIVisualEffectView
#else
typealias AGVisualEffectView = NSVisualEffectView
#endif

//MARK: - ImageView
#if os(iOS)
typealias AGImageView = UIImageView
#else
typealias AGImageView = NSImageView
#endif

//MARK: - TableView
#if os(iOS)
typealias AGTableView = UITableView
#else
typealias AGTableView = NSTableView
#endif

//MARK: - TableViewCell
#if os(iOS)
typealias AGTableViewCell = UITableViewCell
#else
typealias AGTableViewCell = NSTableCellView
#endif

//MARK: - CollectionView
#if os(iOS)
typealias AGCollectionView = UICollectionView
#else
typealias AGCollectionView = NSCollectionView
#endif

#if os(iOS)
typealias AGCollectionViewFlowLayout = UICollectionViewFlowLayout
#else
typealias AGCollectionViewFlowLayout = NSCollectionViewFlowLayout
#endif

//MARK: - CollectionViewCell
#if os(iOS)
typealias AGCollectionViewCell = UICollectionViewCell
#else
typealias AGCollectionViewCell = NSCollectionViewItem
#endif

extension AGCollectionViewCell {
    #if os(OSX)
    var contentView: AGView {
        get {
            return view
        }
        set {
            view = newValue
        }
    }
    #endif
}

//MARK: - Button
#if os(iOS)
typealias AGButton = UIButton
#else
typealias AGButton = NSButton
#endif
extension AGButton {
    #if os(iOS)
    var image: AGImage? {
        get {
            return image(for: .normal)
        }
        set {
            setImage(newValue, for: .normal)
        }
    }
    var highlightImage: AGImage? {
        get {
            return image(for: .highlighted)
        }
        set {
            setImage(newValue, for: .highlighted)
        }
    }
    var title: String? {
        get {
            return title(for: .normal)
        }
        set {
            setTitle(newValue, for: .normal)
        }
    }
    
    #else
    var textColor: AGColor {
        get {
            return AGColor.black
        }
        set {
            let pstyle = NSMutableParagraphStyle()
            pstyle.alignment = .left
            attributedTitle = NSAttributedString(string: title, attributes: [ NSAttributedString.Key.foregroundColor : newValue, NSAttributedString.Key.paragraphStyle : pstyle ])
        }
    }
    #endif
    
    func switchImage(toImage: AGImage) {
        #if os(iOS)
        UIView.animate(withDuration: 0.15, animations: {
            self.isEnabled = false
            self.alpha = 0.3
        }) { (_) in
            self.image = toImage
            self.alpha = 1.0
            self.isEnabled = true
        }
        #else
        NSAnimationContext.runAnimationGroup({ (context) in
            context.duration = 0.3
            self.isEnabled = false
            self.animator().alphaValue = 0.3
        }) {
            self.image = toImage
            self.alphaValue = 1.0
            self.isEnabled = true
        }
        #endif
    }
}

//MARK: - Switch
#if os(iOS)
typealias AGSwitch = UISwitch
#else
typealias AGSwitch = NSButton
#endif
#if os(macOS)
extension AGSwitch {
    var isOn: Bool {
        get {
            return state != .off
        }
        set {
            state = newValue ? .on : .off
        }
    }
}
#endif

//MARK: - WebView
#if os(iOS)
typealias AGWebView = WKWebView
#else
import WebKit
typealias AGWebView = WebView
#endif

#if os(macOS)
extension AGWebView {
    func loadRequest(_ request: URLRequest) {
        self.mainFrame.load(request)
    }
}
#endif

//MARK: - Slider
#if os(iOS)
typealias AGSlider = UISlider
#else
typealias AGSlider = NSSlider
#endif
extension AGSlider {
    #if os(iOS)
    var floatValue: Float {
        get {
            return value
        }
        set {
            setValue(newValue, animated: false)
        }
    }
    var cgFloatValue: CGFloat {
        get {
            return CGFloat(value)
        }
        set {
            setValue(Float(newValue), animated: false)
        }
    }
    var integerValue: Int {
        get {
            return Int(value)
        }
        set {
            setValue(Float(newValue), animated: false)
        }
    }
    var doubleValue: Double {
        get {
            return Double(value)
        }
        set {
            setValue(Float(newValue), animated: false)
        }
    }
    #else
    var minimumValue: Float {
        get {
            return Float(minValue)
        }
        set {
            minValue = Double(newValue)
        }
    }
    var maximumValue: Float {
        get {
            return Float(maxValue)
        }
        set {
            maxValue = Double(newValue)
        }
    }
    #endif
}

//MARK: - SegmentedControl
#if os(iOS)
typealias AGPopSheetButton = UIButton
#else
typealias AGPopSheetButton = NSPopUpButton
#endif

//MARK: - SegmentedControl
#if os(iOS)
typealias AGSegmentedControl = UISegmentedControl
#else
typealias AGSegmentedControl = NSPopUpButton
#endif
#if os(macOS)
extension AGSegmentedControl {
    var selectedSegmentIndex: Int {
        get {
            return indexOfSelectedItem
        }
        set {
            selectItem(at: newValue)
        }
    }
}
#endif

//MARK: - StoryboardSegue
#if os(iOS)
typealias AGStoryboardSegue = UIStoryboardSegue
#else
typealias AGStoryboardSegue = NSStoryboardSegue
#endif
extension AGStoryboardSegue {
    var identifierString: String? {
        get {
            #if os(iOS)
            return identifier
            #else
            return identifier
            #endif
        }
    }
    
    #if os(iOS)
    var destinationController: AGViewController? {
        get {
            return destination
        }
    }
    #endif
}

//MARK: - Storyboard
#if os(iOS)
typealias AGStoryboard = UIStoryboard
#else
typealias AGStoryboard = NSStoryboard
#endif

//MARK: - ViewController
#if os(iOS)
typealias AGViewController = UIViewController
#else
typealias AGViewController = NSViewController
#endif
extension AGViewController {
    #if os(OSX)
    var title: String? {
        get {
            return self.view.window?.title
        }
        set {
            guard let title = newValue else {
                return
            }
            self.view.window?.title = title
        }
    }
    #endif
    
    func performAGSegue(withIdentifier identifier: String, sender: Any?) {
        #if os(iOS)
        performSegue(withIdentifier: identifier, sender: sender)
        #else
        performSegue(withIdentifier: identifier, sender: sender)
        #endif
    }
    
    func dismissVC(_ vc: AGViewController, animated: Bool) {
        #if os(iOS)
        vc.dismiss(animated: animated, completion: nil)
        #else
        dismiss(nil)
        #endif
    }
}

//MARK: - TableViewController
#if os(iOS)
typealias AGTableViewController = UITableViewController
#else
typealias AGTableViewController = NSViewController
#endif


#if os(iOS)
typealias AGBezierPath = UIBezierPath
#else
typealias AGBezierPath = NSBezierPath
#endif

extension AGBezierPath {
    #if os(OSX)
    func addLine(to point: CGPoint) {
        var points = [point]
        self.appendPoints(&points, count: 1)
    }
    
    func addArc(withCenter center: CGPoint, radius: CGFloat, startAngle: CGFloat, endAngle: CGFloat, clockwise: Bool) {
        self.appendArc(withCenter: center, radius: radius, startAngle: startAngle, endAngle: endAngle, clockwise: clockwise)
    }
    #endif
}

#if os(iOS)
typealias AGControl = UIControl
#else
typealias AGControl = NSControl
#endif


#if os(OSX)
extension String {
    func buttonWhiteAttributedTitleString() -> NSAttributedString {
        return buttonAttributedTitleStringWithColor(AGColor.white)
    }
    
    func buttonBlueAttributedTitleString() -> NSAttributedString {
        return buttonAttributedTitleStringWithColor(AGColor(hex: 0x00a0e9))
    }
    
    fileprivate func buttonAttributedTitleStringWithColor(_ color: AGColor) -> NSAttributedString {
        let attributes = [NSAttributedString.Key.foregroundColor: color, NSAttributedString.Key.font: NSFont.systemFont(ofSize: 13)]
        let attributedString = NSMutableAttributedString(string: self)
        let range = NSMakeRange(0, attributedString.length)
        attributedString.addAttributes(attributes, range: range)
        attributedString.setAlignment(.center, range: range)
        attributedString.fixAttributes(in: range)
        
        return attributedString
    }
}
#endif

#if os(iOS)
typealias AGApplication = UIApplication
#else
typealias AGApplication = NSApplication
#endif

