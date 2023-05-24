//
//  AgoraSpatialAudioKit.h
//  AgoraRtcKit
//
//  Copyright (c) 2018 Agora. All rights reserved.
//

#ifndef AgoraSpatialAudioKit_h
#define AgoraSpatialAudioKit_h

#import <Foundation/Foundation.h>
#import "AgoraEnumerates.h"
#import "AgoraObjects.h"

__attribute__((visibility("default"))) @interface AgoraRemoteVoicePositionInfo : NSObject
@property(strong, nonatomic) NSArray<NSNumber*> * _Nonnull position;
@property(strong, nonatomic) NSArray<NSNumber*> * _Nullable forward;
@end

__attribute__((visibility("default"))) @interface AgoraSpatialAudioZone : NSObject
//the zone id
@property(assign, nonatomic)  NSInteger zoneSetId;
//zone center point
@property(strong, nonatomic)  NSArray<NSNumber*> * _Nonnull position;
//forward direction 
@property(strong, nonatomic)  NSArray<NSNumber*> * _Nonnull forward;
//right direction
@property(strong, nonatomic)  NSArray<NSNumber*> * _Nonnull right;
//up direction
@property(strong, nonatomic)  NSArray<NSNumber*> * _Nonnull up;
//the forward side length of the zone
@property(assign, nonatomic)  float forwardLength;
//tehe right side length of the zone
@property(assign, nonatomic)  float rightLength;
//the up side length of the zone
@property(assign, nonatomic)  float upLength;
//the audio attenuation of zone
@property(assign, nonatomic) float audioAttenuation;
@end

@class AgoraRtcEngineKit, AgoraBaseSpatialAudioKit, AgoraLocalSpatialAudioKit;

__attribute__((visibility("default"))) @interface AgoraCloudSpatialAudioConfig : NSObject
@property(assign, nonatomic) AgoraRtcEngineKit* _Nullable rtcEngine;
/** The App ID issued to you by Agora. See [How to get the App ID](https://docs.agora.io/en/Agora%20Platform/token#get-an-app-id). Only users in apps with the same App ID can join the same channel and communicate with each other. Use an App ID to create only one AgoraRtcEngineKit instance.  To change your App ID, call [destroy]([AgoraRtcEngineKit destroy]) to `destroy` the current AgoraRtcEngineKit instance, and after `destroy` returns 0, call [sharedEngineWithConfig]([AgoraRtcEngineKit sharedEngineWithConfig:delegate:]) to create an AgoraRtcEngineKit instance with the new App ID.
 */
@property(copy, nonatomic) NSString* _Nullable appId;
/** The region for connection. This advanced feature applies to scenarios that have regional restrictions. <p>For the regions that Agora supports, see AgoraAreaCode. The area codes support bitwise operation. After specifying the region, the SDK connects to the Agora servers within that region.</p>
 */
@property(assign, nonatomic) NSUInteger deployRegion;
@end

__attribute__((visibility("default"))) @interface AgoraLocalSpatialAudioConfig : NSObject
@property(assign, nonatomic) AgoraRtcEngineKit* _Nullable rtcEngine;
@end

__attribute__((visibility("default"))) @interface AgoraBaseSpatialAudioKit : NSObject

- (int)setMaxAudioRecvCount:(NSUInteger)maxCount NS_SWIFT_NAME(setMaxAudioRecvCount(_:));

- (int)setAudioRecvRange:(float)range NS_SWIFT_NAME(setAudioRecvRange(_:));

- (int)setDistanceUnit:(float)unit NS_SWIFT_NAME(setDistanceUnit(_:));

- (int)updatePlayerPositionInfo:(NSInteger)playerId positionInfo:(AgoraRemoteVoicePositionInfo* _Nonnull)positionInfo NS_SWIFT_NAME(updatePlayerPositionInfo(_:positionInfo:));

- (int)updateSelfPosition:(NSArray<NSNumber*>* _Nonnull)position axisForward:(NSArray<NSNumber*>* _Nonnull)axisForward axisRight:(NSArray<NSNumber*>* _Nonnull)axisRight axisUp:(NSArray<NSNumber*>* _Nonnull)axisUp NS_SWIFT_NAME(updateSelfPosition(_:axisForward:axisRight:axisUp:));

- (int)updateSelfPositionEx:(NSArray<NSNumber*>* _Nonnull)position
                axisForward:(NSArray<NSNumber*>* _Nonnull)axisForward
                axisRight:(NSArray<NSNumber*>* _Nonnull)axisRight
                axisUp:(NSArray<NSNumber*>* _Nonnull)axisUp
                connection:(AgoraRtcConnection * _Nonnull)connection NS_SWIFT_NAME(updateSelfPositionEx(_:axisForward:axisRight:axisUp:connection:));

- (int)muteLocalAudioStream:(BOOL)mute NS_SWIFT_NAME(muteLocalAudioStream(_:));

- (int)muteAllRemoteAudioStreams:(BOOL)mute NS_SWIFT_NAME(muteAllRemoteAudioStreams(_:));

- (int)setZones:(NSArray<AgoraSpatialAudioZone*> * _Nonnull)zones NS_SWIFT_NAME(setZones(_:));

- (int)setPlayerAttenuation:(double)attenuation playerId:(NSUInteger)playerId forceSet:(BOOL)forceSet NS_SWIFT_NAME(setPlayerAttenuation(_:playerId:forceSet:));

- (int)muteRemoteAudioStream:(NSUInteger)uid mute:(BOOL)mute NS_SWIFT_NAME(muteRemoteAudioStream(_:mute:));

@end
__attribute__((visibility("default"))) @interface AgoraLocalSpatialAudioKit : AgoraBaseSpatialAudioKit

+ (instancetype _Nonnull)sharedLocalSpatialAudioWithConfig:(AgoraLocalSpatialAudioConfig* _Nonnull)config NS_SWIFT_NAME(sharedLocalSpatialAudio(with:));

+ (void)destroy NS_SWIFT_NAME(destroy());

- (int)updateRemotePosition:(NSUInteger)uid positionInfo:(AgoraRemoteVoicePositionInfo* _Nonnull)posInfo NS_SWIFT_NAME(updateRemotePosition(_:positionInfo:));

- (int)updateRemotePositionEx:(NSUInteger)uid positionInfo:(AgoraRemoteVoicePositionInfo* _Nonnull)posInfo connection:(AgoraRtcConnection * _Nonnull)connection NS_SWIFT_NAME(updateRemotePositionEx(_:positionInfo:connection:));

- (int)removeRemotePosition:(NSUInteger)uid NS_SWIFT_NAME(removeRemotePosition(_:));

- (int)removeRemotePositionEx:(NSUInteger)uid connection:(AgoraRtcConnection * _Nonnull)connection NS_SWIFT_NAME(removeRemotePositionEx(_:connection:));

- (int)clearRemotePositions NS_SWIFT_NAME(clearRemotePositions());

- (int)clearRemotePositionsEx:(AgoraRtcConnection * _Nonnull)connection NS_SWIFT_NAME(clearRemotePositionsEx(_:));

- (int)setRemoteAudioAttenuation:(double)attenuation userId:(NSUInteger)uid forceSet:(BOOL)forceSet NS_SWIFT_NAME(setRemoteAudioAttenuation(_:userId:forceSet:));

@end

#endif /* AgoraSpatialAudioKit_h */
