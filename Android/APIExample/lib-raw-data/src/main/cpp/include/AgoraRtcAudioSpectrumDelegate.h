//
//  AgoraRtcMediaPlayerProtocol.h
//  AgoraRtcMediaPlayerProtocol
//
//  Copyright (c) 2020 Agora. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "AgoraObjects.h"


@protocol AgoraAudioSpectrumDelegate <NSObject>

@optional
/**
 * Reports the audio spectrum of audio recording.
 *
 * This callback reports the audio spectrum data of the audio recording at the moment
 * in the channel.
 *
 * You can set the time interval of this callback using "enableAudioSpectrumMonitor".
 *
 * @param audioSpectrumData The audio spectrum data of audio recording.
 * - true: Processed.
 * - false: Not processed.
 */
- (BOOL)onLocalAudioSpectrum:(NSArray<NSNumber *> * _Nullable)audioSpectrumData NS_SWIFT_NAME(onLocalAudioSpectrum(_:));
/**
 * Reports the audio spectrum of remote user.
 *
 * This callback reports the IDs and audio spectrum data of the loudest speakers at the moment
 * in the channel.
 *
 * You can set the time interval of this callback using  "enableAudioSpectrumMonitor".
 *
 * @param AudioSpectrumInfo The pointer to AudioSpectrumInfo, which is an array containing
 * the user ID and audio spectrum data for each speaker.
 * - This array contains the following members:
 *   - `uid`, which is the UID of each remote speaker
 *   - `audioSpectrumData`, which reports the audio spectrum of each remote speaker.
 *   - `spectrumDataLength`, the length of audio spectrum data.
 * - true: Processed.
 * - false: Not processed.
 */
- (BOOL)onRemoteAudioSpectrum:(NSArray<AgoraAudioSpectrumInfo *> * _Nullable)AudioSpectrumInfo NS_SWIFT_NAME(onRemoteAudioSpectrum(_:));


@end
