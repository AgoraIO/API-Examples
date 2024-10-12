//
//  PickerSheetView.swift
//  APIExample-SwiftUI
//
//  Created by qinhui on 2024/9/25.
//

import SwiftUI

struct PickerSheetView: View {
    @Binding var selectedOption: String
    var options: [String]
    @Binding var isPresented: Bool
    var onSave: (String) -> Void

    var body: some View {
        NavigationView {
            VStack {
                Picker("请选择一个选项", selection: $selectedOption) {
                    ForEach(options, id: \.self) { option in
                        Text(option).tag(option)
                    }
                }
                .pickerStyle(WheelPickerStyle())
                .labelsHidden()
            }
            .navigationBarTitle("选择器", displayMode: .inline)
            .navigationBarItems(
                leading: Button("取消") {
                    isPresented = false
                },
                trailing: Button("确定") {
                    onSave(selectedOption)
                    isPresented = false
                }
            )
        }
        .onAppear {
            if selectedOption.isEmpty, let firstOption = options.first {
                selectedOption = firstOption
            }
        }
    }
}

