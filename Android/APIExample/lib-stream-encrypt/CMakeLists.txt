# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

add_library(apm-plugin-packet-processing SHARED
    ./src/main/cpp/packet_processing_plugin_jni.cpp
)

include_directories(./src/main/cpp/include)

find_library(log-lib log)

target_link_libraries(apm-plugin-packet-processing ${log-lib})