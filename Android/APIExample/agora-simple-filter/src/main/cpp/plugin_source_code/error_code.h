//
// Created by 张涛 on 2020/5/8.
//

#ifndef AGORAWITHBYTEDANCE_ERROR_CODE_H
#define AGORAWITHBYTEDANCE_ERROR_CODE_H

namespace agora {
    namespace extension {
        enum ERROR_CODE {
            ERROR_OK = 0,
            ERROR_NOT_INIT_RTC_ENGINE = 1,
            ERROR_NOT_INIT_VIDEO_FILTER = 2,
            ERR_NOT_INIT_EXTENSION_PROVIDER = 3,
            ERROR_ERR_PARAMETER = 10,
            ERROR_INVALID_JSON = 100,
            ERROR_INVALID_JSON_TYPE = 101,
        };
    }
}
#endif //AGORAWITHBYTEDANCE_ERROR_CODE_H
