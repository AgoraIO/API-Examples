//
//  SDKRenderExample.swift
//  APIExample-SwiftUI
//
//  Created by qinhui on 2024/8/13.
//

import SwiftUI

struct SDKRenderMockContainerView: View {
    @ObservedObject var viewModel: SDKRenderViewModel
    let localView = VideoView()
    
    var body: some View {
        HStack {
            localView.frame(maxWidth: 200)
            ForEach(viewModel.remoteRenderViews) { view in
                view
            }
        }
    }
}

struct SDKRenderExample: View {
    @State var configs: [String: Any]

    //mock data
    @StateObject private var sdkRenderViewModel: SDKRenderViewModel
    
    //pip controller
    @StateObject private var pipViewModel: PIPViewModel
    
    let myContainerView: SDKRenderMockContainerView

    init(configs: [String: Any]) {
        _configs = State(initialValue: configs)
                
        let viewModel = SDKRenderViewModel(configs: configs)
        _sdkRenderViewModel = StateObject(wrappedValue: viewModel)
        
        myContainerView = SDKRenderMockContainerView(viewModel: viewModel)
        let pipSourceView = UIHostingController(rootView: myContainerView).view!
        _pipViewModel = StateObject(wrappedValue: PIPViewModel(pipSourceView: pipSourceView))
    }
    
    var body: some View {
        VStack(spacing: 30) {
            PIPDisplayView(viewModel: pipViewModel)
                .frame(maxWidth: .infinity, maxHeight: 200)
        
            Button {
                pipViewModel.togglePiP()
            } label: {
                Text("PIP")
            }
            
            Button {
                pipViewModel.changePIPSize {
                     let pipSizes = [
                            CGSize(width: 150, height: 300),
                            CGSize(width: 300, height: 150)
                        ]
                    let i = Int.random(in: 0..<pipSizes.count)
                    return pipSizes[i]
                }
            } label: {
                Text("Change PIP Size")
            }
            
            Spacer()
        }
        .onAppear {
            sdkRenderViewModel.setupRtcEngine(localView: self.myContainerView.localView.videoView)
        }
        .onDisappear {
            sdkRenderViewModel.cleanRtc()
        }
        .frame(maxWidth: .infinity, maxHeight: .infinity)
        .background(Color.orange)
    }
}

//#Preview {
//    SDKRenderExample(configs: [:])
//}
