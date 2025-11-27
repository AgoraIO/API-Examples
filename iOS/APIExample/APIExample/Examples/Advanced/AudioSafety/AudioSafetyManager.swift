//
//  AudioSafetyManager.swift
//  APIExample
//
//  Created by qinhui on 2025/11/26.
//  Copyright © 2025 Agora Corp. All rights reserved.
//

import Foundation
import AgoraRtcKit

/**
 * AudioSafetyManagerConfig - Configuration for AudioSafetyManager
 */
struct AudioSafetyManagerConfig {
    let rtcEngine: AgoraRtcEngineKit
    let localUid: UInt
    let enableRegisterLocal: Bool
    let bufferDurationSeconds: Int
    
    init(rtcEngine: AgoraRtcEngineKit, localUid: UInt, enableRegisterLocal: Bool = true, bufferDurationSeconds: Int = 300) {
        self.rtcEngine = rtcEngine
        self.localUid = localUid
        self.enableRegisterLocal = enableRegisterLocal
        self.bufferDurationSeconds = bufferDurationSeconds
    }
}

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
class AudioSafetyManager: NSObject {
    
    // Audio format constants
    private static let SAMPLE_RATE: Int = 48000
    private static let CHANNELS: Int = 1 // Mono
    private static let BITS_PER_SAMPLE: Int = 16
    private static let BYTES_PER_SAMPLE: Int = BITS_PER_SAMPLE / 8
    
    // Calculate buffer size for specified duration
    private static func calculateBufferSize(durationSeconds: Int) -> Int {
        // Buffer size = sample_rate * channels * bytes_per_sample * duration_seconds
        return SAMPLE_RATE * CHANNELS * BYTES_PER_SAMPLE * durationSeconds
    }
    
    private let config: AudioSafetyManagerConfig
    private var isRecording: Bool = false
    
    // Thread-safe storage for each user's audio buffer
    private let userBuffers = NSMutableDictionary()
    private let bufferLock = NSLock()
    
    // Set of registered user IDs that should be monitored
    private let registeredUsers = NSMutableSet()
    private let registeredUsersLock = NSLock()
    
    // Dispatch queue for writing audio data to memory buffer (high priority, fast)
    // Separate from file I/O to avoid blocking audio data capture
    private let audioWriteQueue = DispatchQueue(label: "com.agora.audiosafety.write", qos: .userInitiated)
    
    // Dispatch queue for file I/O operations (can be slower, won't block audio capture)
    private let fileProcessingQueue = DispatchQueue(label: "com.agora.audiosafety.file", qos: .utility)
    
    init(config: AudioSafetyManagerConfig) {
        self.config = config
        super.init()
        
        // Register audio frame observer BEFORE joining channel
        let result = config.rtcEngine.setAudioFrameDelegate(self)
        if !result {
            print("AudioSafetyManager: Failed to register audio frame observer: \(result)")
        }
        
        if config.enableRegisterLocal {
            registerUser(config.localUid)
        }
        
        // Set audio frame parameters
        // For recording (local user)
        config.rtcEngine.setRecordingAudioFrameParametersWithSampleRate(Self.SAMPLE_RATE, channel: Self.CHANNELS, mode: .readOnly, samplesPerCall: 1024)
                
        // For playback before mixing (remote users)
        // Note: setPlaybackAudioFrameBeforeMixingParameters is required for onPlaybackAudioFrameBeforeMixing callback
        config.rtcEngine.setPlaybackAudioFrameBeforeMixingParametersWithSampleRate(Self.SAMPLE_RATE, channel: Self.CHANNELS, samplesPerCall: 1024)
        
        print("AudioSafetyManager: Initialized and registered (before joining channel)")
    }
    
    /**
     * Register a user ID to start monitoring their audio
     * @param uid User ID to monitor
     */
    func registerUser(_ uid: UInt) {
        registeredUsersLock.lock()
        registeredUsers.add(uid)
        registeredUsersLock.unlock()
        print("AudioSafetyManager: Registered user \(uid) for audio monitoring")
    }
    
    /**
     * Unregister a user ID to stop monitoring their audio
     * @param uid User ID to stop monitoring
     */
    func unregisterUser(_ uid: UInt) {
        registeredUsersLock.lock()
        registeredUsers.remove(uid)
        registeredUsersLock.unlock()
        
        // Optionally remove buffer to free memory
        bufferLock.lock()
        userBuffers.removeObject(forKey: NSNumber(value: uid))
        bufferLock.unlock()
        
        print("AudioSafetyManager: Unregistered user \(uid) from audio monitoring")
    }
    
    /**
     * Start recording (called when joining channel)
     */
    func startRecording() {
        isRecording = true
        print("AudioSafetyManager: Recording started")
    }
    
    /**
     * Stop recording and cleanup
     * Note: This method should be called from a background thread to avoid blocking
     */
    func stopRecording() {
        isRecording = false
        
        // Clear buffers in background to avoid blocking caller thread
        // For large buffers, clearing can take time
        fileProcessingQueue.async { [weak self] in
            guard let self = self else { return }
            
            self.bufferLock.lock()
            self.userBuffers.removeAllObjects()
            self.bufferLock.unlock()
            
            self.registeredUsersLock.lock()
            self.registeredUsers.removeAllObjects()
            self.registeredUsersLock.unlock()
            
            print("AudioSafetyManager: Recording stopped")
        }
    }
    
    /**
     * Unregister from RtcEngine
     * Should be called when leaving channel or destroying engine
     */
    func release() {
        stopRecording()
        config.rtcEngine.setAudioFrameDelegate(nil)
        print("AudioSafetyManager: AudioSafetyManager released")
    }
    
    /**
     * Callback interface for report result
     */
    protocol ReportCallback {
        func onSuccess(wavFileUrl: URL)
        func onError(error: String)
    }
    
    /**
     * Report a user and generate WAV file (executed in worker thread)
     * @param targetUid User ID to report
     * @param callback Callback to receive result
     */
    func reportUser(targetUid: UInt, callback: ReportCallback) {
        if !isRecording {
            callback.onError(error: "Cannot report user: not recording")
            return
        }
        
        // Execute file generation in file processing queue to avoid blocking audio write operations
        // Multiple reports for the same user can run concurrently - each gets a snapshot of the buffer
        fileProcessingQueue.async { [weak self] in
            guard let self = self else { return }
            
            // Check if still recording (may have changed during async execution)
            if !self.isRecording {
                callback.onError(error: "Recording stopped during report generation")
                return
            }
            
            self.bufferLock.lock()
            let buffer = self.userBuffers[NSNumber(value: targetUid)] as? CircularAudioBuffer
            self.bufferLock.unlock()
            
            guard let audioBuffer = buffer else {
                callback.onError(error: "No audio data found for user \(targetUid)")
                return
            }
            
            // Create a snapshot of the buffer (non-destructive read, can be called multiple times)
            let pcmData = audioBuffer.readAll()
            if pcmData.isEmpty {
                callback.onError(error: "No audio data available for user \(targetUid)")
                return
            }
            
            do {
                let wavFileUrl = try self.generateWavFile(uid: targetUid, pcmData: pcmData)
                print("AudioSafetyManager: Generated WAV file for user \(targetUid): \(wavFileUrl.path)")
                callback.onSuccess(wavFileUrl: wavFileUrl)
            } catch {
                print("AudioSafetyManager: Failed to generate WAV file for user \(targetUid): \(error)")
                callback.onError(error: "Failed to generate WAV file: \(error.localizedDescription)")
            }
        }
    }
    
    /**
     * Generate WAV file from PCM data
     */
    private func generateWavFile(uid: UInt, pcmData: Data) throws -> URL {
        let fileName = "audio_report_\(uid)_\(Int64(Date().timeIntervalSince1970 * 1000)).wav"
        
        let fileManager = FileManager.default
        let documentsPath = fileManager.urls(for: .documentDirectory, in: .userDomainMask)[0]
        let fileUrl = documentsPath.appendingPathComponent(fileName)
        
        // Create WAV header
        let header = createWavHeader(dataSize: pcmData.count)
        
        // Combine header and PCM data
        var wavData = Data()
        wavData.append(header)
        wavData.append(pcmData)
        
        // Write to file using Data.write(to:options:) which is available in all iOS versions
        try wavData.write(to: fileUrl, options: .atomic)
        
        return fileUrl
    }
    
    /**
     * Create WAV file header
     */
    private func createWavHeader(dataSize: Int) -> Data {
        var header = Data(capacity: 44)
        
        // RIFF header
        header.append("RIFF".data(using: .ascii)!)
        header.append(contentsOf: withUnsafeBytes(of: UInt32(36 + dataSize).littleEndian) { Data($0) }) // File size - 8
        header.append("WAVE".data(using: .ascii)!)
        
        // fmt chunk
        header.append("fmt ".data(using: .ascii)!)
        header.append(contentsOf: withUnsafeBytes(of: UInt32(16).littleEndian) { Data($0) }) // fmt chunk size
        header.append(contentsOf: withUnsafeBytes(of: UInt16(1).littleEndian) { Data($0) }) // Audio format (1 = PCM)
        header.append(contentsOf: withUnsafeBytes(of: UInt16(Self.CHANNELS).littleEndian) { Data($0) }) // Number of channels
        header.append(contentsOf: withUnsafeBytes(of: UInt32(Self.SAMPLE_RATE).littleEndian) { Data($0) }) // Sample rate
        header.append(contentsOf: withUnsafeBytes(of: UInt32(Self.SAMPLE_RATE * Self.CHANNELS * Self.BYTES_PER_SAMPLE).littleEndian) { Data($0) }) // Byte rate
        header.append(contentsOf: withUnsafeBytes(of: UInt16(Self.CHANNELS * Self.BYTES_PER_SAMPLE).littleEndian) { Data($0) }) // Block align
        header.append(contentsOf: withUnsafeBytes(of: UInt16(Self.BITS_PER_SAMPLE).littleEndian) { Data($0) }) // Bits per sample
        
        // data chunk
        header.append("data".data(using: .ascii)!)
        header.append(contentsOf: withUnsafeBytes(of: UInt32(dataSize).littleEndian) { Data($0) }) // Data size
        
        return header
    }
    
    /**
     * Get or create buffer for a registered user
     * This should only be called from audio write queue to avoid blocking audio thread
     */
    private func getOrCreateBuffer(uid: UInt) -> CircularAudioBuffer? {
        let key = NSNumber(value: uid)
        
        bufferLock.lock()
        defer { bufferLock.unlock() }
        
        // Fast path: buffer already exists
        if let existingBuffer = userBuffers[key] as? CircularAudioBuffer {
            return existingBuffer
        }
        
        // Slow path: create new buffer
        let bufferSize = Self.calculateBufferSize(durationSeconds: config.bufferDurationSeconds)
        let newBuffer = CircularAudioBuffer(capacity: bufferSize)
        userBuffers[key] = newBuffer
        
        return newBuffer
    }
}

// MARK: - AgoraAudioFrameDelegate
extension AudioSafetyManager: AgoraAudioFrameDelegate {
    
    func onRecordAudioFrame(_ frame: AgoraAudioFrame, channelId: String) -> Bool {
        // Early return checks in audio callback to avoid unnecessary operations
        if !isRecording {
            return false
        }
        
        let targetUid = config.localUid
        if targetUid == 0 {
            return false
        }
        
        registeredUsersLock.lock()
        let isRegistered = registeredUsers.contains(NSNumber(value: targetUid))
        registeredUsersLock.unlock()
        
        if !isRegistered {
            return false
        }
        
        // Convert to target format if needed (48kHz, Mono, 16-bit)
        if frame.samplesPerSec != Self.SAMPLE_RATE || frame.channels != Self.CHANNELS || frame.bytesPerSample != Self.BYTES_PER_SAMPLE {
            // Skip if format doesn't match (or implement resampling/conversion if needed)
            return false
        }
        
        // Fast copy data in audio thread (deep copy to avoid buffer modification)
        // Minimize operations in audio thread to prevent blocking
        // Calculate buffer length: samplesPerChannel × channels × bytesPerSample
        let bufferLength = frame.samplesPerChannel * frame.channels * frame.bytesPerSample
        guard let buffer = frame.buffer, bufferLength > 0 else {
            return false
        }
        
        let data = Data(bytes: buffer, count: bufferLength)
        
        // Submit to audio write queue for processing (non-blocking, fast operation)
        audioWriteQueue.async { [weak self] in
            guard let self = self else { return }
            let audioBuffer = self.getOrCreateBuffer(uid: targetUid)
            audioBuffer?.write(data: data)
        }
        
        return false // Don't modify the audio
    }
    
    func onPlaybackAudioFrame(_ frame: AgoraAudioFrame, channelId: String) -> Bool {
        return false
    }
    
    func onMixedAudioFrame(_ frame: AgoraAudioFrame, channelId: String) -> Bool {
        return false
    }
    
    func onEarMonitoringAudioFrame(_ frame: AgoraAudioFrame) -> Bool {
        return false
    }
    
    func onPlaybackAudioFrame(beforeMixing frame: AgoraAudioFrame, channelId: String, uid: UInt) -> Bool {
        // Early return checks in audio callback to avoid unnecessary operations
        if !isRecording {
            return false
        }
        
        if uid == 0 {
            return false
        }
        
        registeredUsersLock.lock()
        let isRegistered = registeredUsers.contains(NSNumber(value: uid))
        registeredUsersLock.unlock()
        
        if !isRegistered {
            return false
        }
        
        // Convert to target format if needed (48kHz, Mono, 16-bit)
        if frame.samplesPerSec != Self.SAMPLE_RATE || frame.channels != Self.CHANNELS || frame.bytesPerSample != Self.BYTES_PER_SAMPLE {
            // Skip if format doesn't match (or implement resampling/conversion if needed)
            return false
        }
        
        // Fast copy data in audio thread (deep copy to avoid buffer modification)
        // Minimize operations in audio thread to prevent blocking
        // Calculate buffer length: samplesPerChannel × channels × bytesPerSample
        let bufferLength = frame.samplesPerChannel * frame.channels * frame.bytesPerSample
        guard let buffer = frame.buffer, bufferLength > 0 else {
            return false
        }
        
        let data = Data(bytes: buffer, count: bufferLength)
        
        // Submit to audio write queue for processing (non-blocking, fast operation)
        audioWriteQueue.async { [weak self] in
            guard let self = self else { return }
            let audioBuffer = self.getOrCreateBuffer(uid: uid)
            audioBuffer?.write(data: data)
        }
        
        return false // Don't modify the audio
    }
    
    func getObservedAudioFramePosition() -> AgoraAudioFramePosition {
        // Return bitwise OR of AgoraAudioFramePosition values to observe specific positions
        // We need .record for local audio and .beforeMixing for remote audio
        return [.record, .beforeMixing]
    }
    
    func getRecordAudioParams() -> AgoraAudioParams {
        // Use parameters set by setRecordingAudioFrameParameters
        return AgoraAudioParams()
    }
    
    func getPlaybackAudioParams() -> AgoraAudioParams {
        // Use parameters set by setPlaybackAudioFrameParameters
        return AgoraAudioParams()
    }
    
    func getMixedAudioParams() -> AgoraAudioParams {
        return AgoraAudioParams()
    }
    
    func getEarMonitoringAudioParams() -> AgoraAudioParams {
        return AgoraAudioParams()
    }
}

/**
 * Circular buffer for storing PCM audio data
 * Thread-safe implementation using NSLock
 */
private class CircularAudioBuffer {
    private let buffer: UnsafeMutablePointer<UInt8>
    private let capacity: Int
    private var writePosition: Int = 0
    private var isFull: Bool = false
    private let lock = NSLock()
    
    init(capacity: Int) {
        self.capacity = capacity
        self.buffer = UnsafeMutablePointer<UInt8>.allocate(capacity: capacity)
        self.buffer.initialize(repeating: 0, count: capacity)
    }
    
    deinit {
        buffer.deinitialize(count: capacity)
        buffer.deallocate()
    }
    
    /**
     * Write audio data to the circular buffer
     * @param data Audio data to write
     */
    func write(data: Data) {
        lock.lock()
        defer { lock.unlock() }
        
        var remaining = data.count
        var srcOffset = 0
        
        while remaining > 0 {
            let available = isFull ? capacity : writePosition
            let spaceAvailable = capacity - available
            let toWrite = min(remaining, spaceAvailable)
            
            if toWrite == 0 {
                // Buffer is full, start overwriting from beginning
                writePosition = 0
                isFull = true
                continue
            }
            
            let endPos = writePosition + toWrite
            if endPos <= capacity {
                // Simple case: write doesn't wrap around
                data.withUnsafeBytes { bytes in
                    let srcPtr = bytes.baseAddress!.advanced(by: srcOffset)
                    let dstPtr = buffer.advanced(by: writePosition)
                    memcpy(dstPtr, srcPtr, toWrite)
                }
                writePosition = endPos
            } else {
                // Write wraps around
                let firstPart = capacity - writePosition
                data.withUnsafeBytes { bytes in
                    let srcPtr = bytes.baseAddress!.advanced(by: srcOffset)
                    let dstPtr1 = buffer.advanced(by: writePosition)
                    memcpy(dstPtr1, srcPtr, firstPart)
                    
                    let srcPtr2 = bytes.baseAddress!.advanced(by: srcOffset + firstPart)
                    memcpy(buffer, srcPtr2, toWrite - firstPart)
                }
                writePosition = toWrite - firstPart
            }
            
            remaining -= toWrite
            srcOffset += toWrite
            
            if writePosition == capacity {
                writePosition = 0
                isFull = true
            }
        }
    }
    
    /**
     * Read all available audio data from the buffer (snapshot, non-destructive)
     * This method creates a snapshot of the current buffer state without affecting writes
     * Can be called multiple times to get the latest data
     * @return Data containing all audio data (most recent data)
     */
    func readAll() -> Data {
        lock.lock()
        defer { lock.unlock() }
        
        if isFull {
            // Buffer is full, return all data starting from writePosition
            // This gives us the most recent data (oldest data is at writePosition)
            var result = Data(count: capacity)
            result.withUnsafeMutableBytes { resultBytes in
                let firstPart = capacity - writePosition
                memcpy(resultBytes.baseAddress!, buffer.advanced(by: writePosition), firstPart)
                memcpy(resultBytes.baseAddress!.advanced(by: firstPart), buffer, writePosition)
            }
            return result
        } else {
            // Buffer not full, return data from 0 to writePosition
            var result = Data(count: writePosition)
            result.withUnsafeMutableBytes { resultBytes in
                memcpy(resultBytes.baseAddress!, buffer, writePosition)
            }
            return result
        }
    }
    
    /**
     * Get the current size of data in the buffer
     */
    func size() -> Int {
        lock.lock()
        defer { lock.unlock() }
        return isFull ? capacity : writePosition
    }
    
    /**
     * Clear the buffer
     */
    func clear() {
        lock.lock()
        defer { lock.unlock() }
        writePosition = 0
        isFull = false
    }
}
