# the minimum version of CMake.
cmake_minimum_required(VERSION 3.4.1)
project(RawDtaProcessing)

set(ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR})

if(DEFINED PACKAGE_FIND_FILE)
    include(${PACKAGE_FIND_FILE})
endif()

include_directories(${ROOT_PATH}
                    ${ROOT_PATH}/include)

set(AGORA_LIB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../oh_modules/AgoraRtcSdk/libs)

add_library(agora_rtc_sdk SHARED IMPORTED)
set_target_properties(agora_rtc_sdk PROPERTIES IMPORTED_LOCATION ${AGORA_LIB_PATH}/${CMAKE_OHOS_ARCH_ABI}/libagora_rtc_sdk.so)

add_library(rawdataprocessing SHARED
                napi_init.cpp
                audio_render.cpp
                origin_audio_data.cpp
                origin_video_data.cpp
            )
target_link_libraries(rawdataprocessing PUBLIC
                    agora_rtc_sdk
                    libace_napi.z.so
                    libohaudio.so
                    libhilog_ndk.z.so  )