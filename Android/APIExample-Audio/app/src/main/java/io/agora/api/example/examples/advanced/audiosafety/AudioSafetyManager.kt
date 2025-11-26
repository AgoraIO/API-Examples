package io.agora.api.example.examples.advanced.audiosafety

import android.content.Context
import android.util.Log
import kotlin.jvm.Volatile
import io.agora.rtc2.Constants
import io.agora.rtc2.IAudioFrameObserver
import io.agora.rtc2.RtcEngine
import io.agora.rtc2.audio.AudioParams
import kotlinx.coroutines.CancellationException
import java.io.File
import java.io.FileOutputStream
import java.nio.ByteBuffer
import java.nio.ByteOrder
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.SupervisorJob
import kotlinx.coroutines.cancel
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext
import java.util.concurrent.ConcurrentHashMap
import kotlin.math.min

data class AudioSafetyManagerConfig(
    val context: Context,
    val rtcEngine: RtcEngine,
    val localUid: Int,
    val enableRegisterLocal: Boolean = true,
    val bufferDurationSeconds: Int = 300,
)

/**
 * AudioSafetyManager - Manages audio recording for safety/moderation purposes
 * 
 * Features:
 * - Circular buffer for in-memory audio storage (48kHz, Mono, 16-bit PCM)
 * - Captures audio from remote users via onPlaybackAudioFrameBeforeMixing
 * - Captures local user audio via onRecordAudioFrame
 * - Generates WAV files on demand when reporting a user
 * - Thread-safe operations
 */
class AudioSafetyManager(
    private val config: AudioSafetyManagerConfig
) : IAudioFrameObserver {
    
    companion object {
        private const val TAG = "AudioSafetyManager"
        
        // Audio format constants
        private const val SAMPLE_RATE = 48000
        private const val CHANNELS = 1 // Mono
        private const val BITS_PER_SAMPLE = 16
        private const val BYTES_PER_SAMPLE = BITS_PER_SAMPLE / 8
        
        // Calculate buffer size for specified duration
        private fun calculateBufferSize(durationSeconds: Int): Int {
            // Buffer size = sample_rate * channels * bytes_per_sample * duration_seconds
            return SAMPLE_RATE * CHANNELS * BYTES_PER_SAMPLE * durationSeconds
        }
    }

    @Volatile private var isRecording: Boolean = false
    
    // Thread-safe storage for each user's audio buffer
    private val userBuffers = ConcurrentHashMap<Int, CircularAudioBuffer>()
    
    // Set of registered user IDs that should be monitored
    private val registeredUsers = ConcurrentHashMap.newKeySet<Int>()
    
    // Coroutine scope for writing audio data to memory buffer (high priority, fast)
    // Separate from file I/O to avoid blocking audio data capture
    private val audioWriteScope = CoroutineScope(SupervisorJob() + Dispatchers.Default)
    
    // Coroutine scope for file I/O operations (can be slower, won't block audio capture)
    private val fileProcessingScope = CoroutineScope(SupervisorJob() + Dispatchers.IO)

    init {
        // Register audio frame observer BEFORE joining channel
        val result = config.rtcEngine.registerAudioFrameObserver(this)
        if (result != 0) {
            Log.e(TAG, "Failed to register audio frame observer: $result")
        }

        if (config.enableRegisterLocal){
            registerUser(config.localUid)
        }
        
        // Set audio frame parameters
        // For recording (local user)
        config.rtcEngine.setRecordingAudioFrameParameters(
            SAMPLE_RATE,
            CHANNELS,
            Constants.RAW_AUDIO_FRAME_OP_MODE_READ_ONLY,
            1024
        )
        
        // For playback before mixing (remote users)
        // Note: setPlaybackAudioFrameBeforeMixingParameters is required for onPlaybackAudioFrameBeforeMixing callback
        config.rtcEngine.setPlaybackAudioFrameBeforeMixingParameters(
            SAMPLE_RATE,
            CHANNELS,
            1024
        )
        
        Log.d(TAG, "AudioSafetyManager initialized and registered (before joining channel)")
    }
    
    /**
     * Register a user ID to start monitoring their audio
     * @param uid User ID to monitor
     */
    fun registerUser(uid: Int) {
        registeredUsers.add(uid)
        Log.d(TAG, "Registered user $uid for audio monitoring")
    }
    
    /**
     * Unregister a user ID to stop monitoring their audio
     * @param uid User ID to stop monitoring
     */
    fun unregisterUser(uid: Int) {
        registeredUsers.remove(uid)
        // Optionally remove buffer to free memory
        userBuffers.remove(uid)
        Log.d(TAG, "Unregistered user $uid from audio monitoring")
    }
    
    /**
     * Start recording (called when joining channel)
     */
    fun startRecording() {
        isRecording = true
        Log.d(TAG, "Recording started")
    }
    
    /**
     * Stop recording and cleanup
     * Note: This method should be called from a background thread to avoid blocking
     */
    fun stopRecording() {
        isRecording = false
        // Clear buffers in background to avoid blocking caller thread
        // For large buffers, clearing can take time
        fileProcessingScope.launch {
            userBuffers.values.forEach { it.clear() }
            userBuffers.clear()
            registeredUsers.clear()
            Log.d(TAG, "Recording stopped")
        }
    }
    
    /**
     * Unregister from RtcEngine
     * Should be called when leaving channel or destroying engine
     */
    fun release() {
        stopRecording()
        config.rtcEngine.registerAudioFrameObserver(null)

        // Cancel coroutine scopes
        audioWriteScope.cancel()
        fileProcessingScope.cancel()
        
        Log.d(TAG, "AudioSafetyManager released")
    }
    
    /**
     * Callback interface for report result
     */
    interface ReportCallback {
        fun onSuccess(wavFileUri: String)
        fun onError(error: String)
    }
    
    /**
     * Report a user and generate WAV file (executed in worker thread)
     * @param targetUid User ID to report
     * @param callback Callback to receive result
     */
    fun reportUser(targetUid: Int, callback: ReportCallback) {
        if (!isRecording) {
            callback.onError("Cannot report user: not recording")
            return
        }
        
        // Execute file generation in file processing coroutine scope to avoid blocking audio write operations
        // Multiple reports for the same user can run concurrently - each gets a snapshot of the buffer
        fileProcessingScope.launch {
            try {
                // Check if still recording (may have changed during coroutine execution)
                if (!isRecording) {
                    callback.onError("Recording stopped during report generation")
                    return@launch
                }
                
                val buffer = userBuffers[targetUid]
                if (buffer == null) {
                    callback.onError("No audio data found for user $targetUid")
                    return@launch
                }
                
                // Create a snapshot of the buffer (non-destructive read, can be called multiple times)
                val pcmData = buffer.readAll()
                if (pcmData.isEmpty()) {
                    callback.onError("No audio data available for user $targetUid")
                    return@launch
                }
                
                val wavFile = withContext(Dispatchers.IO) {
                    generateWavFile(targetUid, pcmData)
                }
                Log.d(TAG, "Generated WAV file for user $targetUid: $wavFile")
                callback.onSuccess(wavFile)
            } catch (e: CancellationException) {
                // Scope was cancelled, don't call callback as manager is being released
                Log.d(TAG, "Report generation cancelled for user $targetUid")
                throw e // Re-throw to respect cancellation
            } catch (e: Exception) {
                Log.e(TAG, "Failed to generate WAV file for user $targetUid", e)
                callback.onError("Failed to generate WAV file: ${e.message}")
            }
        }
    }
    
    /**
     * Generate WAV file from PCM data
     */
    private fun generateWavFile(uid: Int, pcmData: ByteArray): String {
        val fileName = "audio_report_${uid}_${System.currentTimeMillis()}.wav"
        val externalFilesDir = config.context.getExternalFilesDir(null)
            ?: throw IllegalStateException("External files directory is not available")
        val file = File(externalFilesDir, fileName)
        
        FileOutputStream(file).use { fos ->
            // Write WAV header
            writeWavHeader(fos, pcmData.size)
            
            // Write PCM data
            fos.write(pcmData)
            fos.flush()
        }
        
        return file.absolutePath
    }
    
    /**
     * Write WAV file header
     */
    private fun writeWavHeader(fos: FileOutputStream, dataSize: Int) {
        val header = ByteBuffer.allocate(44).order(ByteOrder.LITTLE_ENDIAN)
        
        // RIFF header
        header.put("RIFF".toByteArray())
        header.putInt(36 + dataSize) // File size - 8
        header.put("WAVE".toByteArray())
        
        // fmt chunk
        header.put("fmt ".toByteArray())
        header.putInt(16) // fmt chunk size
        header.putShort(1.toShort()) // Audio format (1 = PCM)
        header.putShort(CHANNELS.toShort()) // Number of channels
        header.putInt(SAMPLE_RATE) // Sample rate
        header.putInt(SAMPLE_RATE * CHANNELS * BYTES_PER_SAMPLE) // Byte rate
        header.putShort((CHANNELS * BYTES_PER_SAMPLE).toShort()) // Block align
        header.putShort(BITS_PER_SAMPLE.toShort()) // Bits per sample
        
        // data chunk
        header.put("data".toByteArray())
        header.putInt(dataSize) // Data size
        
        fos.write(header.array())
    }
    
    /**
     * Get or create buffer for a registered user
     * This should only be called from coroutine scope to avoid blocking audio thread
     * 
     * Note: ConcurrentHashMap.getOrPut is thread-safe, but the lambda may be executed
     * multiple times in case of concurrent access. This is acceptable as buffer creation
     * is idempotent and the final result is the same.
     */
    private fun getOrCreateBuffer(uid: Int): CircularAudioBuffer? {
        // Fast path: buffer already exists
        userBuffers[uid]?.let { return it }
        
        // Slow path: create new buffer
        // ConcurrentHashMap.getOrPut is thread-safe, lambda may execute multiple times but result is the same
        return userBuffers.getOrPut(uid) {
            val bufferSize = calculateBufferSize(config.bufferDurationSeconds)
            CircularAudioBuffer(bufferSize)
        }
    }
    
    // IAudioFrameObserver implementation
    
    override fun onRecordAudioFrame(
        channel: String?,
        audioFrameType: Int,
        samples: Int,
        bytesPerSample: Int,
        channels: Int,
        samplesPerSec: Int,
        byteBuffer: ByteBuffer?,
        renderTimeMs: Long,
        bufferLength: Int
    ): Boolean {
        // Early return checks in audio callback to avoid unnecessary operations
        if (!isRecording || byteBuffer == null) {
            return false
        }
        
        val targetUid = config.localUid
        if (targetUid == 0 || !registeredUsers.contains(targetUid)) {
            return false
        }
        
        // Convert to target format if needed (48kHz, Mono, 16-bit)
        if (samplesPerSec != SAMPLE_RATE || channels != CHANNELS || bytesPerSample != BYTES_PER_SAMPLE) {
            // Skip if format doesn't match (or implement resampling/conversion if needed)
            return false
        }
        
        // Fast copy data in audio thread (deep copy to avoid buffer modification)
        // Minimize operations in audio thread to prevent blocking
        val remaining = byteBuffer.remaining()
        if (remaining <= 0) {
            return false
        }
        
        val data = ByteArray(remaining)
        val position = byteBuffer.position()
        byteBuffer.get(data)
        byteBuffer.position(position) // Restore position
        
        // Submit to audio write coroutine scope for processing (non-blocking, fast operation)
        audioWriteScope.launch {
            val buffer = getOrCreateBuffer(targetUid)
            buffer?.write(data, 0, data.size)
        }
        
        return false // Don't modify the audio
    }
    
    override fun onPlaybackAudioFrame(
        channel: String?,
        audioFrameType: Int,
        samples: Int,
        bytesPerSample: Int,
        channels: Int,
        samplesPerSec: Int,
        byteBuffer: ByteBuffer?,
        renderTimeMs: Long,
        bufferLength: Int
    ): Boolean {
        return false
    }
    
    override fun onMixedAudioFrame(
        channel: String?,
        audioFrameType: Int,
        samples: Int,
        bytesPerSample: Int,
        channels: Int,
        samplesPerSec: Int,
        byteBuffer: ByteBuffer?,
        renderTimeMs: Long,
        bufferLength: Int
    ): Boolean {
        return false
    }
    
    override fun onEarMonitoringAudioFrame(
        type: Int,
        samplesPerChannel: Int,
        bytesPerSample: Int,
        channels: Int,
        samplesPerSec: Int,
        buffer: ByteBuffer?,
        renderTimeMs: Long,
        avsync_type: Int
    ): Boolean {
        return false
    }
    
    override fun onPlaybackAudioFrameBeforeMixing(
        channelId: String?,
        uid: Int,
        type: Int,
        samplesPerChannel: Int,
        bytesPerSample: Int,
        channels: Int,
        samplesPerSec: Int,
        buffer: ByteBuffer?,
        renderTimeMs: Long,
        avsync_type: Int,
        rtpTimestamp: Int,
        presentationMs: Long
    ): Boolean {
        // Early return checks in audio callback to avoid unnecessary operations
        if (!isRecording || buffer == null) {
            return false
        }
        
        if (uid == 0 || !registeredUsers.contains(uid)) {
            return false
        }
        
        // Convert to target format if needed (48kHz, Mono, 16-bit)
        if (samplesPerSec != SAMPLE_RATE || channels != CHANNELS || bytesPerSample != BYTES_PER_SAMPLE) {
            // Skip if format doesn't match (or implement resampling/conversion if needed)
            return false
        }
        
        // Fast copy data in audio thread (deep copy to avoid buffer modification)
        // Minimize operations in audio thread to prevent blocking
        val remaining = buffer.remaining()
        if (remaining <= 0) {
            return false
        }
        
        val data = ByteArray(remaining)
        val position = buffer.position()
        buffer.get(data)
        buffer.position(position) // Restore position
        
        // Submit to audio write coroutine scope for processing (non-blocking, fast operation)
        audioWriteScope.launch {
            val audioBuffer = getOrCreateBuffer(uid)
            audioBuffer?.write(data, 0, data.size)
        }
        
        return false // Don't modify the audio
    }
    
    override fun getObservedAudioFramePosition(): Int {
        // Return 0 to observe all audio frame positions
        // Or return bitwise OR of Constants.AUDIO_FRAME_POSITION_* values
        return 0
    }
    
    override fun getRecordAudioParams(): AudioParams? {
        return null // Use parameters set by setRecordingAudioFrameParameters
    }
    
    override fun getPlaybackAudioParams(): AudioParams? {
        return null // Use parameters set by setPlaybackAudioFrameParameters
    }
    
    override fun getMixedAudioParams(): AudioParams? {
        return null
    }
    
    override fun getEarMonitoringAudioParams(): AudioParams? {
        return null
    }

    /**
     * Circular buffer for storing PCM audio data
     * Thread-safe implementation using synchronized blocks
     */
    private class CircularAudioBuffer(val capacity: Int) {
        private val buffer = ByteArray(capacity)
        private var writePosition = 0
        private var isFull = false
        private val lock = Any()

        /**
         * Write audio data to the circular buffer
         * @param data Audio data to write
         * @param offset Offset in data array
         * @param length Length of data to write
         */
        fun write(data: ByteArray, offset: Int, length: Int) {
            synchronized(lock) {
                var remaining = length
                var srcOffset = offset

                while (remaining > 0) {
                    val available = if (isFull) capacity else writePosition
                    val spaceAvailable = capacity - available
                    val toWrite = min(remaining, spaceAvailable)

                    if (toWrite == 0) {
                        // Buffer is full, start overwriting from beginning
                        writePosition = 0
                        isFull = true
                        continue
                    }

                    val endPos = writePosition + toWrite
                    if (endPos <= capacity) {
                        // Simple case: write doesn't wrap around
                        System.arraycopy(data, srcOffset, buffer, writePosition, toWrite)
                        writePosition = endPos
                    } else {
                        // Write wraps around
                        val firstPart = capacity - writePosition
                        System.arraycopy(data, srcOffset, buffer, writePosition, firstPart)
                        System.arraycopy(data, srcOffset + firstPart, buffer, 0, toWrite - firstPart)
                        writePosition = toWrite - firstPart
                    }

                    remaining -= toWrite
                    srcOffset += toWrite

                    if (writePosition == capacity) {
                        writePosition = 0
                        isFull = true
                    }
                }
            }
        }

        /**
         * Read all available audio data from the buffer (snapshot, non-destructive)
         * This method creates a snapshot of the current buffer state without affecting writes
         * Can be called multiple times to get the latest data
         * @return ByteArray containing all audio data (most recent data)
         */
        fun readAll(): ByteArray {
            synchronized(lock) {
                return if (isFull) {
                    // Buffer is full, return all data starting from writePosition
                    // This gives us the most recent data (oldest data is at writePosition)
                    val result = ByteArray(capacity)
                    System.arraycopy(buffer, writePosition, result, 0, capacity - writePosition)
                    System.arraycopy(buffer, 0, result, capacity - writePosition, writePosition)
                    result
                } else {
                    // Buffer not full, return data from 0 to writePosition
                    val result = ByteArray(writePosition)
                    System.arraycopy(buffer, 0, result, 0, writePosition)
                    result
                }
            }
        }

        /**
         * Get the current size of data in the buffer
         */
        fun size(): Int {
            synchronized(lock) {
                return if (isFull) capacity else writePosition
            }
        }

        /**
         * Clear the buffer
         */
        fun clear() {
            synchronized(lock) {
                writePosition = 0
                isFull = false
            }
        }
    }
}
