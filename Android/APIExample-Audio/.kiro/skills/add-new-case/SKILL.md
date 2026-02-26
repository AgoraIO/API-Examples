---
name: add-new-case
description: Add a new audio API example case to APIExample-Audio (audio-only demo). Use when a PRD requires a new Agora audio feature demonstration. This project uses voice-sdk — no video APIs available.
---

# Add New Case — APIExample-Audio

## Step 1: Fill in the requirement template

Fill in the template below before proceeding. Do not skip any field.

```
## Case Requirement

- Case name:          # Display name shown in the list, e.g. "Voice Changer"
- Group:              # BASIC or ADVANCED
- Sort index:         # Position within the group — check existing cases to avoid duplicates
- Agora audio APIs:   # e.g. setAudioProfile(), setVoiceBeautifierPreset()
- Reference case:     # Closest existing case file, e.g. VoiceEffects.java
- Description:        # 1–3 sentences explaining what this case demonstrates
```

This project uses `voice-sdk`. Do NOT use any video APIs.
Forbidden calls: `enableVideo()`, `setupLocalVideo()`, `VideoCanvas`, `setVideoEncoderConfiguration()`.

## Step 2: Create the Fragment file

Path: `app/src/main/java/io/agora/api/example/examples/{basic|advanced}/YourCaseName.java`

```java
/**
 * This demo demonstrates how to use [describe the audio API feature here].
 *
 * Key APIs used:
 * - [list the primary Agora audio APIs, e.g. RtcEngine.setVoiceBeautifierPreset()]
 *
 * NOTE: This project uses voice-sdk. Do NOT call any video APIs.
 *
 * @see <a href="https://docs.agora.io/en/">Agora API Reference</a>
 */
@Example(
    index    = 10,
    group    = ADVANCED,
    name     = R.string.item_your_case_name,
    actionId = R.id.action_mainFragment_to_yourCaseName,
    tipsId   = R.string.your_case_name_tips
)
public class YourCaseName extends BaseFragment implements View.OnClickListener {
    private static final String TAG = YourCaseName.class.getSimpleName();

    private RtcEngine engine;
    private int myUid;
    private boolean joined = false;

    @Override
    public void onActivityCreated(@Nullable Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);
        // Check if the context is valid
        Context context = getContext();
        if (context == null) {
            return;
        }
        try {
            RtcEngineConfig config = new RtcEngineConfig();
            /**
             * The context of Android Activity
             */
            config.mContext = context.getApplicationContext();
            /**
             * The App ID issued to you by Agora. See
             * <a href="https://docs.agora.io/en/Agora%20Platform/token#get-an-app-id">
             * How to get the App ID</a>
             */
            config.mAppId = getString(R.string.agora_app_id);
            /**
             * Sets the channel profile of the Agora RtcEngine.
             * CHANNEL_PROFILE_COMMUNICATION(0): one-on-one or group calls.
             * CHANNEL_PROFILE_LIVE_BROADCASTING(1): live broadcast with broadcaster/audience roles.
             */
            config.mChannelProfile = Constants.CHANNEL_PROFILE_LIVE_BROADCASTING;
            /**
             * IRtcEngineEventHandler is an abstract class providing default implementation.
             * The SDK uses this class to report to the app on SDK runtime events.
             */
            config.mEventHandler = iRtcEngineEventHandler;
            config.mAudioScenario = Constants.AudioScenario.getValue(Constants.AudioScenario.DEFAULT);
            config.mAreaCode = ((MainApplication) getActivity().getApplication()).getGlobalSettings().getAreaCode();
            engine = RtcEngine.create(config);

            /**
             * This parameter is for reporting the usages of APIExample to Agora backend.
             * Generally, it is not necessary for you to set this parameter.
             */
            engine.setParameters("{"
                    + "\"rtc.report_app_scenario\":"
                    + "{"
                    + "\"appScenario\":" + 100 + ","
                    + "\"serviceType\":" + 11 + ","
                    + "\"appVersion\":\"" + RtcEngine.getSdkVersion() + "\""
                    + "}"
                    + "}");

            /**
             * Setting the local access point if the private cloud IP was set,
             * otherwise the config will be invalid.
             * IMPORTANT: getPrivateCloudConfig() can return null — always null-check.
             */
            LocalAccessPointConfiguration localAccessPointConfiguration =
                    ((MainApplication) getActivity().getApplication()).getGlobalSettings().getPrivateCloudConfig();
            if (localAccessPointConfiguration != null) {
                engine.setLocalAccessPoint(localAccessPointConfiguration);
            }
        } catch (Exception e) {
            e.printStackTrace();
            getActivity().onBackPressed();
        }
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        /**
         * leaveChannel and destroy the RtcEngine instance.
         * After calling destroy, you MUST NOT use the engine anymore.
         */
        if (engine != null) {
            engine.leaveChannel();
        }
        handler.post(RtcEngine::destroy);
        engine = null;
    }

    /**
     * IRtcEngineEventHandler is an abstract class providing default implementation.
     * The SDK uses this class to report to the app on SDK runtime events.
     * NOTE: All callbacks run on a BACKGROUND thread — use runOnUIThread() for UI updates.
     */
    private final IRtcEngineEventHandler iRtcEngineEventHandler = new IRtcEngineEventHandler() {
       
        @Override
        public void onJoinChannelSuccess(String channel, int uid, int elapsed) {
            Log.i(TAG, String.format("onJoinChannelSuccess channel %s uid %d", channel, uid));
            myUid = uid;
            joined = true;
            runOnUIThread(() -> { /* update join button, enable controls */ });
        }

        @Override
        public void onUserJoined(int uid, int elapsed) {
            Log.i(TAG, "onUserJoined -> " + uid);
            runOnUIThread(() -> { /* update audio seat */ });
        }

        @Override
        public void onUserOffline(int uid, int reason) {
            Log.i(TAG, String.format("user %d offline, reason %d", uid, reason));
            runOnUIThread(() -> { /* remove audio seat */ });
        }
    };
}
```

The engine initialization above is the **canonical pattern** — copy it exactly. Key points:
- `config.mAreaCode` must come from `GlobalSettings.getAreaCode()`
- `setParameters(...)` for app scenario reporting is required for all cases
- `getPrivateCloudConfig()` can return `null` — always null-check before calling `setLocalAccessPoint()`
- Wrap the entire block in try/catch and call `onBackPressed()` on failure
- All `IRtcEngineEventHandler` callbacks run on a background thread — always use `runOnUIThread()` for UI updates
- Include `TAG`, `myUid`, `joined` fields — they are used by nearly every case

## Step 3: Create the XML layout

Path: `app/src/main/res/layout/fragment_your_case_name.xml`

Typical structure for audio cases:
- Channel name input + Join/Leave button
- `AudioOnlyLayout` or `WaveformView` for audio status
- Feature control buttons (mute, effect toggle, etc.)

## Step 4: Add string resources

File: `app/src/main/res/values/strings.xml`

```xml
<string name="item_your_case_name">Your Case Name</string>
<string name="your_case_name_tips">This example demonstrates how to use XXX audio API.</string>
```

## Step 5: Add navigation entries

File: `app/src/main/res/navigation/nav_graph.xml`

Add an action inside `<fragment id="@+id/mainFragment">`:
```xml
<action
    android:id="@+id/action_mainFragment_to_yourCaseName"
    app:destination="@id/yourCaseName" />
```

Add a destination at the root level:
```xml
<fragment
    android:id="@+id/yourCaseName"
    android:name="io.agora.api.example.examples.advanced.YourCaseName"
    android:label="YourCaseName" />
```

## Step 6: Verify

```bash
./gradlew assembleDebug
```

- [ ] Case appears in the correct group
- [ ] Tapping the case navigates to the case screen
- [ ] Audio works after joining the channel
- [ ] No engine leak after leaving the screen
- [ ] Target audio API works as expected

## Common mistakes

| Symptom | Cause |
|---|---|
| Case not in list | `@Example` annotation missing, or class not under `io.agora.api.example.examples` |
| Crash on tap | `actionId` does not match the action id in `nav_graph.xml` |
| Build error: resource not found | String not added to `strings.xml` |
| Runtime crash `NoSuchMethodError` | Called a video API that does not exist in `voice-sdk` |
| NPE on `setLocalAccessPoint` | `getPrivateCloudConfig()` returns null — must null-check before calling |
