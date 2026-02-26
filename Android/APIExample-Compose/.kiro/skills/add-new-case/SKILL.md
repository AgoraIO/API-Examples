---
name: add-new-case
description: Add a new API example case to APIExample-Compose (Jetpack Compose demo). Use when a PRD requires a Compose UI implementation or porting an existing APIExample case to Compose. Kotlin only, no XML layouts.
---

# Add New Case — APIExample-Compose

## Step 1: Fill in the requirement template

Fill in the template below before proceeding. Do not skip any field.

```
## Case Requirement

- Case name:          # Display name shown in the list, e.g. "Video Snapshot"
                      # String key must use the prefix: example_
- Group:              # BasicExampleList or AdvanceExampleList
- Agora APIs:         # e.g. takeSnapshot(), setBeautyEffectOptions()
- Reference case:     # Closest existing case file — default to JoinChannelVideo.kt
- Description:        # 1–3 sentences explaining what this case demonstrates
```

This project has no reflection-based registration. You must manually add the entry to `model/Examples.kt`.

## Step 2: Create the Composable file

Path: `app/src/main/java/io/agora/api/example/compose/samples/YourCaseName.kt`

Follow the two-function pattern. `JoinChannelVideo.kt` is the canonical reference.

```kotlin
@Composable
fun YourCaseName() {
    val context = LocalContext.current
    val lifecycleOwner = LocalLifecycleOwner.current

    // State
    var isJoined by rememberSaveable { mutableStateOf(false) }
    var channelName by rememberSaveable { mutableStateOf("") }

    // RtcEngine — created once, survives recomposition
    val rtcEngine = remember {
        RtcEngine.create(RtcEngineConfig().apply {
            mContext = context
            mAppId = BuildConfig.AGORA_APP_ID
            mEventHandler = object : IRtcEngineEventHandler() {
                override fun onJoinChannelSuccess(channel: String?, uid: Int, elapsed: Int) {
                    isJoined = true   // safe to mutate Compose state from any thread
                }
                override fun onLeaveChannel(stats: RtcStats?) {
                    isJoined = false
                }
            }
        })
    }

    // Cleanup
    DisposableEffect(lifecycleOwner) {
        onDispose {
            if (isJoined) rtcEngine.leaveChannel()
            RtcEngine.destroy()
        }
    }

    // Permissions
    val permissionLauncher = rememberLauncherForActivityResult(
        ActivityResultContracts.RequestMultiplePermissions()
    ) { grantedMap ->
        if (grantedMap.values.all { it }) {
            TokenUtils.gen(channelName, 0) { token ->
                val options = ChannelMediaOptions().apply {
                    channelProfile = Constants.CHANNEL_PROFILE_LIVE_BROADCASTING
                    clientRoleType = Constants.CLIENT_ROLE_BROADCASTER
                }
                rtcEngine.joinChannel(token, channelName, 0, options)
            }
        }
    }

    // Stateless UI
    YourCaseNameView(
        channelName = channelName,
        isJoined = isJoined,
        onJoinClick = { name ->
            channelName = name
            permissionLauncher.launch(arrayOf(
                Manifest.permission.RECORD_AUDIO,
                Manifest.permission.CAMERA
            ))
        },
        onLeaveClick = { rtcEngine.leaveChannel() }
    )
}

@Composable
private fun YourCaseNameView(
    channelName: String,
    isJoined: Boolean,
    onJoinClick: (String) -> Unit,
    onLeaveClick: () -> Unit
) {
    Column(Modifier.fillMaxSize()) {
        ChannelNameInput(
            channelName = channelName,
            isJoined = isJoined,
            onJoinClick = onJoinClick,
            onLeaveClick = onLeaveClick
        )
    }
}
```

State rules:
- `rememberSaveable` — values that must survive rotation (channelName, isJoined, uid)
- `remember` — objects that must not be recreated (RtcEngine, collections)
- `IRtcEngineEventHandler` callbacks can mutate Compose state directly — the snapshot system is thread-safe
- **Android View APIs (Toast, Dialog, AlertDialog, etc.) MUST still be called on the main thread.** Use `coroutineScope.launch { }` (from `rememberCoroutineScope()`) to post to the main thread inside callbacks.

## Step 3: Register in Examples.kt

File: `app/src/main/java/io/agora/api/example/compose/model/Examples.kt`

```kotlin
val AdvanceExampleList = listOf(
    // … existing entries …
    Example(R.string.example_your_case_name) { YourCaseName() }
)
```

List order is display order.

## Step 4: Add string resource

File: `app/src/main/res/values/strings.xml`

```xml
<string name="example_your_case_name">Your Case Name</string>
```

The Compose project only needs the name string — there is no separate tips string.

## Step 5: Verify

```bash
./gradlew assembleDebug
```

- [ ] Case appears in the correct group on the home screen
- [ ] Tapping the case navigates to the case screen
- [ ] Channel join succeeds
- [ ] `DisposableEffect.onDispose` fires on back navigation (confirm in Logcat)
- [ ] State survives screen rotation (`rememberSaveable` values intact)
- [ ] Target API works as expected

## Common mistakes

| Symptom | Cause |
|---|---|
| Case not in list | Entry not added to `Examples.kt` |
| Build error: resource not found | String not added to `strings.xml` |
| State lost on rotation | Used `remember` instead of `rememberSaveable` |
| Engine not destroyed after leaving | Wrong key in `DisposableEffect`, or `RtcEngine.destroy()` missing in `onDispose` |
| Preview crash | `@Preview` on the stateful function — only preview the private `*View` function |
| Toast crash on background thread | Called `Toast`/`Dialog` inside `IRtcEngineEventHandler` callback without `coroutineScope.launch { }` |
