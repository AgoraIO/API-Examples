//
//  JoinChannelVideo.m
//  APIExample
//
//  Created by zhaoyongqiang on 2023/7/11.
//

#import "VoiceChanger.h"
#import "KeyCenter.h"
#import <AgoraRtcKit/AgoraRtcKit.h>
#import "VideoView.h"
#import "APIExample_OC-swift.h"

@interface VoiceChangerEntry ()
@property (weak, nonatomic) IBOutlet UITextField *textField;

@end

@implementation VoiceChangerEntry

- (void)viewDidLoad {
    [super viewDidLoad];
}

- (IBAction)onClickJoinButton:(id)sender {
    [self.textField resignFirstResponder];
    
    UIStoryboard *storyBoard = [UIStoryboard storyboardWithName:@"VoiceChanger" bundle:nil];
    BaseViewController *newViewController = [storyBoard instantiateViewControllerWithIdentifier:@"VoiceChanger"];
    newViewController.configs = @{@"channelName": self.textField.text};
    [self.navigationController pushViewController:newViewController animated:YES];
}

@end


@interface VoiceChanger ()<AgoraRtcEngineDelegate>
@property (weak, nonatomic) IBOutlet UIView *containerView;
@property (nonatomic, strong)VideoView *localView;
@property (nonatomic, strong)VideoView *remoteView;
@property (nonatomic, strong)AgoraRtcEngineKit *agoraKit;
@property (weak, nonatomic) IBOutlet UIButton *chatBeautifierBtn;
@property (weak, nonatomic) IBOutlet UIButton *timbreTransformationBtn;
@property (weak, nonatomic) IBOutlet UIButton *voiceChangerBtn;
@property (weak, nonatomic) IBOutlet UIButton *styleTransformationBtn;
@property (weak, nonatomic) IBOutlet UIButton *roomAcousticsBtn;
@property (weak, nonatomic) IBOutlet UIButton *pitchCorrectionBtn;
@property (weak, nonatomic) IBOutlet UIButton *voiceConversionBtn;
@property (weak, nonatomic) IBOutlet UIButton *equalizationFreqBtn;
@property (weak, nonatomic) IBOutlet UIButton *reverbKeyBtn;
@property (weak, nonatomic) IBOutlet UISlider *reverbValueSlider;
@property (weak, nonatomic) IBOutlet UILabel *audioEffectParam1Label;
@property (weak, nonatomic) IBOutlet UILabel *audioEffectParam2Label;
@property (weak, nonatomic) IBOutlet UISlider *audioEffectParam1Slider;
@property (weak, nonatomic) IBOutlet UISlider *audioEffectParam2Slider;

@property (nonatomic, assign) AgoraAudioEqualizationBandFrequency equalizationFreq;
@property (nonatomic, assign) NSInteger equalizationGain;
@property (nonatomic, assign) AgoraAudioReverbType reverbType;
@property (nonatomic, strong) NSMutableDictionary *reverbMap;
@property (nonatomic, assign) AgoraAudioEffectPreset currentAudioEffects;

@end

@implementation VoiceChanger

- (NSMutableDictionary *)reverbMap {
    if (_reverbMap == nil) {
        _reverbMap = [NSMutableDictionary dictionaryWithDictionary:@{
            [NSString stringWithFormat:@"%ld",AgoraAudioReverbDryLevel]: @(0),
            [NSString stringWithFormat:@"%ld",AgoraAudioReverbWetLevel]: @(0),
            [NSString stringWithFormat:@"%ld",AgoraAudioReverbRoomSize]: @(0),
            [NSString stringWithFormat:@"%ld",AgoraAudioReverbWetDelay]: @(0),
            [NSString stringWithFormat:@"%ld",AgoraAudioReverbStrength]: @(0),
        }];
    }
    return _reverbMap;
}
- (VideoView *)localView {
    if (_localView == nil) {
        _localView = (VideoView *)[NSBundle loadVideoViewFormType:StreamTypeLocal audioOnly:NO];
    }
    return _localView;
}

- (VideoView *)remoteView {
    if (_remoteView == nil) {
        _remoteView = (VideoView *)[NSBundle loadVideoViewFormType:StreamTypeRemote audioOnly:NO];
    }
    return _remoteView;
}

- (IBAction)onChatBeautifier:(UIButton *)sender {
    UIAlertController *alertVC = [UIAlertController alertControllerWithTitle:@"Set Chat Beautifier".localized message:nil preferredStyle:(UIAlertControllerStyleActionSheet)];
    NSDictionary *params = @{
        @"Off": @(AgoraVoiceBeautifierPresetOff),
        @"FemaleFresh": @(AgoraVoiceBeautifierPresetChatBeautifierFresh),
        @"FemaleVitality": @(AgoraVoiceBeautifierPresetChatBeautifierVitality),
        @"MaleMagnetic": @(AgoraVoiceBeautifierPresetChatBeautifierMagnetic),
    };
    NSArray *allKeys = params.allKeys;
    for (int i = 0; i < allKeys.count; i++) {
        NSString *title = allKeys[i];
        UIAlertAction *ok = [UIAlertAction actionWithTitle:title.localized style:(UIAlertActionStyleDefault) handler:^(UIAlertAction * _Nonnull action) {
            [self resetVoiceChanger];
            [self updateAudioEffectsControls:(AgoraAudioEffectPresetOff)];
            //when using this method with setLocalVoiceReverbPreset,
            //the method called later overrides the one called earlier
            AgoraVoiceBeautifierPreset preset = ((NSNumber *)params[title]).integerValue;
            [self.agoraKit setVoiceBeautifierPreset:preset];
            [self.chatBeautifierBtn setTitle:title.localized forState:(UIControlStateNormal)];
        }];
        [alertVC addAction:ok];
    }
    UIAlertAction *cancel = [UIAlertAction actionWithTitle:@"Cancel".localized style:(UIAlertActionStyleCancel) handler:nil];
    [alertVC addAction:cancel];
//    [self presentViewController:alertVC animated:YES completion:nil];
    [self presentAlertViewController:alertVC];
}
- (IBAction)onVoiceChanger:(UIButton *)sender {
    UIAlertController *alertVC = [UIAlertController alertControllerWithTitle:@"Set Voice Changer".localized message:nil preferredStyle:(UIAlertControllerStyleActionSheet)];
    NSDictionary *params = @{
        @"Off": @(AgoraAudioEffectPresetOff),
        @"FxUncle": @(AgoraAudioEffectPresetVoiceChangerEffectUncle),
        @"Old Man": @(AgoraAudioEffectPresetVoiceChangerEffectOldMan),
        @"Baby Boy": @(AgoraAudioEffectPresetVoiceChangerEffectBoy),
        @"FxSister": @(AgoraAudioEffectPresetVoiceChangerEffectSister),
        @"Baby Girl": @(AgoraAudioEffectPresetVoiceChangerEffectGirl),
        @"ZhuBaJie": @(AgoraAudioEffectPresetVoiceChangerEffectPigKin),
        @"Hulk": @(AgoraAudioEffectPresetVoiceChangerEffectHulk)
    };
    NSArray *allKeys = params.allKeys;
    for (int i = 0; i < allKeys.count; i++) {
        NSString *title = allKeys[i];
        UIAlertAction *ok = [UIAlertAction actionWithTitle:title.localized style:(UIAlertActionStyleDefault) handler:^(UIAlertAction * _Nonnull action) {
            [self resetVoiceChanger];
            AgoraAudioEffectPreset preset = ((NSNumber *)params[title]).integerValue;
            [self updateAudioEffectsControls:preset];
            //when using this method with setLocalVoiceReverbPreset,
            //the method called later overrides the one called earlier
            [self.agoraKit setAudioEffectPreset:preset];
            [self.voiceChangerBtn setTitle:title.localized forState:(UIControlStateNormal)];
        }];
        [alertVC addAction:ok];
    }
    UIAlertAction *cancel = [UIAlertAction actionWithTitle:@"Cancel".localized style:(UIAlertActionStyleCancel) handler:nil];
    [alertVC addAction:cancel];
//    [self presentViewController:alertVC animated:YES completion:nil];
    [self presentAlertViewController:alertVC];
}
- (IBAction)onStyleTransformation:(UIButton *)sender {
    UIAlertController *alertVC = [UIAlertController alertControllerWithTitle:@"Set Style Transformation".localized message:nil preferredStyle:(UIAlertControllerStyleActionSheet)];
    NSDictionary *params = @{
        @"Off": @(AgoraAudioEffectPresetOff),
        @"Pop": @(AgoraAudioEffectPresetStyleTransformationPopular),
        @"R&B": @(AgoraAudioEffectPresetStyleTransformationRnb)
    };
    NSArray *allKeys = params.allKeys;
    for (int i = 0; i < allKeys.count; i++) {
        NSString *title = allKeys[i];
        UIAlertAction *ok = [UIAlertAction actionWithTitle:title.localized style:(UIAlertActionStyleDefault) handler:^(UIAlertAction * _Nonnull action) {
            [self resetVoiceChanger];
            AgoraAudioEffectPreset preset = ((NSNumber *)params[title]).integerValue;
            [self updateAudioEffectsControls:preset];
            //when using this method with setLocalVoiceReverbPreset,
            //the method called later overrides the one called earlier
            [self.agoraKit setAudioEffectPreset:preset];
            [self.styleTransformationBtn setTitle:title.localized forState:(UIControlStateNormal)];
        }];
        [alertVC addAction:ok];
    }
    UIAlertAction *cancel = [UIAlertAction actionWithTitle:@"Cancel".localized style:(UIAlertActionStyleCancel) handler:nil];
    [alertVC addAction:cancel];
//    [self presentViewController:alertVC animated:YES completion:nil];
    [self presentAlertViewController:alertVC];
}
- (IBAction)onRoomAcoustics:(UIButton *)sender {
    UIAlertController *alertVC = [UIAlertController alertControllerWithTitle:@"Set Room Acoustics".localized message:nil preferredStyle:(UIAlertControllerStyleActionSheet)];
    NSDictionary *params = @{
        @"Off": @(AgoraAudioEffectPresetOff),
        @"KTV": @(AgoraAudioEffectPresetRoomAcousticsKTV),
        @"Vocal Concert": @(AgoraAudioEffectPresetRoomAcousVocalConcer),
        @"Studio": @(AgoraAudioEffectPresetRoomAcousStudio),
        @"Phonograph": @(AgoraAudioEffectPresetRoomAcousPhonograph),
        @"Virtual Stereo": @(AgoraAudioEffectPresetRoomAcousVirtualStereo),
        @"Spacial": @(AgoraAudioEffectPresetRoomAcousSpatial),
        @"Ethereal": @(AgoraAudioEffectPresetRoomAcousEthereal),
        @"3D Voice": @(AgoraAudioEffectPresetRoomAcous3DVoice)
    };
    NSArray *allKeys = params.allKeys;
    for (int i = 0; i < allKeys.count; i++) {
        NSString *title = allKeys[i];
        UIAlertAction *ok = [UIAlertAction actionWithTitle:title.localized style:(UIAlertActionStyleDefault) handler:^(UIAlertAction * _Nonnull action) {
            [self resetVoiceChanger];
            AgoraAudioEffectPreset preset = ((NSNumber *)params[title]).integerValue;
            [self updateAudioEffectsControls:preset];
            //when using this method with setLocalVoiceReverbPreset,
            //the method called later overrides the one called earlier
            [self.agoraKit setAudioEffectPreset:preset];
            [self.roomAcousticsBtn setTitle:title.localized forState:(UIControlStateNormal)];
        }];
        [alertVC addAction:ok];
    }
    UIAlertAction *cancel = [UIAlertAction actionWithTitle:@"Cancel".localized style:(UIAlertActionStyleCancel) handler:nil];
    [alertVC addAction:cancel];
//    [self presentViewController:alertVC animated:YES completion:nil];
    [self presentAlertViewController:alertVC];
}
- (IBAction)onPitchCorrection:(UIButton *)sender {
    UIAlertController *alertVC = [UIAlertController alertControllerWithTitle:@"Set Pitch Correction".localized message:nil preferredStyle:(UIAlertControllerStyleActionSheet)];
    NSDictionary *params = @{
        @"Pitch Correction": @(AgoraAudioEffectPresetPitchCorrection)
    };
    NSArray *allKeys = params.allKeys;
    for (int i = 0; i < allKeys.count; i++) {
        NSString *title = allKeys[i];
        UIAlertAction *ok = [UIAlertAction actionWithTitle:title.localized style:(UIAlertActionStyleDefault) handler:^(UIAlertAction * _Nonnull action) {
            [self resetVoiceChanger];
            AgoraAudioEffectPreset preset = ((NSNumber *)params[title]).integerValue;
            [self updateAudioEffectsControls:preset];
            //when using this method with setLocalVoiceReverbPreset,
            //the method called later overrides the one called earlier
            [self.agoraKit setAudioEffectPreset:preset];
            [self.pitchCorrectionBtn setTitle:title.localized forState:(UIControlStateNormal)];
        }];
        [alertVC addAction:ok];
    }
    UIAlertAction *cancel = [UIAlertAction actionWithTitle:@"Cancel".localized style:(UIAlertActionStyleCancel) handler:nil];
    [alertVC addAction:cancel];
//    [self presentViewController:alertVC animated:YES completion:nil];
    [self presentAlertViewController:alertVC];
}
- (IBAction)onVoiceConversion:(UIButton *)sender {
    UIAlertController *alertVC = [UIAlertController alertControllerWithTitle:@"Set Voice Conversion".localized message:nil preferredStyle:(UIAlertControllerStyleActionSheet)];
    NSDictionary *params = @{
        @"Off": @(AgoraVoiceConversionPresetOff),
        @"Neutral": @(AgoraVoiceConversionPresetNeutral),
        @"Sweet": @(AgoraVoiceConversionPresetSweet),
        @"Solid": @(AgoraVoiceConversionPresetChangerSolid),
        @"Bass": @(AgoraVoiceConversionPresetChangerBass)
    };
    NSArray *allKeys = params.allKeys;
    for (int i = 0; i < allKeys.count; i++) {
        NSString *title = allKeys[i];
        UIAlertAction *ok = [UIAlertAction actionWithTitle:title.localized style:(UIAlertActionStyleDefault) handler:^(UIAlertAction * _Nonnull action) {
            [self resetVoiceChanger];
            AgoraVoiceConversionPreset preset = ((NSNumber *)params[title]).integerValue;
            [self updateAudioEffectsControls:AgoraAudioEffectPresetOff];
            //when using this method with setLocalVoiceReverbPreset,
            //the method called later overrides the one called earlier
            [self.agoraKit setVoiceConversionPreset:preset];
            [self.voiceConversionBtn setTitle:title.localized forState:(UIControlStateNormal)];
        }];
        [alertVC addAction:ok];
    }
    UIAlertAction *cancel = [UIAlertAction actionWithTitle:@"Cancel".localized style:(UIAlertActionStyleCancel) handler:nil];
    [alertVC addAction:cancel];
//    [self presentViewController:alertVC animated:YES completion:nil];
    [self presentAlertViewController:alertVC];
}

- (IBAction)onTimbreTransformation:(UIButton *)sender {
    UIAlertController *alertVC = [UIAlertController alertControllerWithTitle:@"Set Timbre Transformation".localized message:nil preferredStyle:(UIAlertControllerStyleActionSheet)];
    NSDictionary *params = @{
        @"Off": @(AgoraVoiceBeautifierPresetOff),
        @"Vigorous": @(AgoraVoiceBeautifierTimbreTransformationVigorous),
        @"Deep": @(AgoraVoiceBeautifierTimbreTransformationDeep),
        @"Mellow": @(AgoraVoiceBeautifierTimbreTransformationMellow),
        @"Falsetto": @(AgoraVoiceBeautifierTimbreTransformationFalsetto),
        @"Full": @(AgoraVoiceBeautifierTimbreTransformationFull),
        @"Clear": @(AgoraVoiceBeautifierTimbreTransformationClear),
        @"Resounding": @(AgoraVoiceBeautifierTimbreTransformationResounding),
        @"Ringing": @(AgoraVoiceBeautifierTimbreTransformatRinging)
    };
    NSArray *allKeys = params.allKeys;
    for (int i = 0; i < allKeys.count; i++) {
        NSString *title = allKeys[i];
        UIAlertAction *ok = [UIAlertAction actionWithTitle:title.localized style:(UIAlertActionStyleDefault) handler:^(UIAlertAction * _Nonnull action) {
            [self resetVoiceChanger];
            [self updateAudioEffectsControls:(AgoraAudioEffectPresetOff)];
            //when using this method with setLocalVoiceReverbPreset,
            //the method called later overrides the one called earlier
            AgoraVoiceBeautifierPreset preset = ((NSNumber *)params[title]).integerValue;
            [self.agoraKit setVoiceBeautifierPreset:preset];
            [self.timbreTransformationBtn setTitle:title.localized forState:(UIControlStateNormal)];
        }];
        [alertVC addAction:ok];
    }
    UIAlertAction *cancel = [UIAlertAction actionWithTitle:@"Cancel".localized style:(UIAlertActionStyleCancel) handler:nil];
    [alertVC addAction:cancel];
//    [self presentViewController:alertVC animated:YES completion:nil];
    [self presentAlertViewController:alertVC];
}
- (IBAction)onAudioEffectsParamUpdated:(UISlider *)sender {
    int param1 = self.audioEffectParam1Slider.isEnabled ? self.audioEffectParam1Slider.value : 0;
    int param2 = self.audioEffectParam2Slider.isEnabled ? self.audioEffectParam2Slider.value : 0;
    [self.agoraKit setAudioEffectParameters:self.currentAudioEffects param1:param1 param2:param2];
}
- (IBAction)onLocalVoicePitch:(UISlider *)sender {
    [self.agoraKit setLocalVoicePitch:sender.value];
}
- (IBAction)onLocalVoiceEqualizaitonFreq:(UIButton *)sender {
    UIAlertController *alertVC = [UIAlertController alertControllerWithTitle:@"Set Band Frequency".localized message:nil preferredStyle:(UIAlertControllerStyleActionSheet)];
    NSDictionary *params = @{
        @"31Hz": @(AgoraAudioEqualizationBand31),
        @"62Hz": @(AgoraAudioEqualizationBand62),
        @"125Hz": @(AgoraAudioEqualizationBand125),
        @"250Hz": @(AgoraAudioEqualizationBand250),
        @"500Hz": @(AgoraAudioEqualizationBand500),
        @"1kHz": @(AgoraAudioEqualizationBand1K),
        @"2kHz": @(AgoraAudioEqualizationBand2K),
        @"4kHz": @(AgoraAudioEqualizationBand4K),
        @"8kHz": @(AgoraAudioEqualizationBand8K),
        @"16kHz": @(AgoraAudioEqualizationBand16K)
    };
    NSArray *allKeys = params.allKeys;
    for (int i = 0; i < allKeys.count; i++) {
        NSString *title = allKeys[i];
        UIAlertAction *ok = [UIAlertAction actionWithTitle:title.localized style:(UIAlertActionStyleDefault) handler:^(UIAlertAction * _Nonnull action) {
            AgoraAudioEqualizationBandFrequency preset = ((NSNumber *)params[title]).integerValue;
            self.equalizationFreq = preset;
            [self.agoraKit setLocalVoiceEqualizationOfBandFrequency:self.equalizationFreq withGain:self.equalizationGain];
            [self.equalizationFreqBtn setTitle:title.localized forState:(UIControlStateNormal)];
        }];
        [alertVC addAction:ok];
    }
    UIAlertAction *cancel = [UIAlertAction actionWithTitle:@"Cancel".localized style:(UIAlertActionStyleCancel) handler:nil];
    [alertVC addAction:cancel];
//    [self presentViewController:alertVC animated:YES completion:nil];
    [self presentAlertViewController:alertVC];
}
- (IBAction)onLocalVoiceEqualizationGain:(UISlider *)sender {
    self.equalizationGain = (int)sender.value;
    [self.agoraKit setLocalVoiceEqualizationOfBandFrequency:self.equalizationFreq withGain:self.equalizationGain];
}
- (void)updateReverbValueRange: (AgoraAudioReverbType)reverbKey {
    float min = 0;
    float max = 0;
    switch (reverbKey) {
        case AgoraAudioReverbDryLevel:
        case AgoraAudioReverbWetLevel:
            min = -20;
            max = 10;
            break;
            
        case AgoraAudioReverbRoomSize:
        case AgoraAudioReverbStrength:
            min = 0;
            max = 100;
            break;
            
        case AgoraAudioReverbWetDelay:
            min = 0;
            max = 200;
            break;
            
        default:
            break;
    }
    self.reverbValueSlider.minimumValue = min;
    self.reverbValueSlider.maximumValue = max;
    self.reverbValueSlider.value = ((NSNumber *)[self.reverbMap objectForKey:[NSString stringWithFormat:@"%ld",self.reverbType]]).floatValue;
}
- (IBAction)onLocalVoiceReverbKey:(UIButton *)sender {
    UIAlertController *alertVC = [UIAlertController alertControllerWithTitle:@"Set Reverb Key".localized message:nil preferredStyle:(UIAlertControllerStyleActionSheet)];
    NSDictionary *params = @{
        @"Dry Level": @(AgoraAudioReverbDryLevel),
        @"Wet Level": @(AgoraAudioReverbWetLevel),
        @"Room Size": @(AgoraAudioReverbRoomSize),
        @"Wet Delay": @(AgoraAudioReverbWetDelay),
        @"Strength": @(AgoraAudioReverbStrength)
    };
    NSArray *allKeys = params.allKeys;
    for (int i = 0; i < allKeys.count; i++) {
        NSString *title = allKeys[i];
        UIAlertAction *ok = [UIAlertAction actionWithTitle:title.localized style:(UIAlertActionStyleDefault) handler:^(UIAlertAction * _Nonnull action) {
            AgoraAudioReverbType preset = ((NSNumber *)params[title]).integerValue;
            [self updateReverbValueRange:preset];
            [self.reverbKeyBtn setTitle:title.localized forState:(UIControlStateNormal)];
        }];
        [alertVC addAction:ok];
    }
    UIAlertAction *cancel = [UIAlertAction actionWithTitle:@"Cancel".localized style:(UIAlertActionStyleCancel) handler:nil];
    [alertVC addAction:cancel];
//    [self presentViewController:alertVC animated:YES completion:nil];
    [self presentAlertViewController:alertVC];
}
- (IBAction)onLocalVoiceReverbValue:(UISlider *)sender {
    int value = (int)sender.value;
    [self.reverbMap setValue:@(value) forKey:[NSString stringWithFormat:@"%ld",self.reverbType]];
    [self.agoraKit setLocalVoiceReverbOfType:self.reverbType withValue:value];
}
- (IBAction)onVoiceFormantChange:(UISlider *)sender {
    [self.agoraKit setLocalVoiceFormant:sender.value];
}
- (void)resetVoiceChanger {
    [self.chatBeautifierBtn setTitle:@"Off" forState:(UIControlStateNormal)];
    [self.timbreTransformationBtn setTitle:@"Off" forState:(UIControlStateNormal)];
    [self.voiceChangerBtn setTitle:@"Off" forState:(UIControlStateNormal)];
    [self.styleTransformationBtn setTitle:@"Off" forState:(UIControlStateNormal)];
    [self.roomAcousticsBtn setTitle:@"Off" forState:(UIControlStateNormal)];
    [self.pitchCorrectionBtn setTitle:@"Off" forState:(UIControlStateNormal)];
    [self.voiceConversionBtn setTitle:@"Off" forState:(UIControlStateNormal)];
}

- (void)updateAudioEffectsControls: (AgoraAudioEffectPreset)effect {
    self.currentAudioEffects = effect;
    if (effect == AgoraAudioEffectPresetRoomAcous3DVoice) {
        [self.audioEffectParam1Slider setEnabled:YES];
        [self.audioEffectParam2Slider setEnabled:NO];
        self.audioEffectParam1Label.text = @"Cycle";
        self.audioEffectParam2Label.text = @"N/A";
        self.audioEffectParam1Slider.minimumValue = 0;
        self.audioEffectParam1Slider.maximumValue = 60;
        self.audioEffectParam1Slider.value = 10;
    } else if (effect == AgoraAudioEffectPresetPitchCorrection) {
        [self.audioEffectParam1Slider setEnabled:YES];
        [self.audioEffectParam2Slider setEnabled:YES];
        self.audioEffectParam1Label.text = @"Tonic Mode";
        self.audioEffectParam2Label.text = @"Tonic Pitch";
        
        self.audioEffectParam1Slider.minimumValue = 1;
        self.audioEffectParam1Slider.maximumValue = 3;
        self.audioEffectParam1Slider.value = 1;
        self.audioEffectParam2Slider.minimumValue = 1;
        self.audioEffectParam2Slider.maximumValue = 12;
        self.audioEffectParam2Slider.value = 4;
    } else {
        [self.audioEffectParam1Slider setEnabled:NO];
        [self.audioEffectParam2Slider setEnabled:NO];
        self.audioEffectParam1Label.text = @"N/A";
        self.audioEffectParam2Label.text = @"N/A";
    }
}



- (void)viewDidLoad {
    [super viewDidLoad];
    self.equalizationFreq = AgoraAudioEqualizationBand31;
    self.equalizationGain = 0;
    self.reverbType = AgoraAudioReverbDryLevel;
    self.currentAudioEffects = AgoraAudioEffectPresetOff;
    
    // reset voice changer options
    [self resetVoiceChanger];
    [self.equalizationFreqBtn setTitle:@"31Hz".localized forState:(UIControlStateNormal)];
    [self.reverbKeyBtn setTitle:@"Dry Level".localized forState:(UIControlStateNormal)];
    
    // layout render view
    [self.localView setPlaceholder:@"Local Host".localized];
    [self.remoteView setPlaceholder:@"Remote Host".localized];
    [self.containerView layoutStream:@[self.localView, self.remoteView]];
    
    // set up agora instance when view loaded
    AgoraRtcEngineConfig *config = [[AgoraRtcEngineConfig alloc] init];
    config.appId = KeyCenter.AppId;
    config.channelProfile = AgoraChannelProfileLiveBroadcasting;
    
    self.agoraKit = [AgoraRtcEngineKit sharedEngineWithConfig:config delegate:self];
    
    NSString *channelName = [self.configs objectForKey:@"channelName"];
    // make myself a broadcaster
    [self.agoraKit setClientRole:(AgoraClientRoleBroadcaster)];
    // enable video module and set up video encoding configs
    [self.agoraKit enableAudio];
    [self.agoraKit enableVideo];
    
    // Before calling the method, you need to set the profile
    // parameter of setAudioProfile to AUDIO_PROFILE_MUSIC_HIGH_QUALITY(4)
    // or AUDIO_PROFILE_MUSIC_HIGH_QUALITY_STEREO(5), and to set
    // scenario parameter to AUDIO_SCENARIO_GAME_STREAMING(3).
    [self.agoraKit setAudioProfile:(AgoraAudioProfileMusicHighQualityStereo)];
    [self.agoraKit setAudioScenario:(AgoraAudioScenarioGameStreaming)];
    
    AgoraVideoEncoderConfiguration *encoderConfig = [[AgoraVideoEncoderConfiguration alloc] initWithSize:CGSizeMake(960, 540)
                                                                                               frameRate:(AgoraVideoFrameRateFps15)
                                                                                                 bitrate:15
                                                                                         orientationMode:(AgoraVideoOutputOrientationModeFixedPortrait)
                                                                                              mirrorMode:(AgoraVideoMirrorModeAuto)];
    [self.agoraKit setVideoEncoderConfiguration:encoderConfig];
    
    // set up local video to render your local camera preview
    AgoraRtcVideoCanvas *videoCanvas = [[AgoraRtcVideoCanvas alloc] init];
    videoCanvas.uid = 0;
    // the view to be binded
    videoCanvas.view = self.localView.videoView;
    videoCanvas.renderMode = AgoraVideoRenderModeHidden;
    [self.agoraKit setupLocalVideo:videoCanvas];
    // you have to call startPreview to see local video
    [self.agoraKit startPreview];
    
    // Set audio route to speaker
    [self.agoraKit setEnableSpeakerphone:YES];
    
    // start joining channel
    // 1. Users can only see each other after they join the
    // same channel successfully using the same app id.
    // 2. If app certificate is turned on at dashboard, token is needed
    // when joining channel. The channel name and uid used to calculate
    // the token has to match the ones used for channel join
    AgoraRtcChannelMediaOptions *options = [[AgoraRtcChannelMediaOptions alloc] init];
    options.autoSubscribeAudio = YES;
    options.autoSubscribeVideo = YES;
    options.publishCameraTrack = YES;
    options.publishMicrophoneTrack = YES;
    options.clientRoleType = AgoraClientRoleBroadcaster;
    
    [[NetworkManager shared] generateTokenWithChannelName:channelName uid:0 success:^(NSString * _Nullable token) {
        int result = [self.agoraKit joinChannelByToken:token channelId:channelName uid:0 mediaOptions:options joinSuccess:nil];
        if (result != 0) {
            // Usually happens with invalid parameters
            // Error code description can be found at:
            // en: https://api-ref.agora.io/en/video-sdk/ios/4.x/documentation/agorartckit/agoraerrorcode
            // cn: https://doc.shengwang.cn/api-ref/rtc/ios/error-code
            NSLog(@"joinChannel call failed: %d, please check your params", result);
        }
    }];
}

- (void)viewDidDisappear:(BOOL)animated {
    [super viewDidDisappear:animated];
    [self.agoraKit disableAudio];
    [self.agoraKit disableVideo];
    [self.agoraKit stopPreview];
    [self.agoraKit leaveChannel:nil];
    [AgoraRtcEngineKit destroy];
}

/// callback when error occured for agora sdk, you are recommended to display the error descriptions on demand
/// to let user know something wrong is happening
/// Error code description can be found at:
/// en: https://api-ref.agora.io/en/video-sdk/ios/4.x/documentation/agorartckit/agoraerrorcode
/// cn: https://doc.shengwang.cn/api-ref/rtc/ios/error-code
/// @param errorCode error code of the problem
- (void)rtcEngine:(AgoraRtcEngineKit *)engine didOccurError:(AgoraErrorCode)errorCode {
    [LogUtil log:[NSString stringWithFormat:@"Error %ld occur",errorCode] level:(LogLevelError)];
}

- (void)rtcEngine:(AgoraRtcEngineKit *)engine didJoinChannel:(NSString *)channel withUid:(NSUInteger)uid elapsed:(NSInteger)elapsed {
    [LogUtil log:[NSString stringWithFormat:@"Join %@ with uid %lu elapsed %ldms", channel, uid, elapsed] level:(LogLevelDebug)];
    self.localView.uid = uid;
}

/// callback when a remote user is joinning the channel, note audience in live broadcast mode will NOT trigger this event
/// @param uid uid of remote joined user
/// @param elapsed time elapse since current sdk instance join the channel in ms
- (void)rtcEngine:(AgoraRtcEngineKit *)engine didJoinedOfUid:(NSUInteger)uid elapsed:(NSInteger)elapsed {
    [LogUtil log:[NSString stringWithFormat:@"remote user join: %lu %ldms", uid, elapsed] level:(LogLevelDebug)];
    // Only one remote video view is available for this
    // tutorial. Here we check if there exists a surface
    // view tagged as this uid.
    AgoraRtcVideoCanvas *videoCanvas = [[AgoraRtcVideoCanvas alloc]init];
    videoCanvas.uid = uid;
    // the view to be binded
    videoCanvas.view = self.remoteView.videoView;
    videoCanvas.renderMode = AgoraVideoRenderModeHidden;
    [self.agoraKit setupRemoteVideo:videoCanvas];
    self.remoteView.uid = uid;
}

/// callback when a remote user is leaving the channel, note audience in live broadcast mode will NOT trigger this event
/// @param uid uid of remote joined user
/// @param reason reason why this user left, note this event may be triggered when the remote user
/// become an audience in live broadcasting profile
- (void)rtcEngine:(AgoraRtcEngineKit *)engine didOfflineOfUid:(NSUInteger)uid reason:(AgoraUserOfflineReason)reason {
    // to unlink your view from sdk, so that your view reference will be released
    // note the video will stay at its last frame, to completely remove it
    // you will need to remove the EAGL sublayer from your binded view
    AgoraRtcVideoCanvas *videoCanvas = [[AgoraRtcVideoCanvas alloc]init];
    videoCanvas.uid = uid;
    // the view to be binded
    videoCanvas.view = nil;
    [self.agoraKit setupRemoteVideo:videoCanvas];
    self.remoteView.uid = 0;
    [LogUtil log:[NSString stringWithFormat:@"remote user left: %lu", uid] level:(LogLevelDebug)];
}

@end
