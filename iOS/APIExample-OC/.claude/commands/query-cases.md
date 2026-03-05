# query-cases

Find existing examples in the APIExample-OC project by feature, API name, or keyword.

## When to Use

- User asks "where is the screen sharing example?"
- User wants to find code for a specific Agora API in Objective-C
- User needs to understand how a feature is implemented
- Before creating a new case, to check if similar functionality already exists

## Instructions

1. Check the `+[MenuSection menus]` method in `APIExample-OC/ViewController.m` for the list of all registered examples
2. Each `MenuItem` contains:
   - `name`: Display name (localized)
   - `storyboard`: Storyboard file name (matches the example folder name)
   - `controller`: Main view controller identifier
3. Example source files are located at:
   - `APIExample-OC/Examples/Basic/<ExampleName>/`
   - `APIExample-OC/Examples/Advanced/<ExampleName>/`
4. Each example folder typically contains:
   - `<ExampleName>.h` / `<ExampleName>.m` — Entry and Main view controllers
   - `<ExampleName>.storyboard` — UI layout

## Example Queries

| User Query | Search Strategy |
|------------|-----------------|
| "screen sharing" | Look for `ScreenShare` folder in Advanced |
| "custom video" | Look for `CustomVideoSourcePush`, `CustomVideoRender` |
| "joinChannel" | Check `JoinChannelVideo`, `JoinChannelAudio` in Basic |
| "spatial audio" | Look for `SpatialAudio` in Advanced |

## Output Format

When reporting results, include:
- Example name and location
- Brief description of what the example demonstrates
- Key Agora APIs used in the example
