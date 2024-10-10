//
//  PickerView.swift
//  APIExample-SwiftUI
//
//  Created by zhaoyongqiang on 2024/4/1.
//

import SwiftUI

struct PickerView: View {
    @Binding var dataArray: [String]
    @State var onTapCancel: () -> Void
    @State var onTapSure: (String) -> Void
    @State private var selectedValue = ""
    var body: some View {
        ZStack {
            Picker("", selection: $selectedValue) {
                ForEach(dataArray, id: \.self) { value in
                    Text(value)
                }
            }
            .pickerStyle(WheelPickerStyle())
            HStack {
                Button("Cancel".localized) {
                    onTapCancel()
                }
                .padding(.horizontal, 5)
                .padding(.vertical, 3)
                .background(.blue)
                .foregroundStyle(.white)
                .clipShape(.rect(cornerRadius: 5))
                Spacer()
                Button("Sure".localized) {
                    let value = selectedValue.isEmpty ? dataArray.first : selectedValue
                    onTapSure(value ?? "")
                }
                .padding(.horizontal, 5)
                .padding(.vertical, 3)
                .background(.blue)
                .foregroundStyle(.white)
                .clipShape(.rect(cornerRadius: 5))
            }
            .padding(.horizontal, 20)
            .padding(.bottom, 150)
        }.background(ignoresSafeAreaEdges: .bottom)
    }
}

#Preview {
    PickerView(dataArray: .constant(["aa", "bb", "cc"]), onTapCancel: {
        
    }, onTapSure: { value in
        print(value)
    })
}
