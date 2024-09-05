export type OnSelectListener = (index: number, text: string) => void

export type OnCheckListener = (isCheck: boolean) => void

export type OnSliderListener = (onProgress: number) => void

@Component
export struct AudioChannelSlider {
  @Prop isEnable: boolean = true
  @Prop name: Resource | string = ''
  private onSlide?: OnSliderListener
  @State num: number = 0

  build() {
    Row() {
      Text(this.name)
      Slider({
        value: this.num,
        min: 0,
        max: 100,
        style: SliderStyle.OutSet
      })// .showTips(true)
        .opacity(this.isEnable ? 1 : 0.4)
        .enabled(this.isEnable)
        .onChange((value: number, mode: SliderChangeMode) => {
          this.num = value
          this.onSlide?.(value)
        })
        .width('50%')
    }
    .width('100%')
    .justifyContent(FlexAlign.SpaceBetween)
  }
}

@Component
export struct AudioChannelToggle {
  @Prop isEnable: boolean = true
  @Prop name: Resource | string = ''
  @State isCheck: boolean = true
  private onCheck?: OnCheckListener

  build() {
    Row() {
      Text(this.name)
      Toggle({ type: ToggleType.Switch, isOn: this.isEnable && this.isCheck })
        .onChange((isOn: boolean) => {
          this.isCheck = isOn
          this.onCheck?.(isOn)
        })
    }
    .enabled(this.isEnable)
    .width('100%')
    .justifyContent(FlexAlign.SpaceBetween)
  }
}

@Component
export struct AudioChannelSpinner {
  @Prop isEnable: boolean = true
  @Prop name: Resource | string = ''
  @Prop items: (Resource | string)[] = []
  private onSelect?: OnSelectListener
  @State selectIndex: number = 0

  build() {
    Row() {
      Text(this.name)
      if (this.items && this.items.length > 0) {
        Select(this.items.map(item => {
          const opt: SelectOption = { value: item }
          return opt;
        }))
          .enabled(this.isEnable)
          .selected(this.selectIndex)
          .value(this.items[this.selectIndex])
          .font({ size: 16, weight: 500 })// .fontColor('#182431')
          .selectedOptionFont({ size: 16, weight: 400 })
          .optionFont({ size: 16, weight: 400 })
          .space(10)
          .arrowPosition(ArrowPosition.END)
          .menuAlign(MenuAlignType.START, { dx: 0, dy: 0 })
          .optionWidth(300)
          .optionHeight(300)
          .onSelect((index: number, text: string) => {
            this.selectIndex = index
            this.onSelect?.(index, text)
          })
      }
    }
    .width('100%')
    .justifyContent(FlexAlign.SpaceBetween)
  }
}