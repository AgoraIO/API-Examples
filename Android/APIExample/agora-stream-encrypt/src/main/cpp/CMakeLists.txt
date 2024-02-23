# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

project(agora-stream-encrypt)

#link agora so
set(agora-lib-so ${PROJECT_SOURCE_DIR}/../agoraLibs/${CMAKE_ANDROID_ARCH_ABI}/libagora-rtc-sdk.so)
link_libraries(${agora-lib-so})

set(agora-ffmpeg-so ${PROJECT_SOURCE_DIR}/../agoraLibs/${CMAKE_ANDROID_ARCH_ABI}/libagora-ffmpeg.so)
link_libraries(${agora-ffmpeg-so})

set(agora-soundtouch-so ${PROJECT_SOURCE_DIR}/../agoraLibs/${CMAKE_ANDROID_ARCH_ABI}/libagora-soundtouch.so)
link_libraries(${agora-soundtouch-so})

set(agora-fdkaac-so ${PROJECT_SOURCE_DIR}/../agoraLibs/${CMAKE_ANDROID_ARCH_ABI}/libagora-fdkaac.so)
link_libraries(${agora-fdkaac-so})


set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")


# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
        agora-stream-encrypt

             # Sets the library as a shared library.
             SHARED
        # Provides a relative path to your source file(s).
        packet_processing_plugin_jni.cpp)

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_include_directories(agora-stream-encrypt PRIVATE ${PROJECT_SOURCE_DIR})

target_link_libraries( # Specifies the target library.
                        agora-stream-encrypt
                        ${agora-lib-so}
                        ${agora-ffmpeg-so}
                        ${agora-soundtouch-so}
                        ${agora-fdkaac-so}

                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib}
#                       ${GLESv2}
                        ${GLESv3}
                        EGL
                        android)