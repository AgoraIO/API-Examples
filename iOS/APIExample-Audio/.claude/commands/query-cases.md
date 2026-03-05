# query-cases

Find existing examples in the APIExample-Audio project by feature, API name, or keyword.

## When to Use

- User asks "where is the audio mixing example?"
- User wants to find code for a specific Agora audio API
- User needs to understand how an audio feature is implemented
- Before creating a new case, to check if similar functionality already exists

## Instructions

1. Check the `menus` array in `APIExample-Audio/ViewController.swift` for the list of all registered examples
2. Each `MenuItem` contains:
   - `name`: Display name (localized)
   - `storyboard`: Storyboard file name (matches the example folder name)
   - `controller`: Main view controller identifier
3. Example source files are located at:
   - `APIExample-Audio/Examples/Basic/<ExampleName>/`
   - `APIExample-Audio/Examples/Advanced/<ExampleName>/`
4. Each example folder typically contains:
   - `<ExampleName>.swift` — Entry and Main view controllers
   - `Base.lproj/<ExampleName>.storyboard` — UI layout

## Example Queries

| User Query | Search Strategy |
|------------|-----------------|
| "audio mixing" | Look for `AudioMixing` folder in Advanced |
| "custom audio" | Look for `CustomAudioSource`, `CustomPcmAudioSource`, `CustomAudioRender` |
| "voice effects" | Look for `VoiceChanger` in Advanced |
| "spatial audio" | Look for `SpatialAudio` in Advanced |

## Output Format

When reporting results, include:
- Example name and location
- Brief description of what the example demonstrates
- Key Agora audio APIs used in the example
