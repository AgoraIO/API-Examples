

/**
 * Select in item style, provide multiple choices based on menuList
 */
@Component
export struct CommonItemSelect {
  private selects: ResourceStr[] = [];
  private callback: (index: number, value?: string) => void = () => {
  };
  private selectOption: SelectOption[] = [];
  private name: ResourceStr = '';
  private testId: string = 'test_itemSelect';
  private selectIndex: number = 0;
  private fontSize: Font = { size: 15, weight: FontWeight.Medium };
  private isItemStyle: boolean = true;

  aboutToAppear() {
    this.selects.forEach((selects, index) => {
      this.selectOption[index] = { value: selects }
    })
  }

  build() {
    Row() {
      Text(this.name)
        .margin({ left: this.isItemStyle ? 0 : 12, right: this.isItemStyle ? 0 : 12 })
      Blank()
      Select(this.selectOption)
        .id(this.testId)
        .value(this.selects[this.selectIndex])
        .backgroundColor($r('app.color.background_shallow_grey'))
        .borderRadius(19)
        .constraintSize({ minWidth: 150 })
        .selected(this.selectIndex)
        .font(this.fontSize)
        .optionFont(this.fontSize)
        .onSelect((index, value) => {
          this.callback(index, value)
          this.selectIndex = index
        })
        .height('100%')
    }
    .size({ width: '100%', height: this.isItemStyle ? 42 : 70 })
    .padding({ left: this.isItemStyle ? 0 : 12, right: this.isItemStyle ? 0 : 12 })
    .borderRadius(this.isItemStyle ? 0 : 24)
    .backgroundColor(this.isItemStyle ? Color.Transparent : Color.White)
    .margin({ top: 6, bottom: 6 })
  }
}