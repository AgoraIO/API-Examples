//
//  CustomRenderView.swift
//  APIExample-SwiftUI
//
//  Created by qinhui on 2024/8/12.
//

import SwiftUI

struct MyContainerView: View {
    var localView = PixelBufferCustomRenderView()
    var remoteView = PixelBufferCustomRenderView()
    
    var body: some View {
        HStack {
            localView
            remoteView
        }
    }
}

struct CustomRenderExample: View {
    @State var configs: [String: Any]
    
    //mock页面数据
    @StateObject private var customRenderViewModel: CustomRenderViewModel
    
    //pip管理类，用于管理pip状态
    @StateObject private var pipViewModel: PIPViewModel
    
    let myContainerView: MyContainerView

    init(configs: [String: Any]) {
        _configs = State(initialValue: configs)
        
        //step1：初始化控制PIP的viewModel，向viewModel提供需要进行PIP展示的视图（SwiftUI视图使用UIHostingController包装）
        myContainerView = MyContainerView()
        let pipSourceView = UIHostingController(rootView: myContainerView).view!
        _pipViewModel = StateObject(wrappedValue: PIPViewModel(pipSourceView: pipSourceView))
        
        //初始化页面数据管理类
        _customRenderViewModel = StateObject(wrappedValue: CustomRenderViewModel(configs: configs))
    }
        
    var body: some View {
        VStack(spacing: 30) {
            PIPDisplayView(viewModel: pipViewModel)
                .frame(maxWidth: .infinity, maxHeight: 200)
        
            Button {
                //step2:使用viewModel控制PIP状态
                pipViewModel.togglePiP()
            } label: {
                Text("画中画")
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
                Text("切换画中画尺寸")
            }
            
            Spacer()
        }
        .onAppear {
            //配置rtc
            customRenderViewModel.setupRtcEngine(localView: self.myContainerView.localView.videoView)
            
            //以下方法调用根据业务需要自行决定
            //添加远端视图（支持添加多个视图，视图布局由业务上层决定）
            customRenderViewModel.addRenderView(renderView: self.myContainerView.remoteView.videoView)
            
            //删除远端视图
//            customRenderViewModel.removeRenderView(renderView: self.myContainerView.remoteView.videoView)
        }
        .onDisappear {
            //清理rtc信息
            customRenderViewModel.cleanRtc()
        }
        .frame(maxWidth: .infinity, maxHeight: .infinity)
        .background(Color.orange)
    }
}

#Preview {
    CustomRenderExample(configs: [:])
}
