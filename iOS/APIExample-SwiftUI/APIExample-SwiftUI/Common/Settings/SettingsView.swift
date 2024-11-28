//
//  SettingsView.swift
//  APIExample-SwiftUI
//
//  Created by zhaoyongqiang on 2024/3/20.
//

import SwiftUI
import AgoraRtcKit

enum SettingsCellType {
    case text
    case select
    case input
    case toggle
}

class SettingsSectionModel: Identifiable, ObservableObject {
    let id = UUID()
    var name: String = ""
    @Published var settingsModel: [SettingsModel] = []
    
    init(name: String, settingsModel: [SettingsModel]) {
        self.name = name
        self.settingsModel = settingsModel
    }
}

class SettingsModel: Identifiable, ObservableObject {
    let id = UUID()
    var key: String = ""
    var label: String = ""
    var settingItem: SettingItem?
    @Published var value: String = ""
    var placeHolder: String?
    @Published var isOn: Bool = false
    @Published var isShowDialog: Bool = false
    var type: SettingsCellType = .text
    
    init(key: String, label: String, settingItem: SettingItem? = nil, value: String = "", placeHolder: String? = nil, isOn: Bool = false, isShowDialog: Bool = false, type: SettingsCellType) {
        self.key = key
        self.label = label
        self.settingItem = settingItem
        self.value = value
        self.placeHolder = placeHolder
        self.isOn = isOn
        self.isShowDialog = isShowDialog
        self.type = type
    }
}

class ViewModel: ObservableObject {
    @Published var items: [SettingsSectionModel] = []
    
    init(items: [SettingsSectionModel] = []) {
        self.items = items
        createData()
    }
    
    func addItem(_ item: SettingsSectionModel) {
        items.append(item)
    }
    
    private func createData() {
        var videoSettingsModel = [SettingsModel]()
        videoSettingsModel.append(SettingsModel(key: "resolution", label: "Resolution".localized, settingItem: GlobalSettings.shared.getSetting(key: "resolution"), value: GlobalSettings.shared.getSetting(key: "resolution")?.selectedOption().label ?? "", type: .select))
        videoSettingsModel.append(SettingsModel(key: "fps", label: "Frame Rate".localized, settingItem: GlobalSettings.shared.getSetting(key: "fps"), value: GlobalSettings.shared.getSetting(key: "fps")?.selectedOption().label ?? "", type: .select))
        videoSettingsModel.append(SettingsModel(key: "orientation", label: "Orientation".localized, settingItem: GlobalSettings.shared.getSetting(key: "orientation"), value: GlobalSettings.shared.getSetting(key: "orientation")?.selectedOption().label ?? "", type: .select))
        videoSettingsModel.append(SettingsModel(key: "role", label: "Pick Role".localized, settingItem: GlobalSettings.shared.getSetting(key: "role"), value: GlobalSettings.shared.getSetting(key: "role")?.selectedOption().label ?? "", type: .select))
        let videoSectionModel = SettingsSectionModel(name: "Video Configurations", settingsModel: videoSettingsModel)
        
        addItem(videoSectionModel)
        
        var metadataSettingsModel = [SettingsModel]()
        metadataSettingsModel.append(SettingsModel(key: "sdk_ver", label: "SDK Version", value: "\(AgoraRtcEngineKit.getSdkVersion())", type: .text))
        let metadata = SettingsSectionModel(name: "Metadata", settingsModel: metadataSettingsModel)
        
        addItem(metadata)
        
        var privateCloudSettingsModel = [SettingsModel]()
        privateCloudSettingsModel.append(SettingsModel(key: "ip", label: "IP Address", value: GlobalSettings.shared.getCache(key: "ip"), placeHolder: "please input IP", type: .input))
        privateCloudSettingsModel.append(SettingsModel(key: "report", label: "Log Report", isOn: false, type: .toggle))
        privateCloudSettingsModel.append(SettingsModel(key: "log_server_domain", label: "Log Server Domain", value: GlobalSettings.shared.getCache(key: "log_server_domain"), placeHolder: "please input log server IP", type: .input))
        privateCloudSettingsModel.append(SettingsModel(key: "log_server_port", label: "Log Server Port", value: GlobalSettings.shared.getCache(key: "log_server_port"), placeHolder: "please input Server Port", type: .input))
        privateCloudSettingsModel.append(SettingsModel(key: "log_server_path", label: "Log Server Path", value: GlobalSettings.shared.getCache(key: "log_server_path"), placeHolder: "please input Server Path", type: .input))
        privateCloudSettingsModel.append(SettingsModel(key: "log_server_https", label: "Use Https", isOn: false, type: .toggle))
        let privateCloud = SettingsSectionModel(name: "Private cloud", settingsModel: privateCloudSettingsModel)
        
        addItem(privateCloud)
    }
}

//struct SettingsView: View {
//    @ObservedObject var viewModel = ViewModel()
//    
//    var body: some View {
//        List {
//            ForEach(viewModel.items) { section in
//                Section(section.name) {
//                    ForEach(section.settingsModel, id: \.id) { item in
//                        let binding = bindindItem(item: item)
//                        
//                        switch item.type {
//                        case .text:
//                            HStack {
//                                Text(item.label)
//                                Spacer()
//                                Text(item.value)
//                            }
//                        case .select:
//                            HStack {
//                                Text(item.label)
//                                Spacer()
//                                Button(item.value) {
//                                    item.isShowDialog = true
//                                    viewModel.objectWillChange.send()
//                                }
//                                .confirmationDialog(item.label, isPresented: binding.isShowDialog, titleVisibility: .visible) {
//                                    ForEach(item.settingItem?.options ?? []) { option in
//                                        Button(option.label) {
//                                            GlobalSettings.shared.cache[item.key] = option.value
//                                            item.value = option.label
//                                        }
//                                    }
//                                }
//                            }
//                        case .toggle:
//                            HStack {
//                                Text(item.label)
//                                Spacer()
//                                Toggle("", isOn: binding.isOn)
//                                    .onChange(of: item.isOn) { value in
//                                        GlobalSettings.shared.cache[item.key] = value
//                                    }
//                            }
//                        case .input:
//                            HStack {
//                                Text(item.label)
//                                Spacer()
//                                TextField(item.placeHolder ?? "", text: binding.value)
//                                    .textFieldStyle(.roundedBorder)
//                                    .onChange(of: item.value) { text in
//                                        GlobalSettings.shared.cache[item.key] = text
//                                    }
//                            }
//                        }
//                    }
//                }
//            }
//        }
//        .navigationTitle("Settings")
//    }
//    
//    private func bindindItem(item: SettingsModel) -> Binding<SettingsModel> {
//        let index = viewModel.items.firstIndex(where: { $0.settingsModel.contains(where: { $0.id == item.id } )}) ?? 0
//        let subIndex = viewModel.items[index].settingsModel.firstIndex(where: { $0.id == item.id }) ?? 0
//        return $viewModel.items[index].settingsModel[subIndex]
//    }
//}

//#Preview {
//    SettingsView()
//}
