//
//  AgoraAudioTube.m
//  Agora-Screen-Sharing-iOS-Broadcast
//
//  Created by CavanSu on 2019/12/4.
//  Copyright © 2019 Agora. All rights reserved.
//

#import <CoreMedia/CoreMedia.h>
#import "AgoraAudioTube.h"
#import "AgoraAudioProcessing.h"
#include "external_resampler.h"

#pragma mark - Audio Buffer
const int bufferSamples = 48000 * 8;
size_t dataPointerSize = bufferSamples;
int16_t dataPointer[bufferSamples];
int16_t appAudio[bufferSamples];
int16_t micAudio[bufferSamples];
int64_t appAudioIndex = 0;
int64_t micAudioIndex = 0;

int16_t mixPushAudio[bufferSamples];

#pragma mark - Resample
int resampleApp(int16_t* sourceBuffer,
                size_t sourceBufferSize,
                size_t totalSamples,
                int inDataSamplesPer10ms,
                int outDataSamplesPer10ms,
                int channels,
                int sampleRate,
                int resampleRate);

int resampleMic(int16_t* sourceBuffer,
                size_t sourceBufferSize,
                size_t totalSamples,
                int inDataSamplesPer10ms,
                int outDataSamplesPer10ms,
                int channels,
                int sampleRate,
                int resampleRate);

static external_resampler* resamplerAppLeft;
static external_resampler* resamplerAppRight;
static external_resampler* resampleMicLeft;
static external_resampler* resampleMicRight;

// App
int16_t inLeftAppResampleBuffer[bufferSamples];
int16_t inRightAppResampleBuffer[bufferSamples];

int inLeftAppResampleBufferIndex = 0;
int inRightAppResampleBufferIndex = 0;

// Mic
int16_t inLeftMicResampleBuffer[bufferSamples];
int16_t inRightMicResampleBuffer[bufferSamples];

int inLeftMicResampleBufferIndex = 0;
int inRightMicResampleBufferIndex = 0;

// Resample Out Buffer
int16_t outLeftResampleBuffer[bufferSamples];
int16_t outRightResampleBuffer[bufferSamples];

int outLeftResampleBufferIndex = 0;
int outRightResampleBufferIndex = 0;

static NSObject *lock = [[NSObject alloc] init];

@implementation AgoraAudioTube

+ (void)agoraKit:(AgoraRtcEngineKit * _Nonnull)agoraKit pushAudioCMSampleBuffer:(CMSampleBufferRef _Nonnull)sampleBuffer resampleRate:(NSUInteger)resampleRate type:(AudioType)type; {
    @synchronized (lock) {
        [self privateAgoraKit:agoraKit
      pushAudioCMSampleBuffer:sampleBuffer
                 resampleRate:resampleRate
                         type:type];
    }
}

+ (void)privateAgoraKit:(AgoraRtcEngineKit * _Nonnull)agoraKit pushAudioCMSampleBuffer:(CMSampleBufferRef _Nonnull)sampleBuffer resampleRate:(NSUInteger)resampleRate type:(AudioType)type {
    CFRetain(sampleBuffer);
    
    OSStatus err = noErr;
    
    CMBlockBufferRef audioBuffer = CMSampleBufferGetDataBuffer(sampleBuffer);
    if (audioBuffer == nil) {
        CFRelease(sampleBuffer);
        return;
    }
    
    size_t lengthAtOffset;
    size_t totalBytes;
    char *samples;
    
    err = CMBlockBufferGetDataPointer(audioBuffer,
                                      0,
                                      &lengthAtOffset,
                                      &totalBytes,
                                      &samples);
    
    if (totalBytes == 0 || err != noErr) {
        CFRelease(sampleBuffer);
        return;
    }
    
    CMAudioFormatDescriptionRef format = CMSampleBufferGetFormatDescription(sampleBuffer);
    const AudioStreamBasicDescription *description = CMAudioFormatDescriptionGetStreamBasicDescription(format);
    
    memset(dataPointer, 0, sizeof(int16_t) * bufferSamples);
    
    err = CMBlockBufferCopyDataBytes(audioBuffer,
                                     0,
                                     totalBytes,
                                     dataPointer);
    
    if (err != noErr) {
        CFRelease(sampleBuffer);
        return;
    }
    
    size_t totalSamples = totalBytes / (description->mBitsPerChannel / 8);
    UInt32 channels = description->mChannelsPerFrame;
    Float64 sampleRate = description->mSampleRate;
    
    // float to int
    if (description->mFormatFlags & kAudioFormatFlagIsFloat) {
        float* floatData = (float*)dataPointer;
        int16_t* intData = (int16_t*)dataPointer;
        for (int i = 0; i < totalSamples; i++) {
            float tmp = floatData[i] * 32767;
            intData[i] = (tmp >= 32767) ?  32767 : tmp;
            intData[i] = (tmp < -32767) ? -32767 : tmp;
        }
    }
    
    // big endian to little endian
    if (description->mFormatFlags & kAudioFormatFlagIsBigEndian) {
        uint8_t* p = (uint8_t*)dataPointer;
        for (int i = 0; i < totalBytes; i += 2) {
            uint8_t tmp;
            tmp = p[i];
            p[i] = p[i + 1];
            p[i + 1] = tmp;
        }
    }
    
    // rearrange left and right channels
    if ((description->mFormatFlags & kAudioFormatFlagIsNonInterleaved) && channels == 2) {
        int16_t* intData = (int16_t*)dataPointer;
        int16_t newBuffer[totalSamples];
        for (int i = 0; i < totalSamples / 2; i++) {
            newBuffer[2 * i] = intData[i];
            newBuffer[2 * i + 1] = intData[totalSamples / 2 + i];
        }
        memcpy(dataPointer, newBuffer, sizeof(int16_t) * totalSamples);
    }
    
    // mono to stereo
    if (channels == 1) {
        int16_t* intData = (int16_t*)dataPointer;
        int16_t newBuffer[totalSamples * 2];
        
        if ((totalSamples * sizeof(int16_t)) > dataPointerSize) {
            NSLog(@"totalSamples size: %lu", (totalSamples * sizeof(int16_t)));
        }
        
        for (int i = 0; i < totalSamples; i++) {
            newBuffer[2 * i] = intData[i];
            newBuffer[2 * i + 1] = intData[i];
        }
        totalSamples *= 2;
        memcpy(dataPointer, newBuffer, sizeof(int16_t) * totalSamples);
        totalBytes *= 2;
        channels = 2;
    }
    
    // ResampleRate
    if (sampleRate != resampleRate) {
        int inDataSamplesPer10ms = sampleRate / 100;
        int outDataSamplesPer10ms = (int)resampleRate / 100;
        
        int16_t* intData = (int16_t*)dataPointer;
        
        switch (type) {
            case AudioTypeApp:
                totalSamples = resampleApp(intData, dataPointerSize, totalSamples,
                                           inDataSamplesPer10ms, outDataSamplesPer10ms, channels, sampleRate, (int)resampleRate);
                break;
            case AudioTypeMic:
                totalSamples = resampleMic(intData, dataPointerSize, totalSamples,
                                           inDataSamplesPer10ms, outDataSamplesPer10ms, channels, sampleRate, (int)resampleRate);
                break;
        }
        
        totalBytes = totalSamples * sizeof(int16_t);
    }
    
    switch (type) {
        case AudioTypeApp: {
            memcpy(appAudio + appAudioIndex, dataPointer, totalBytes);
            appAudioIndex += totalSamples;
            
            int64_t mixIndex = appAudioIndex > micAudioIndex ? micAudioIndex : appAudioIndex;
            
            int16_t pushBuffer[appAudioIndex];
            
            memcpy(pushBuffer, appAudio, appAudioIndex * sizeof(int16_t));
            
            for (int i = 0; i < mixIndex; i ++) {
                pushBuffer[i] = (appAudio[i] + micAudio[i]) / 2;
            }
            
            [AgoraAudioProcessing pushAudioFrame:(unsigned char *)pushBuffer
                                   withFrameSize:appAudioIndex * sizeof(int16_t)];
            
            memset(appAudio, 0, bufferSamples * sizeof(int16_t));
            appAudioIndex = 0;
            
            memmove(micAudio, micAudio + mixIndex, (bufferSamples - mixIndex) * sizeof(int16_t));
            micAudioIndex -= mixIndex;
        }
            break;
        case AudioTypeMic: {
            memcpy(micAudio + micAudioIndex, dataPointer, totalBytes);
            micAudioIndex += totalSamples;
        }
            break;
    }
    
    CFRelease(sampleBuffer);
}

int resampleApp(int16_t* sourceBuffer, size_t sourceBufferSize, size_t totalSamples, int inDataSamplesPer10ms, int outDataSamplesPer10ms, int channels, int sampleRate, int resampleRate)
{
    int16_t* intData = (int16_t*)sourceBuffer;
    for (int i = 0; i < totalSamples; i ++) {
        if (i % 2) {
            inRightAppResampleBuffer[inRightAppResampleBufferIndex] = intData[i];
            inRightAppResampleBufferIndex ++;
        } else {
            inLeftAppResampleBuffer[inLeftAppResampleBufferIndex] = intData[i];
            inLeftAppResampleBufferIndex ++;
        }
    }
    
    if (!resamplerAppLeft) {
        resamplerAppLeft = new external_resampler();
    }
    
    if (!resamplerAppRight) {
        resamplerAppRight = new external_resampler();
    }
    
    int pPos = 0;
    
    // App Right
    while (inRightAppResampleBufferIndex > inDataSamplesPer10ms) {
        resamplerAppRight->do_resample(inRightAppResampleBuffer + pPos,
                                       inDataSamplesPer10ms,
                                       channels / 2,
                                       sampleRate,
                                       
                                       outRightResampleBuffer + outRightResampleBufferIndex,
                                       outDataSamplesPer10ms,
                                       channels / 2,
                                       (int)resampleRate);
        
        pPos += inDataSamplesPer10ms;
        inRightAppResampleBufferIndex -= inDataSamplesPer10ms;
        outRightResampleBufferIndex += outDataSamplesPer10ms;
    }
    
    memmove(inRightAppResampleBuffer,
            inRightAppResampleBuffer + pPos,
            sizeof(int16_t) * (bufferSamples - pPos));
    
    // App Left
    pPos = 0;
    
    while (inLeftAppResampleBufferIndex > inDataSamplesPer10ms) {
        resamplerAppLeft->do_resample(inLeftAppResampleBuffer + pPos,
                                      inDataSamplesPer10ms,
                                      channels / 2,
                                      sampleRate,
                                      
                                      outLeftResampleBuffer + outLeftResampleBufferIndex,
                                      outDataSamplesPer10ms,
                                      channels / 2,
                                      (int)resampleRate);
        
        pPos += inDataSamplesPer10ms;
        inLeftAppResampleBufferIndex -= inDataSamplesPer10ms;
        outLeftResampleBufferIndex += outDataSamplesPer10ms;
    }
    
    memmove(inLeftAppResampleBuffer,
            inLeftAppResampleBuffer + pPos,
            sizeof(int16_t) * (bufferSamples - pPos));
    
    memset(intData, 0, sourceBufferSize);
    
    for (int i = 0; i < outRightResampleBufferIndex; i ++) {
        intData[2 * i] = outRightResampleBuffer[i];
        intData[2 * i + 1] = outLeftResampleBuffer[i];
    }
    
    int samples = outLeftResampleBufferIndex * 2;
    // Reset
    outLeftResampleBufferIndex = 0;
    outRightResampleBufferIndex = 0;
    
    return samples;
}

int resampleMic(int16_t* sourceBuffer, size_t sourceBufferSize, size_t totalSamples, int inDataSamplesPer10ms, int outDataSamplesPer10ms, int channels, int sampleRate, int resampleRate)
{
    int16_t* intData = (int16_t*)sourceBuffer;
    for (int i = 0; i < totalSamples; i ++) {
        if (i % 2) {
            inRightMicResampleBuffer[inRightMicResampleBufferIndex] = intData[i];
            inRightMicResampleBufferIndex ++;
        } else {
            inLeftMicResampleBuffer[inLeftMicResampleBufferIndex] = intData[i];
            inLeftMicResampleBufferIndex ++;
        }
    }
    
    if (!resampleMicLeft) {
        resampleMicLeft = new external_resampler();
    }
    
    if (!resampleMicRight) {
        resampleMicRight = new external_resampler();
    }
    
    int pPos = 0;
    
    // App Right
    while (inRightMicResampleBufferIndex > inDataSamplesPer10ms) {
        resampleMicRight->do_resample(inRightMicResampleBuffer + pPos,
                                      inDataSamplesPer10ms,
                                      channels / 2,
                                      sampleRate,
                                      
                                      outRightResampleBuffer + outRightResampleBufferIndex,
                                      outDataSamplesPer10ms,
                                      channels / 2,
                                      (int)resampleRate);
        
        pPos += inDataSamplesPer10ms;
        inRightMicResampleBufferIndex -= inDataSamplesPer10ms;
        outRightResampleBufferIndex += outDataSamplesPer10ms;
    }
    
    memmove(inRightMicResampleBuffer,
            inRightMicResampleBuffer + pPos,
            sizeof(int16_t) * (bufferSamples - pPos));
    
    // App Left
    pPos = 0;
    
    while (inLeftMicResampleBufferIndex > inDataSamplesPer10ms) {
        resampleMicLeft->do_resample(inLeftMicResampleBuffer + pPos,
                                     inDataSamplesPer10ms,
                                     channels / 2,
                                     sampleRate,
                                     
                                     outLeftResampleBuffer + outLeftResampleBufferIndex,
                                     outDataSamplesPer10ms,
                                     channels / 2,
                                     (int)resampleRate);
        
        pPos += inDataSamplesPer10ms;
        inLeftMicResampleBufferIndex -= inDataSamplesPer10ms;
        outLeftResampleBufferIndex += outDataSamplesPer10ms;
    }
    
    memmove(inLeftMicResampleBuffer,
            inLeftMicResampleBuffer + pPos,
            sizeof(int16_t) * (bufferSamples - pPos));
    
    memset(intData, 0, sourceBufferSize);
    
    for (int i = 0; i < outRightResampleBufferIndex; i ++) {
        intData[2 * i] = outRightResampleBuffer[i];
        intData[2 * i + 1] = outLeftResampleBuffer[i];
    }
    
    int samples = outLeftResampleBufferIndex * 2;
    // Reset
    outLeftResampleBufferIndex = 0;
    outRightResampleBufferIndex = 0;
    
    return samples;
}

@end
