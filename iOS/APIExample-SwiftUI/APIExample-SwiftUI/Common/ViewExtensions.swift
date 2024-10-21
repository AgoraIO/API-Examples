//
//  Extensions.swift
//  APIExample-SwiftUI
//
//  Created by qinhui on 2024/10/17.
//

import SwiftUI
extension View {
    @ViewBuilder
    func adaptiveNavigationBarTitleDisplayMode(dispalayMode: NavigationBarItem.TitleDisplayMode = .inline) -> some View {
        if #available(iOS 14.0, *) {
            self.navigationBarTitleDisplayMode(dispalayMode)
        } else {
            self
        }
    }
    
    @ViewBuilder
    func adaptiveBackground(_ color: Color = .white, ignoresSafeAreaEdges edges: Edge.Set = .all) -> some View {
       if #available(iOS 15.0, *) {
           self.background(color, ignoresSafeAreaEdges: edges)
       } else {
           self.background(color.edgesIgnoringSafeArea(edges))
       }
   }
    
    @ViewBuilder
    func adaptiveNavigationTitle(_ title: String = "") -> some View {
        if #available(iOS 14.0, *) {
            self.navigationTitle(title)
                .adaptiveNavigationBarTitleDisplayMode()
        } else {
            self.navigationBarTitle(Text(title), displayMode: .inline)
        }
   }
    
    @ViewBuilder
    func adaptiveForegroundStyle(_ color: Color) -> some View {
        if #available(iOS 15.0, *) {
            self.foregroundStyle(color)
        } else {
            self.foregroundColor(color)
        }
    }
    
    @ViewBuilder
    func adaptiveCancelStyleButton(title: String, action: @escaping () -> Void) -> some View {
        if #available(iOS 15.0, *) {
            Button(title, role: .cancel,  action: action)
        } else {
            Button(title, action: action)
                .foregroundColor(.red)
        }
    }
    
    @ViewBuilder
    func adaptiveDialog<Actions: View>(
            title: String,
            isPresented: Binding<Bool>,
            @ViewBuilder actions: @escaping () -> Actions,
            actionSheetActions: [ActionSheet.Button]
        ) -> some View {
        if #available(iOS 15.0, *) {
            self.confirmationDialog(title, isPresented: isPresented, titleVisibility: .visible, actions: actions)
        } else {
            self.actionSheet(isPresented: isPresented) {
                ActionSheet(
                    title: Text(title),
                    buttons: actionSheetActions
                )
            }
        }
    }
    
    @ViewBuilder
    func adaptiveButtonStyle() -> some View {
        if #available(iOS 15.0, *) {
            self.buttonStyle(.borderedProminent)
        } else {
            self.buttonStyle(DefaultButtonStyle())
        }
    }
    
    @ViewBuilder
    func adaptiveTint(_ color: Color) -> some View {
        if #available(iOS 16.0, *) {
            self.tint(color)
        } else {
            self.foregroundColor(color)
        }
    }
}

