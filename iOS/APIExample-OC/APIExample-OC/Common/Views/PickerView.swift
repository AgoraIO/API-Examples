//
//  PickerView.swift
//  APIExample
//
//  Created by zhaoyongqiang on 2023/3/21.
//  Copyright © 2023 Agora Corp. All rights reserved.
//

import UIKit

extension String {
    var localized: String { NSLocalizedString(self, comment: "") }
}

@objcMembers
class PickerView: UIView {
    private lazy var cancelButton: UIButton = {
        let button = UIButton()
        button.setTitle("Cancel".localized, for: .normal)
        button.backgroundColor = .blue
        button.layer.cornerRadius = 5
        button.setTitleColor(.white, for: .normal)
        button.titleLabel?.font = .systemFont(ofSize: 14)
        button.addTarget(self, action: #selector(onTapCancelButton), for: .touchUpInside)
        return button
    }()
    private lazy var sureButton: UIButton = {
        let button = UIButton()
        button.setTitle("Sure".localized, for: .normal)
        button.backgroundColor = .blue
        button.layer.cornerRadius = 5
        button.setTitleColor(.white, for: .normal)
        button.titleLabel?.font = .systemFont(ofSize: 14)
        button.addTarget(self, action: #selector(onTapSureButton), for: .touchUpInside)
        return button
    }()
    private lazy var titleLabel: UILabel = {
        let label = UILabel()
        label.text = ""
        label.textColor = .black
        label.font = UIFont.boldSystemFont(ofSize: 16)
        return label
    }()
    private lazy var pickerView: UIPickerView = {
        let pickerView = UIPickerView()
        pickerView.dataSource = self
        pickerView.delegate = self
        return pickerView
    }()
    private var selectedValue: String?
    
    // MARK: Public
    var pickerViewSelectedValueClosure: ((String) -> Void)?
    var dataArray: [String]?
    var rowHeight: CGFloat = 40
    
    override init(frame: CGRect) {
        super.init(frame: frame)
        setupUI()
    }
    
    required init?(coder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
    
    private func setupUI() {
        backgroundColor = .white
        addSubview(cancelButton)
        addSubview(titleLabel)
        addSubview(sureButton)
        addSubview(pickerView)
        cancelButton.translatesAutoresizingMaskIntoConstraints = false
        titleLabel.translatesAutoresizingMaskIntoConstraints = false
        sureButton.translatesAutoresizingMaskIntoConstraints = false
        pickerView.translatesAutoresizingMaskIntoConstraints = false
        
        widthAnchor.constraint(equalToConstant: UIScreen.main.bounds.width).isActive = true
        
        cancelButton.leadingAnchor.constraint(equalTo: leadingAnchor, constant: 10).isActive = true
        cancelButton.topAnchor.constraint(equalTo: topAnchor, constant: 5).isActive = true
        cancelButton.widthAnchor.constraint(equalToConstant: 50).isActive = true
        cancelButton.heightAnchor.constraint(equalToConstant: 30).isActive = true
        
        titleLabel.centerXAnchor.constraint(equalTo: centerXAnchor).isActive = true
        titleLabel.centerYAnchor.constraint(equalTo: cancelButton.centerYAnchor).isActive = true
        
        sureButton.centerYAnchor.constraint(equalTo: cancelButton.centerYAnchor).isActive = true
        sureButton.trailingAnchor.constraint(equalTo: trailingAnchor, constant: -10).isActive = true
        sureButton.widthAnchor.constraint(equalToConstant: 50).isActive = true
        sureButton.heightAnchor.constraint(equalToConstant: 30).isActive = true
        
        pickerView.leadingAnchor.constraint(equalTo: leadingAnchor).isActive = true
        pickerView.bottomAnchor.constraint(equalTo: bottomAnchor).isActive = true
        pickerView.trailingAnchor.constraint(equalTo: trailingAnchor).isActive = true
        pickerView.topAnchor.constraint(equalTo: cancelButton.bottomAnchor, constant: 5).isActive = true
        pickerView.heightAnchor.constraint(equalToConstant: 160).isActive = true
    }
    
    @objc
    func show() {
        AlertManager.show(view: self, alertPostion: .bottom)
    }
    
    @objc
    private func onTapCancelButton() {
        AlertManager.hiddenView()
    }
    @objc
    private func onTapSureButton() {
        pickerViewSelectedValueClosure?(selectedValue ?? "")
        AlertManager.hiddenView()
    }
}

extension PickerView: UIPickerViewDelegate, UIPickerViewDataSource {
    func pickerView(_ pickerView: UIPickerView, rowHeightForComponent component: Int) -> CGFloat {
        rowHeight
    }

    func pickerView(_ pickerView: UIPickerView, didSelectRow row: Int, inComponent component: Int) {
        selectedValue = dataArray?[row]
    }

    func pickerView(_ pickerView: UIPickerView, titleForRow row: Int, forComponent component: Int) -> String? {
        dataArray?[row] ?? ""
    }

    func numberOfComponents(in pickerView: UIPickerView) -> Int {
        1
    }
    func pickerView(_ pickerView: UIPickerView, numberOfRowsInComponent component: Int) -> Int {
        dataArray?.count ?? 0
    }
}
