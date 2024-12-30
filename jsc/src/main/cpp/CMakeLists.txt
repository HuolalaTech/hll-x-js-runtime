cmake_minimum_required(VERSION 3.10.2)
project(jsc_runtime)

set(PACKAGE_NAME jsc_runtime)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")

set(JS_RUNTIME_SRCS
        jni/com_paladin_jsruntime_jsc_JSCExecutor.cpp
        jsc_executor.cpp
        jsc_utils.cpp
        ../../../../common/src/main/cpp/utils.cpp
        )

include_directories(PUBLIC
        include/JavaScriptCore
        include
        jni
        ../../../../common/src/main/cpp/include
        )

message(${CMAKE_SOURCE_DIR})

file(GLOB_RECURSE allCPP
        *.cpp
        *.c)

add_library(
        ${PACKAGE_NAME}
        SHARED
        ${JS_RUNTIME_SRCS}
)

find_library(
        log-lib
        log)

target_link_libraries(${PACKAGE_NAME}
        ${CMAKE_SOURCE_DIR}/../jniLibs/${ANDROID_ABI}/libjsc.so
        ${log-lib}
        )
