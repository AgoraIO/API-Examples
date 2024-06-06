import { TitleBar } from '../common/TitleBar';
import { ExampleCategory, ExampleItem } from '../model/DataType';
import { COLLECTION_CATEGORIES } from './Data';
import { router } from '@kit.ArkUI';


@Extend(Column)
function ColumnStyle() {
  .width('100%')
  .borderRadius(24)
  .backgroundColor(Color.White)
  .padding({ left: 12, right: 12, bottom: 4, top: 4 })
}

@Entry
@Component
struct Index {
  @State message: string = 'Hello World';

  build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Start }) {
      TitleBar({ title: $r('app.string.app_name') })
        .flexGrow(0)

      Column(){
        ExampleCategoryNavigation(
          {
            categories: COLLECTION_CATEGORIES
          }
        )
      }
      .flexGrow(1)
      .flexBasis(100)
      .margin({ bottom: 10 })
    }
    .backgroundColor($r('app.color.background_shallow_grey'))
  }
}

@Component
export struct ExampleCategoryNavigation {
  private categories: ExampleCategory[] = new Array;

  build() {
    Column() {
      List() {
        ForEach(this.categories, (category: ExampleCategory, categoryIndex: number) => {
          ListItem() {
            Column() {
              Text(category.title)
                .height(48)
                .fontSize(14)
                .width('100%')
                .textAlign(TextAlign.Start)
                .fontFamily('HarmonyHeiTi-Medium')
                .fontColor($r('app.color.font_color_shallow'))
                .padding({ bottom: 4, top: 4, left: 4 })

              Column() {
                ForEach(category.childNodes, (item: ExampleItem,
                                              itemIndex: number) => {
                  ExampleItemNavigation({
                    exampleItem: item
                  })
                })
              }
              .ColumnStyle()
            }
          }
          .id('ListItem' + categoryIndex)
        })
      }
      .width('100%')
      .layoutWeight(1)
      .padding({ left: 16, right: 16 })
      .id('list_001')

      Blank()
    }
    .height('100%')
  }
}

@Component
struct ExampleItemNavigation {
  private exampleItem: ExampleItem | null = null;

  build() {
    Row() {
      if (this.exampleItem) {
        Text(this.exampleItem.title)
          .fontSize(16)
          .layoutWeight(1)
          .margin({ left: 4 })
          .align(Alignment.Start)
          .fontFamily('HarmonyHeiTi-Medium')
          .fontColor($r('app.color.font_color_dark'))
      }
      Blank()
    }
    .height(48)
    .width('100%')
    .onClick(() => {
      if (this.exampleItem) {
        // Click to jump to the corresponding page
        router.pushUrl({
          url: this.exampleItem.url
        })
      }
    })
  }
}