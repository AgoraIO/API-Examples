//
//  KtvCopyrightMusic.swift
//  APIExample-SwiftUI
//
//  Created by zhaoyongqiang on 2024/3/25.
//

import SwiftUI

struct KtvCopyrightMusic: View {
    var body: some View {
        ZStack {
            Button("Ktv copyright music".localized) {
                let urlString = "https://docs.agora.io/en/interactive-live-streaming/overview/product-overview?platform=ios"
                
                if let url = URL(string: urlString) {
                    UIApplication.shared.open(url)
                }
            }
            .adaptiveButtonStyle()
        }
    }
}

struct KtvCopyrightMusicPreviews: PreviewProvider {
    static var previews: some View {
        KtvCopyrightMusic()
    }
}
