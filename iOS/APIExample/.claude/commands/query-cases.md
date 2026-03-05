# query-cases

Find existing examples in the APIExample project by feature, API name, or keyword.

## When to Use

- User asks "where is the screen sharing example?"
- User wants to find code for a specific Agora API
- User needs to understand how a feature is implemented
- Before creating a new case, to check if similar functionality already exists

## Instructions

1. Check the `menus` array in `APIExample/ViewController.swift` for the list of all registered examples
2. Each `MenuItem` contains:
   - `name`: Display name (localized)
   - `storyboard`: Storyboard file name (matches the example folder name)
   - `controller`: Main view controller identifier
3. Example source files are located at:
   - `APIExample/Examples/Basic/<ExampleName>/`
   - `APIExample/Examples/Advanced/<ExampleName>/`
4. Each example folder typically contains:
   - `<ExampleName>.swift` — Entry and Main view controllers
   - `Base.lproj/<ExampleName>.storyboard` — UI layout

## Example Queries

| User Query | Search Strategy |
|------------|-----------------|
| "screen sharing" | Look for `ScreenShare` folder in Advanced |
| "custom video" | Look for `CustomVideoSourcePush`, `CustomVideoRender` |
| "joinChannel" | Check `JoinChannelVideo`, `JoinChannelAudio` in Basic |
| "beauty filter" | Look for `AgoraBeauty`, `ThirdBeautify`, `VideoProcess` |

## Output Format

When reporting results, include:
- Example name and location
- Brief description of what the example demonstrates
- Key Agora APIs used in the example
