//
//  TransferVideo.h
//  AgoraShareSrceen
//
//  Created by LEB on 2021/6/9.
//

#ifndef TransferVideo_h
#define TransferVideo_h

#define MAGIC_NUMBER 0x12345678

typedef struct TransferFrame {
    int64_t magic;
    int64_t  ver;
    int64_t rotation;
    int64_t frameType;// 1 video 2 audio 3 audio mic
    int64_t type;
    int64_t width;
    int64_t height;
    int64_t length;
    int64_t timescale;
    int64_t timevalues;
} TransferFrame;

#endif /* TransferVideo_h */
