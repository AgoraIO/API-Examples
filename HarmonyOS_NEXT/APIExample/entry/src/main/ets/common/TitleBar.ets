/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import router from '@ohos.router'

// Page title bar
@Component
export struct TitleBar {
  title: Resource = $r('app.media.ic_back');
  showBack: boolean = false;
  showSetting: boolean = false;

  build() {
    Column() {
      Row() {
        if (this.showBack) {
          Image($r('app.media.ic_back'))
            .width(20)
            .height(20)
            .margin({ left: 26 })
            .objectFit(ImageFit.Contain)
            .onClick(() => {
              router.back()
            })
            .id('backBtn')
        }
        Text(this.title)
          .fontSize(20)
          .layoutWeight(1)
          .margin({ left: 16 })
          .align(Alignment.Start)
        Blank()

        if (this.showSetting) {
          Image($r('app.media.ic_settings'))
            .width(20)
            .height(20)
            .margin({ right: 26 })
            .objectFit(ImageFit.Contain)
            .onClick(() => {
              router.pushUrl({
                url: 'pages/Settings'
              })
            })
            .id('settingsBtn')
        }
      }
      .height(56)
      .width('100%')
    }
  }
}