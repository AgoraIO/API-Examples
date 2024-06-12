import { ExampleCategory, ExampleItem } from '../model/DataType'

export const BASIC_ITEMS: ExampleItem[] =
  [
    {
      title: $r('app.string.item_joinvideochannel'),
      url: 'pages/basic/JoinVideoChannel'
    }
  ]

export const ADVANCE_ITEMS: ExampleItem[] =
  [
    {
      title: $r('app.string.item_joinmultichannel'),
      url: 'pages/advance/JoinMultiChannel'
    },
    {
      title: $r('app.string.item_mediametadata'),
      url: 'pages/advance/MediaMetadata'
    },
    {
      title: $r('app.string.item_customaudiosource'),
      url: 'pages/advance/CustomAudioSource'
    }
  ]

export const COLLECTION_CATEGORIES: ExampleCategory[] =
  [
    {
      childNodes: BASIC_ITEMS,
      title: $r('app.string.category_basic')
    },
    {
      childNodes: ADVANCE_ITEMS,
      title: $r('app.string.category_advance')
    },
  ]