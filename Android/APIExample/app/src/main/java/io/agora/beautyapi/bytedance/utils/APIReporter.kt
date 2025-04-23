package io.agora.beautyapi.bytedance.utils

import io.agora.rtc2.RtcEngine
import org.json.JSONObject
import java.util.concurrent.Executors
import java.lang.ref.WeakReference

enum class APIType(val value: Int) {
    KTV(1),             // Karaoke
    CALL(2),            // Call/Co-hosting
    BEAUTY(3),          // Beauty
    VIDEO_LOADER(4),    // Instant Loading
    PK(5),              // Team Battle
    VIRTUAL_SPACE(6),   // Virtual Space
    SCREEN_SPACE(7),    // Screen Sharing
    AUDIO_SCENARIO(8)   // Audio
}

enum class ApiEventType(val value: Int) {
    API(0),
    COST(1),
    CUSTOM(2)
}

object ApiEventKey {
    const val TYPE = "type"
    const val DESC = "desc"
    const val API_VALUE = "apiValue"
    const val TIMESTAMP = "ts"
    const val EXT = "ext"
}

object ApiCostEvent {
    const val CHANNEL_USAGE = "channelUsage"                 // Channel usage duration
    const val FIRST_FRAME_ACTUAL = "firstFrameActual"        // Actual first frame duration
    const val FIRST_FRAME_PERCEIVED = "firstFramePerceived"  // Perceived first frame duration
}

class APIReporter(
    private val type: APIType,
    private val version: String,
    rtcEngine: RtcEngine
) {
    private val tag = "APIReporter"
    private val messageId = "agora:scenarioAPI"
    private val durationEventStartMap = HashMap<String, Long>()
    private val category = "${type.value}_Android_$version"
    private val executorService = Executors.newSingleThreadExecutor()
    private val rtcEngineRef = WeakReference(rtcEngine)

    init {
        configParameters()
    }

    // Report regular scenario API
    fun reportFuncEvent(name: String, value: Map<String, Any>, ext: Map<String, Any>) {
        executorService.submit {
            rtcEngineRef.get()?.let {
                val eventMap = mapOf(ApiEventKey.TYPE to ApiEventType.API.value, ApiEventKey.DESC to name)
                val labelMap = mapOf(
                    ApiEventKey.API_VALUE to value,
                    ApiEventKey.TIMESTAMP to getCurrentTs(),
                    ApiEventKey.EXT to ext
                )
                val event = convertToJSONString(eventMap) ?: ""
                val label = convertToJSONString(labelMap) ?: ""
                it.sendCustomReportMessage(messageId, category, event, label, 0)
            }
        }
    }

    fun startDurationEvent(name: String) {
        durationEventStartMap[name] = getCurrentTs()
    }

    fun endDurationEvent(name: String, ext: Map<String, Any>) {
        val beginTs = durationEventStartMap[name] ?: return
        durationEventStartMap.remove(name)
        val ts = getCurrentTs()
        val cost = (ts - beginTs).toInt()

        innerReportCostEvent(ts, name, cost, ext)
    }

    // Report time-consuming event point information
    fun reportCostEvent(name: String, cost: Int, ext: Map<String, Any>) {
        durationEventStartMap.remove(name)
        innerReportCostEvent(
            ts = getCurrentTs(),
            name = name,
            cost = cost,
            ext = ext
        )
    }

    // Report custom information
    fun reportCustomEvent(name: String, ext: Map<String, Any>) {
        executorService.submit {
            rtcEngineRef.get()?.let {
                val eventMap = mapOf(ApiEventKey.TYPE to ApiEventType.CUSTOM.value, ApiEventKey.DESC to name)
                val labelMap = mapOf(ApiEventKey.TIMESTAMP to getCurrentTs(), ApiEventKey.EXT to ext)
                val event = convertToJSONString(eventMap) ?: ""
                val label = convertToJSONString(labelMap) ?: ""
                it.sendCustomReportMessage(messageId, category, event, label, 0)
            }
        }
    }

    private fun writeLog(content: String, level: Int) {
        rtcEngineRef.get()?.writeLog(level, content)
    }

    fun cleanCache() {
        durationEventStartMap.clear()
    }

    // ---------------------- private ----------------------

    private fun configParameters() {
        executorService.submit {
            rtcEngineRef.get()?.let {
                // it.setParameters("{\"rtc.qos_for_test_purpose\": true}") // Used for test environment
                // Data reporting
                it.setParameters("{\"rtc.direct_send_custom_event\": true}")
                // Log writing
                it.setParameters("{\"rtc.log_external_input\": true}")
            }
        }
    }

    private fun getCurrentTs(): Long {
        return System.currentTimeMillis()
    }

    private fun innerReportCostEvent(ts: Long, name: String, cost: Int, ext: Map<String, Any>) {
        executorService.submit {
            rtcEngineRef.get()?.let {
//                writeLog("reportCostEvent: $name cost: $cost ms", Constants.LOG_LEVEL_INFO)
                val eventMap = mapOf(ApiEventKey.TYPE to ApiEventType.COST.value, ApiEventKey.DESC to name)
                val labelMap = mapOf(ApiEventKey.TIMESTAMP to ts, ApiEventKey.EXT to ext)
                val event = convertToJSONString(eventMap) ?: ""
                val label = convertToJSONString(labelMap) ?: ""
                it.sendCustomReportMessage(messageId, category, event, label, cost)
            }
        }
    }

    private fun convertToJSONString(dictionary: Map<String, Any>): String? {
        return try {
            JSONObject(dictionary).toString()
        } catch (e: Exception) {
            LogUtils.e(tag, "convert to json fail: $e dictionary: $dictionary")
            null
        }
    }
}