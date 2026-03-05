# query-cases

Find existing examples in the APIExample-SwiftUI project by feature, API name, or keyword.

## When to Use

- User asks "where is the screen sharing example?"
- User wants to find code for a specific Agora API
- User needs to understand how a feature is implemented in SwiftUI
- Before creating a new case, to check if similar functionality already exists

## Instructions

1. Check the `menus` array in `APIExample-SwiftUI/ContentView.swift` for the list of all registered examples
2. Each `MenuItem` contains:
   - `name`: Display name (localized)
   - `view`: The Entry view wrapped in `AnyView`
3. Example source files are located at:
   - `APIExample-SwiftUI/Examples/Basic/<ExampleName>/`
   - `APIExample-SwiftUI/Examples/Advanced/<ExampleName>/`
4. Each example folder typically contains:
   - `<ExampleName>.swift` — Entry view and Main view
   - `<ExampleName>RTC.swift` — RTC wrapper class (ObservableObject)

## Example Queries

| User Query | Search Strategy |
|------------|-----------------|
| "screen sharing" | Look for `ScreenShare` folder in Advanced |
| "custom audio" | Look for `CustomPCMAudioSource`, `CustomAudioRender` |
| "joinChannel" | Check `JoinChannelVideo`, `JoinChannelAudio` in Basic |
| "spatial audio" | Look for `SpatialAudio` in Advanced |

## Output Format

When reporting results, include:
- Example name and location
- Brief description of what the example demonstrates
- Key Agora APIs used in the example
