//
//  CustomRenderView.swift
//  APIExample-SwiftUI
//
//  Created by qinhui on 2024/8/12.
//

import SwiftUI

struct PixelBufferRenderMockContainerView: View {
    @ObservedObject var viewModel: PixelBufferRenderViewModel

    var localView = PixelBufferCustomRenderView()
    
    var body: some View {
        HStack {
            localView
            ForEach(viewModel.remoteRenderViews) { view in
                view
            }
        }
    }
}

@available(iOS 15.0, *)
struct PixelBufferRenderExample: View {
    @State var configs: [String: Any]
    
    //mock data
    @StateObject private var customRenderViewModel: PixelBufferRenderViewModel
    
    //pip controller
    @StateObject private var pipViewModel: PIPViewModel
    
    let myContainerView: PixelBufferRenderMockContainerView

    init(configs: [String: Any]) {
        _configs = State(initialValue: configs)
        
        let viewModel = PixelBufferRenderViewModel(configs: configs)
        _customRenderViewModel = StateObject(wrappedValue: viewModel)
        
        myContainerView = PixelBufferRenderMockContainerView(viewModel: viewModel)
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
            customRenderViewModel.setupRtcEngine(localView: self.myContainerView.localView.videoView)
        }
        .onDisappear {
            customRenderViewModel.cleanRtc()
        }
        .frame(maxWidth: .infinity, maxHeight: .infinity)
    }
}

struct CustomRenderExamplePreviews: PreviewProvider {
    static var previews: some View {
        if #available(iOS 15.0, *) {
            PixelBufferRenderExample(configs: [:])
        } else {
            // Fallback on earlier versions
        }
    }
}

