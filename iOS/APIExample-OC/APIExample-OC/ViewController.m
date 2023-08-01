//
//  ViewController.m
//  APIExample
//
//  Created by zhaoyongqiang on 2023/7/11.
//

#import "ViewController.h"
#import "BaseViewController.h"
#import <AgoraRtcKit/AgoraRtcKit.h>

@interface MenuItem : NSObject
@property(nonatomic, copy)NSString *name;
@property(nonatomic, copy)NSString *entry;
@property(nonatomic, copy)NSString *storyboard;
@property(nonatomic, copy)NSString *controller;
@property(nonatomic, copy)NSString *note;

@end
@implementation MenuItem

-(instancetype)initWithName: (NSString *)name storyboard: (NSString *)storyboard controller: (NSString *)controller {
    self.name = name;
    self.storyboard = storyboard;
    self.controller = controller;
    self.entry = @"EntryViewController";
    return self;
}

@end

@interface MenuSection : NSObject

@property(nonatomic, copy)NSString *name;
@property(nonatomic, strong)NSArray<MenuItem *> *rows;

@end
@implementation MenuSection

- (instancetype)initWithName: (NSString *)name rows: (NSArray<MenuItem *> *)rows {
    self.name = name;
    self.rows = rows;
    return self;
}

+ (NSArray *)menus {
    NSMutableArray *array = [NSMutableArray array];
    MenuSection *basicSection = [[MenuSection alloc] initWithName:@"Basic" rows:@[
        [[MenuItem alloc]initWithName:@"Join a channel (Token)".localized storyboard:@"JoinChannelVideoToken" controller:@""],
        [[MenuItem alloc]initWithName:@"Join a channel (Video)".localized storyboard:@"JoinChannelVideo" controller:@""],
        [[MenuItem alloc]initWithName:@"Join a channel (Audio)".localized storyboard:@"JoinChannelAudio" controller:@""],
        [[MenuItem alloc]initWithName:@"Local or remote recording".localized storyboard:@"JoinChannelVideoRecorder" controller:@""]
    ]];
    [array addObject:basicSection];
    MenuSection *anvancedSection = [[MenuSection alloc] initWithName:@"Anvanced" rows:@[
        [[MenuItem alloc]initWithName:@"Live Streaming".localized storyboard:@"LiveStreaming" controller:@"LiveStreaming"],
        [[MenuItem alloc]initWithName:@"RTMP Streaming".localized storyboard:@"RTMPStreaming" controller:@""],
        [[MenuItem alloc]initWithName:@"Fusion CDN Streaming".localized storyboard:@"FusionCDN" controller:@"FusionCDN"],
        [[MenuItem alloc]initWithName:@"Video Metadata".localized storyboard:@"VideoMetadata" controller:@""],
        [[MenuItem alloc]initWithName:@"Voice Changer".localized storyboard:@"VoiceChanger" controller:@""],
        [[MenuItem alloc]initWithName:@"Custom Audio Source".localized storyboard:@"CustomPcmAudioSource" controller:@""],
        [[MenuItem alloc]initWithName:@"Custom Audio Render".localized storyboard:@"CustomAudioRender" controller:@""],
        [[MenuItem alloc]initWithName:@"Custom Video Source(Push)".localized storyboard:@"CustomVideoSourcePush" controller:@""],
        [[MenuItem alloc]initWithName:@"Custom Video Render".localized storyboard:@"CustomVideoRender" controller:@""],
        [[MenuItem alloc]initWithName:@"Raw Audio Data".localized storyboard:@"RawAudioData" controller:@""],
        [[MenuItem alloc]initWithName:@"Raw Video Data".localized storyboard:@"RawVideoData" controller:@""],
        [[MenuItem alloc]initWithName:@"Picture In Picture(require iOS15)".localized storyboard:@"PictureInPicture" controller:@""],
        [[MenuItem alloc]initWithName:@"Simple Filter Extension".localized storyboard:@"SimpleFilter" controller:@""],
        [[MenuItem alloc]initWithName:@"Join Multiple Channels".localized storyboard:@"JoinMultiChannel" controller:@""],
        [[MenuItem alloc]initWithName:@"Stream Encryption".localized storyboard:@"StreamEncryption" controller:@""],
        [[MenuItem alloc]initWithName:@"Audio Mixing".localized storyboard:@"AudioMixing" controller:@""],
        [[MenuItem alloc]initWithName:@"Media Player".localized storyboard:@"MediaPlayer" controller:@""],
        [[MenuItem alloc]initWithName:@"Screen Share".localized storyboard:@"ScreenShare" controller:@""],
        [[MenuItem alloc]initWithName:@"Video Process".localized storyboard:@"VideoProcess" controller:@""],
        [[MenuItem alloc]initWithName:@"Rhythm Player".localized storyboard:@"RhythmPlayer" controller:@""],
        [[MenuItem alloc]initWithName:@"Create Data Stream".localized storyboard:@"CreateDataStream" controller:@""],
        [[MenuItem alloc]initWithName:@"Media Channel Relay".localized storyboard:@"MediaChannelRelay" controller:@""],
        [[MenuItem alloc]initWithName:@"Spatial Audio".localized storyboard:@"SpatialAudio" controller:@""],
        [[MenuItem alloc]initWithName:@"Content Inspect".localized storyboard:@"ContentInspect" controller:@""],
        [[MenuItem alloc]initWithName:@"Mutli Camera(iOS13.0+)".localized storyboard:@"MutliCamera" controller:@""]
    ]];
    [array addObject:anvancedSection];
    
    return array.copy;
}

@end

@interface ViewController ()<UITableViewDelegate, UITableViewDataSource>
@property (weak, nonatomic) IBOutlet UITableView *tableView;

@property (nonatomic, strong)NSArray<MenuSection *> *datas;

@end

@implementation ViewController

- (NSArray<MenuSection *> *)datas {
    return [MenuSection menus];
}

- (void)viewDidLoad {
    [super viewDidLoad];
    self.title = @"Agora API Example";
    self.tableView.delegate = self;
    self.tableView.dataSource = self;
    self.tableView.tableFooterView = [UIView new];
}

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
    return self.datas.count;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    return self.datas[section].rows.count;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:@"cell" forIndexPath:indexPath];
    cell.textLabel.text = self.datas[indexPath.section].rows[indexPath.row].name;
    return cell;
}

- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section {
    return self.datas[section].name;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    [tableView deselectRowAtIndexPath:indexPath animated:YES];
    MenuItem *item = self.datas[indexPath.section].rows[indexPath.row];
    UIStoryboard *storyboard = [UIStoryboard storyboardWithName:item.storyboard bundle:nil];
    
    BaseViewController *controller = [storyboard instantiateViewControllerWithIdentifier:item.entry];
    controller.title = item.name;
    [self.navigationController pushViewController:controller animated:YES];
}

- (CGFloat)tableView:(UITableView *)tableView heightForFooterInSection:(NSInteger)section {
    if (section == 0) { return 0;}
    return 100;
}

- (UIView *)tableView:(UITableView *)tableView viewForFooterInSection:(NSInteger)section {
    if (section == 0) { return nil;}
    UILabel *lable = [[UILabel alloc] init];
    lable.font = [UIFont systemFontOfSize:16 weight:1.5];
    lable.text = [NSString stringWithFormat:@"SDK Version: %@",[AgoraRtcEngineKit getSdkVersion]];
    lable.textAlignment = NSTextAlignmentCenter;
    return lable;
}

@end
