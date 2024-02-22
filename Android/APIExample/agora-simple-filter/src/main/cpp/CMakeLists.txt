# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

project(agora-simple-filter)

#link agora so
set(agora-lib-so ${PROJECT_SOURCE_DIR}/../agoraLibs/${CMAKE_ANDROID_ARCH_ABI}/libagora-rtc-sdk.so)
link_libraries(${agora-lib-so})

#link opencv so
set(opencv-lib-so ${PROJECT_SOURCE_DIR}/../jniLibs/${CMAKE_ANDROID_ARCH_ABI}/libopencv_java4.so)
link_libraries(${opencv-lib-so})

#opencv inc
set(opencv-inc ${PROJECT_SOURCE_DIR}/../cpp/plugin_source_code)
include_directories(${opencv-inc})

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")


# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
        agora-simple-filter

             # Sets the library as a shared library.
             SHARED
        plugin_source_code/ExtensionProvider.cpp
        plugin_source_code/ExtensionVideoFilter.cpp
        plugin_source_code/ExtensionAudioFilter.cpp
        plugin_source_code/EGLCore.cpp
        plugin_source_code/JniHelper.cpp
        plugin_source_code/VideoProcessor.cpp
        plugin_source_code/AudioProcessor.cpp
        plugin_source_code/external_thread_pool.cpp
        # Provides a relative path to your source file(s).
        agora-simple-filter.cpp)

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

target_include_directories(agora-simple-filter PRIVATE ${PROJECT_SOURCE_DIR})
target_include_directories(agora-simple-filter PRIVATE ${PROJECT_SOURCE_DIR}/AgoraRtcKit)

target_link_libraries( # Specifies the target library.
                        agora-simple-filter
                        ${agora-lib-so}
                        ${agora-ffmpeg-so}
                        ${agora-soundtouch-so}
                        ${agora-fdkaac-so}

                        ${opencv-lib-so}
                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib}
#                       ${GLESv2}
                        ${GLESv3}
                        EGL
                        android)