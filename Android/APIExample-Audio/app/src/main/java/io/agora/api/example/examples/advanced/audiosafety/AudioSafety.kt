package io.agora.api.example.examples.advanced.audiosafety

import android.os.Bundle
import android.os.Handler
import android.os.Looper
import android.util.Log
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.Button
import android.widget.EditText
import androidx.fragment.app.setFragmentResultListener
import io.agora.api.example.MainApplication
import io.agora.api.example.R
import io.agora.api.example.common.BaseFragment
import io.agora.api.example.common.widget.AudioOnlyLayout
import io.agora.api.example.common.widget.AudioSeatManager
import io.agora.api.example.utils.CommonUtil
import io.agora.api.example.utils.PermissonUtils
import io.agora.api.example.utils.TokenUtils
import io.agora.rtc2.ChannelMediaOptions
import io.agora.rtc2.Constants
import io.agora.rtc2.IRtcEngineEventHandler
import io.agora.rtc2.RtcEngine
import io.agora.rtc2.RtcEngineConfig
import kotlin.random.Random

/**
 * Audio Safety Example
 *
 * This example demonstrates how to use AudioSafetyManager to:
 * - Record audio from local and remote users in a circular buffer
 * - Report users and generate WAV files with audio evidence
 * - Support moderation/safety features for voice chat
 */
class AudioSafety : BaseFragment(), View.OnClickListener {

    companion object {
        private const val TAG = "AudioSafety"
    }

    private lateinit var etChannel: EditText
    private lateinit var btnJoin: Button
    private var engine: RtcEngine? = null
    private var audioSafetyManager: AudioSafetyManager? = null
    private var audioSeatManager: AudioSeatManager? = null
    private val myUid =
        Random.nextInt(10000, 100000000)  // Random UID known in advance, used for both config.localUid and joinChannel
    private var joined: Boolean = false

    // Current config values (can be updated from config page)
    private var currentEnableRegisterLocal: Boolean = true
    private var currentBufferDurationSeconds: Int = 300

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        handler = Handler(Looper.getMainLooper())
    }

    override fun onCreateView(inflater: LayoutInflater, container: ViewGroup?, savedInstanceState: Bundle?): View? {
        val view = inflater.inflate(R.layout.fragment_audio_safety, container, false)
        return view
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)

        etChannel = view.findViewById(R.id.et_channel)
        btnJoin = view.findViewById(R.id.btn_join)
        btnJoin.setOnClickListener(this)

        // Initialize audio seat manager for displaying users (8 seats)
        audioSeatManager = AudioSeatManager(
            view.findViewById(R.id.audio_place_01),
            view.findViewById(R.id.audio_place_02),
            view.findViewById(R.id.audio_place_03),
            view.findViewById(R.id.audio_place_04),
            view.findViewById(R.id.audio_place_05),
            view.findViewById(R.id.audio_place_06),
            view.findViewById(R.id.audio_place_07),
            view.findViewById(R.id.audio_place_08)
        )

        // Set click listeners on audio seats to report users
        setupReportListeners(view)

        // Listen for config result from config page
        setFragmentResultListener(AudioSafetyConfigFragment.REQUEST_KEY) { _, bundle ->
            currentEnableRegisterLocal = bundle.getBoolean(AudioSafetyConfigFragment.ARG_ENABLE_REGISTER_LOCAL, true)
            currentBufferDurationSeconds = bundle.getInt(AudioSafetyConfigFragment.ARG_BUFFER_DURATION_SECONDS, 300)

            // Recreate AudioSafetyManager with new config if engine exists
            engine?.let { rtcEngine ->
                audioSafetyManager?.release()
                val newConfig = AudioSafetyManagerConfig(
                    context = requireContext(),
                    rtcEngine = rtcEngine,
                    localUid = myUid,
                    enableRegisterLocal = currentEnableRegisterLocal,
                    bufferDurationSeconds = currentBufferDurationSeconds
                )
                audioSafetyManager = AudioSafetyManager(newConfig)
            }
        }
    }

    override fun onActivityCreated(savedInstanceState: Bundle?) {
        super.onActivityCreated(savedInstanceState)
        val context = context ?: return

        try {
            // Initialize RtcEngine
            val config = RtcEngineConfig()
            config.mContext = context.applicationContext
            config.mAppId = getString(R.string.agora_app_id)
            config.mChannelProfile = Constants.CHANNEL_PROFILE_LIVE_BROADCASTING
            config.mEventHandler = iRtcEngineEventHandler
            val mainApp = activity?.application as? MainApplication
            config.mAreaCode = mainApp?.globalSettings?.areaCode ?: 0

            engine = RtcEngine.create(config)

            // Set reporting parameters
            engine?.setParameters(
                "{" + "\"rtc.report_app_scenario\":" + "{" + "\"appScenario\":" + 100 + "," + "\"serviceType\":" + 11 + "," + "\"appVersion\":\"" + RtcEngine.getSdkVersion() + "\"" + "}" + "}"
            )

            // Set local access point if configured
            val localAccessPointConfiguration =
                (activity?.application as? MainApplication)?.globalSettings?.privateCloudConfig
            if (localAccessPointConfiguration != null) {
                engine?.setLocalAccessPoint(localAccessPointConfiguration)
            }

            // Use current config values (default or updated from config page)
            // Use the random UID generated at class initialization
            // This same UID will be used for both config.localUid and joinChannel
            val audioSafetyManagerConfig = AudioSafetyManagerConfig(
                context = context,
                rtcEngine = engine!!,
                localUid = myUid,  // Random UID known in advance, will be used for joinChannel
                enableRegisterLocal = currentEnableRegisterLocal,
                bufferDurationSeconds = currentBufferDurationSeconds
            )
            // Initialize AudioSafetyManager
            audioSafetyManager = AudioSafetyManager(audioSafetyManagerConfig)
        } catch (e: Exception) {
            e.printStackTrace()
            activity?.onBackPressed()
        }
    }

    override fun onDestroy() {
        super.onDestroy()

        // Cleanup
        if (engine != null) {
            engine?.leaveChannel()
        }

        audioSafetyManager?.release()
        audioSafetyManager = null

        handler.post {
            RtcEngine.destroy()
        }
        engine = null
    }

    override fun onClick(v: View) {
        when (v.id) {
            R.id.btn_join -> {
                if (!joined) {
                    CommonUtil.hideInputBoard(activity, etChannel)
                    val channelId = etChannel.text.toString()

                    // Check permissions
                    checkOrRequestPermisson(object : PermissonUtils.PermissionResultCallback {
                        override fun onPermissionsResult(
                            allPermissionsGranted: Boolean, permissions: Array<String>, grantResults: IntArray
                        ) {
                            if (allPermissionsGranted) {
                                joinChannel(channelId)
                            }
                        }
                    })
                } else {
                    joined = false
                    engine?.leaveChannel()
                    btnJoin.text = getString(R.string.join)
                    audioSeatManager?.downAllSeats()
                    audioSafetyManager?.stopRecording()
                }
            }
        }
    }

    private fun joinChannel(channelId: String) {
        engine?.setClientRole(Constants.CLIENT_ROLE_BROADCASTER)
        engine?.setDefaultAudioRoutetoSpeakerphone(true)

        // AudioSafetyManager is already initialized and registered before joining channel
        // Use the same UID that was set in config (myUid was set when creating config)
        TokenUtils.gen(requireContext(), channelId, myUid) { token ->
            val option = ChannelMediaOptions()
            option.autoSubscribeAudio = true
            option.autoSubscribeVideo = true

            val res = engine?.joinChannel(token, channelId, myUid, option) ?: -1
            if (res != 0) {
                showAlert(RtcEngine.getErrorDescription(res))
                Log.e(TAG, RtcEngine.getErrorDescription(res))
                return@gen
            }

            btnJoin.isEnabled = false
        }
    }

    private fun setupReportListeners(view: View) {
        // Set click listeners on all audio seats (8 seats)
        val seatIds = listOf(
            R.id.audio_place_01,
            R.id.audio_place_02,
            R.id.audio_place_03,
            R.id.audio_place_04,
            R.id.audio_place_05,
            R.id.audio_place_06,
            R.id.audio_place_07,
            R.id.audio_place_08
        )

        seatIds.forEach { seatId ->
            view.findViewById<AudioOnlyLayout>(seatId)?.let { seat ->
                // Enable clickable and focusable for click events
                seat.isClickable = true
                seat.isFocusable = true

                seat.setOnClickListener { clickedSeat ->
                    val uid = clickedSeat.tag as? Int
                    if (uid == null) {
                        showShortToast("No user in this seat")
                        return@setOnClickListener
                    }

                    if (!joined) {
                        showShortToast("Please join a channel first")
                        return@setOnClickListener
                    }

                    // Allow reporting any user including local user
                    reportUser(uid)
                }
            }
        }
    }

    private fun reportUser(uid: Int) {
        if (!joined) {
            showShortToast("Please join a channel first")
            return
        }

        // Show loading indicator
        showShortToast("Generating audio evidence for user $uid...")

        audioSafetyManager?.reportUser(uid, object : AudioSafetyManager.ReportCallback {
            override fun onSuccess(wavFileUri: String) {
                // Callback is executed in worker thread, post to main thread for UI update
                handler.post {
                    showLongToast("Audio evidence saved: $wavFileUri")
                    Log.d(TAG, "Reported user $uid, WAV file: $wavFileUri")

                    // Here you can upload the WAV file to your moderation service
                    // For example:
                    // uploadToModerationService(wavFileUri, uid)
                }
            }

            override fun onError(error: String) {
                // Callback is executed in worker thread, post to main thread for UI update
                handler.post {
                    showShortToast("Failed to generate audio evidence: $error")
                    Log.e(TAG, "Failed to report user $uid: $error")
                }
            }
        })
    }

    private val iRtcEngineEventHandler = object : IRtcEngineEventHandler() {

        override fun onError(err: Int) {
            Log.w(TAG, String.format("onError code %d message %s", err, RtcEngine.getErrorDescription(err)))
        }

        override fun onJoinChannelSuccess(channel: String, uid: Int, elapsed: Int) {
            Log.i(TAG, String.format("onJoinChannelSuccess channel %s uid %d", channel, uid))
            showShortToast(String.format("Joined channel %s as user %d", channel, uid))

            joined = true

            handler.post {
                btnJoin.isEnabled = true
                btnJoin.text = getString(R.string.leave)

                // Start recording (observer already registered before joining channel)
                audioSafetyManager?.startRecording()

                // Show local user in seat
                audioSeatManager?.upLocalSeat(uid)
            }
        }

        override fun onLeaveChannel(stats: RtcStats) {
            super.onLeaveChannel(stats)
            Log.i(TAG, String.format("local user %d leaveChannel!", myUid))
            showShortToast(String.format("Left channel"))

            handler.post {
                audioSafetyManager?.stopRecording()
            }
        }

        override fun onUserJoined(uid: Int, elapsed: Int) {
            super.onUserJoined(uid, elapsed)
            Log.i(TAG, "onUserJoined->$uid")
            showShortToast(String.format("User %d joined!", uid))

            handler.post {
                audioSeatManager?.upRemoteSeat(uid)
                if (audioSeatManager?.getRemoteSeat(uid) != null) {
                    // Register remote user for audio monitoring
                    audioSafetyManager?.registerUser(uid)
                }
            }
        }

        override fun onUserOffline(uid: Int, reason: Int) {
            Log.i(TAG, String.format("user %d offline! reason:%d", uid, reason))
            showShortToast(String.format("User %d left", uid))

            handler.post {
                audioSeatManager?.downSeat(uid)
                // Unregister user when user leaves
                audioSafetyManager?.unregisterUser(uid)
            }
        }
    }
}
