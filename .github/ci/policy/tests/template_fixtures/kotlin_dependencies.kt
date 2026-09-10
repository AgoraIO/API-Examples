// Controlled Android/SDK doubles. No network, native library or Compose runtime is used.
class Context { val applicationContext get() = this }
object Looper { fun getMainLooper() = this; fun myLooper() = this }
class Handler(looper: Looper) {
    fun post(block: () -> Unit) { queue.add(block) }
    companion object {
        val queue = ArrayDeque<() -> Unit>()
        fun drain() { while (queue.isNotEmpty()) queue.removeFirst()() }
    }
}
object BuildConfig { const val AGORA_APP_ID = "" }
object SettingPreferences { fun getArea() = 0 }
object AgoraConfig { var certificate = ""; fun getAppCertificate() = certificate }
object TokenUtils {
    data class Request(val channel: String, val uid: Int, val callback: (String?) -> Unit)
    val requests = mutableListOf<Request>()
    fun genToken(channel: String, uid: Int, callback: (String?) -> Unit) {
        requests.add(Request(channel, uid, callback))
    }
}
open class IRtcEngineEventHandler {
    class RtcStats
    open fun onJoinChannelSuccess(channel: String?, uid: Int, elapsed: Int) {}
    open fun onLeaveChannel(stats: RtcStats?) {}
}
typealias RtcStats = IRtcEngineEventHandler.RtcStats
class RtcEngineConfig {
    var mAreaCode = 0
    var mContext: Context? = null
    var mAppId = ""
    var mEventHandler: IRtcEngineEventHandler? = null
}
class ChannelMediaOptions { var publishMicrophoneTrack = false }
class RtcEngine(val config: RtcEngineConfig) {
    var destroyed = false
    var leaves = 0
    val joins = mutableListOf<Pair<String, Int>>()
    fun joinChannel(token: String?, channel: String, uid: Int, options: ChannelMediaOptions): Int {
        check(!destroyed) { "join after destroy" }
        joins.add(channel to uid)
        return joinResult
    }
    fun leaveChannel() { check(!destroyed); leaves++ }
    companion object {
        val instances = mutableListOf<RtcEngine>()
        var destroys = 0
        var failCreation = false
        var joinResult = 0
        fun create(config: RtcEngineConfig): RtcEngine {
            if (failCreation) throw Exception("controlled initialization failure")
            return RtcEngine(config).also { instances.add(it) }
        }
        fun destroy() { destroys++; instances.last().destroyed = true }
    }
}
