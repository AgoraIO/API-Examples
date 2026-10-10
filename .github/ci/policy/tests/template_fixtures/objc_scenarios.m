static void drain(void) {
    __block BOOL done = NO;
    dispatch_async(dispatch_get_main_queue(), ^{ done = YES; });
    while (!done) [[NSRunLoop currentRunLoop] runUntilDate:[NSDate dateWithTimeIntervalSinceNow:0.001]];
}
static void request(TemplateMain *sut, NSString *channel) {
    [sut requestJoin:channel requestPermission:^(void (^completion)(BOOL)) { completion(YES); }];
    drain();
}
static void respond(NSInteger index, NSString *token) {
    dispatch_sync(dispatch_get_global_queue(QOS_CLASS_DEFAULT, 0), ^{
        [[NetworkManager shared] respond:index token:token];
    });
    drain();
}
int main(void) { @autoreleasepool {
    TemplateMain *sut = [TemplateMain new];
    [sut viewDidLoad]; [sut setupRTC];
    assert(engines.count == 1);
    AgoraRtcEngineKit *first = engines.lastObject;
    request(sut, @"old");
    [sut onDestroy]; [sut onDestroy]; respond(0, @"test-response");
    assert(joins == 0 && destroys == 1 && first.leaves == 1);
    [sut setupRTC]; request(sut, @"fresh"); respond(0, @"test-response"); assert(joins == 0);
    respond(1, @"test-response"); assert(joins == 1 && [lastChannel isEqual:@"fresh"] && lastUID == 0);
    [sut leaveChannel]; request(sut, @"leaving"); [sut leaveChannel]; respond(2, @"test-response");
    assert(joins == 1);
    request(sut, @"superseded"); request(sut, @"latest");
    respond(4, @"test-response"); respond(3, @"test-response");
    assert(joins == 2 && [lastChannel isEqual:@"latest"]);
    __block void (^permission)(BOOL);
    [sut requestJoin:@"permission-old" requestPermission:^(void (^completion)(BOOL)) { permission = completion; }];
    NSInteger count = [NetworkManager shared].requestCount;
    [sut onDestroy]; [sut setupRTC]; permission(YES); drain();
    assert([NetworkManager shared].requestCount == count);
    [sut requestJoin:@"denied" requestPermission:^(void (^completion)(BOOL)) { completion(NO); }];
    drain(); assert([NetworkManager shared].requestCount == count);
    certificate = @"enabled-in-test";
    request(sut, @"missing-token"); respond(count, nil); assert(joins == 2);
    certificate = nil;
    request(sut, @"no-certificate"); respond(count + 1, nil); assert(joins == 3);
    joinResult = -1;
    request(sut, @"join-error"); respond(count + 2, @"test-response");
    [sut onDestroy];
    puts("PASS: OC bridge selector, pending exit, repeat cleanup/setup, reopen, leave, reordered responses, permission and token failures");
} }
