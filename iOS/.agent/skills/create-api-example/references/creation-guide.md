# 如何创建新的 API 示例

在 APIExample 工程中**新增**一个独立示例时，请按本清单执行。关于命名、结构、Storyboard、菜单等约定，见 `api-example-conventions` skill。

## 1. 确定示例标识与分区

- **标识：** 简短的 PascalCase 名称（如 `MyFeature`），将用于文件夹名、storyboard 名及 storyboard 中的 identifier。
- **分区：** 将示例放在 **Basic** 或 **Advanced**（`APIExample/Examples/Basic/` 或 `APIExample/Examples/Advanced/`）。

## 2. 创建文件夹与 Swift 文件

- 创建文件夹：  
  `APIExample/Examples/<Basic|Advanced>/<示例名>/`
- 在该文件夹下添加 Swift 文件（如 `MyFeature.swift`）。

**Entry 类（`<示例名>Entry`）：**

- 继承 `UIViewController`。
- 至少包含：
  - 频道名输入框的 `@IBOutlet`。
  - 加入按钮的 `@IBOutlet` 或 action。
  - `let identifier = "<示例名>"`（与文件夹名一致）。
- 在「加入」的 action 中：
  - 从输入框读取频道名。
  - 调用 `channelTextField.resignFirstResponder()`。
  - `let storyBoard = UIStoryboard(name: identifier, bundle: nil)`。
  - `guard let newViewController = storyBoard.instantiateViewController(withIdentifier: identifier) as? BaseViewController else { return }`。
  - `newViewController.title = channelName`。
  - `newViewController.configs = ["channelName": channelName]`（如需可增加 resolution、fps、orientation 等键）。
  - `navigationController?.pushViewController(newViewController, animated: true)`。

**Main 类（`<示例名>Main`）：**

- 继承 `BaseViewController`。
- 在 `viewDidLoad()` 中从 `configs` 读取参数（如 `configs["channelName"] as? String`）。
- 使用 `KeyCenter.AppId`、`GlobalSettings.shared.area` 创建 `AgoraRtcEngineKit`，需要时调用 `Util.configPrivatization(agoraKit:)`。
- 实现 Agora 业务逻辑（加入频道、开关音视频等），并实现 `AgoraRtcEngineDelegate` 等协议。
- 若示例需要展示本地/远端画面，使用 `AGEVideoContainer` / `Bundle.loadVideoView` 等布局。

参考实现：`JoinChannelVideo.swift`、`VideoProcess.swift`。

## 3. 创建 Storyboard

- 创建文件：  
  `APIExample/Examples/<Basic|Advanced>/<示例名>/Base.lproj/<示例名>.storyboard`
- 添加**两个视图控制器**：

  **场景 1 – Entry**

  - **Storyboard ID** 设为 `EntryViewController`。
  - **Custom Class** 设为 `<示例名>Entry`，Module 为 `APIExample`。
  - 添加频道名输入框和加入按钮，并连接到 Entry 类（outlet 与加入按钮的 action）。

  **场景 2 – Main**

  - **Storyboard ID** 设为 `<示例名>`（与标识一致）。
  - **Custom Class** 设为 `<示例名>Main`，Module 为 `APIExample`。
  - 搭建示例主界面，并将所需 outlet/action 连接到 Main 类。

- 确保从菜单进入该 storyboard 时，首先展示的是 Entry（应用通过 identifier `EntryViewController` 加载 Entry）。

## 4. 在菜单中注册示例

- 打开 `APIExample/ViewController.swift`。
- 找到对应的 `MenuSection`（`"Basic"` 或 `"Anvanced"`）。
- 在该 section 的 `rows` 中追加一条 `MenuItem`，例如：

  ```swift
  MenuItem(name: "My Feature".localized, storyboard: "MyFeature", controller: "MyFeature")
  ```

- `storyboard` 与 storyboard 文件名（不含扩展名）一致。`controller` 可与 Main VC 的 storyboard identifier 一致，或填 `""`（由 Entry 负责 push Main 即可）。

## 5. 将 Storyboard 加入 Xcode 工程

- 新 storyboard 必须加入 app target 的 **Resources**，才会参与构建。
- 在 Xcode 中：右键示例所在分组（或 `Examples`）→「Add Files to "<Target>"…」，选择 `<示例名>.storyboard`，并勾选 APIExample target。  
  或在 `APIExample.xcodeproj/project.pbxproj` 中添加文件引用与 **PBXBuildFile**，并加入 target 的 Resources 构建阶段。

## 6. 可选：分辨率 / 帧率 / 方向

若示例需要设置视频分辨率、帧率或方向：

- 在 **Entry** 中增加相应 UI（如按钮），用变量保存（参考 `JoinChannelVideoEntry` 的 `width`、`height`、`fps`、`orientation`）。
- 通过 `configs` 传给 Main，如 `"resolution": CGSize(...)`、`"fps": fps`、`"orientation": orientation`。
- 在 **Main** 中从 `configs` 读取，必要时用 `GlobalSettings.shared.getSetting(key: "resolution")` 等取默认值，再传给 `setVideoEncoderConfiguration` 等 API。

## 7. 创建 SKILL.md API 使用指南

在示例目录下创建 `SKILL.md` 文件，用于指导用户在业务场景中使用相关 API。

**文件路径：** `APIExample/Examples/<Basic|Advanced>/<示例名>/SKILL.md`

**使用模板：** 复制 `.agent/skills/create-api-example/assets/SKILL-template.md` 并填充内容

**内容结构：**
1. **YAML frontmatter** - name, description, metadata
2. **功能说明** - 简要描述示例的核心功能
3. **核心 API 调用流程** - 5-7 个关键步骤，每步都有代码示例
4. **关键事件回调** - 列出重要的 delegate 方法及其作用
5. **常见问题** - 3-6 个 FAQ 及解决方案
6. **参考文档** - 官方文档链接
7. **相关示例** - 其他相关示例的引用

**参考示例：** `APIExample/Examples/Basic/JoinChannelVideo/SKILL.md`

**编写要点：**
- 从示例代码中提取关键 API 调用
- 提供完整的代码片段，不要省略重要参数
- 解释每个参数的作用和可选值
- 列出常见问题和解决方案
- 添加官方文档链接

**YAML frontmatter 示例：**
```yaml
---
name: video-process-guide
description: 指导在业务场景中实现视频处理功能，包括滤镜、水印和美颜
compatibility: [Cursor, Kiro]
license: MIT
metadata:
  example: VideoProcess
  category: Advanced
  apis: [setVideoEncoderConfiguration, setBeautyEffectOptions, addVideoWatermark]
---
```

## 清单小结

- [ ] 在 `APIExample/Examples/Basic/` 或 `Advanced/` 下创建示例文件夹。
- [ ] 实现 Entry 类（`<示例名>Entry`）：频道输入、加入 action、通过 `configs` 推送 Main。
- [ ] 实现 Main 类（`<示例名>Main`）：继承 `BaseViewController`，使用 `configs` 与 Agora API。
- [ ] 创建 Storyboard：Entry（ID `EntryViewController`，Class `<示例名>Entry`）与 Main（ID `<示例名>`，Class `<示例名>Main`）。
- [ ] 创建 SKILL.md：API 使用指南，包括调用流程、业务场景和常见问题。
- [ ] 在 `ViewController.swift` 的对应 section 中新增一条 `MenuItem`。
- [ ] 将 storyboard 加入 Xcode target 的 Resources（或更新 project.pbxproj）。

完成后，构建并运行应用，新示例会出现在列表中，并可从入口页加入频道或运行对应演示。
