cmake_minimum_required(VERSION 3.10.2)
project(v8_runtime)

set(PACKAGE_NAME v8_runtime)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")

set(JS_RUNTIME_SRCS
        jni/com_paladin_jsruntime_v8_V8Executor.cpp
        v8_executor.cpp
        v8_utils.cpp
        V8Inspector.cpp
        ../../../../common/src/main/cpp/utils.cpp
        )

include_directories(PUBLIC
        include
        include/v8
        include/v8/cppgc
        include/v8/cppgc/libplatform
        include/v8/cppgc/internal
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

find_library( # Sets the name of the path variable.
        android-lib
        # Specifies the name of the NDK library that
        # you want CMake to locate.
        android )

#if("${ANDROID_ABI}" MATCHES "arm64-v8a$" OR "${ANDROID_ABI}" MATCHES "x86_64$")
#     Enable pointer compression on 64 bit platforms
#    target_compile_definitions(${PACKAGE_NAME} PRIVATE V8_COMPRESS_POINTERS V8_31BIT_SMIS_ON_64BIT_ARCH)
#endif()

if(${ANDROID_ABI} STREQUAL "arm64-v8a" OR ${ANDROID_ABI} STREQUAL "x86_64")
    string(APPEND CMAKE_CXX_FLAGS " -DV8_COMPRESS_POINTERS")
endif()

target_link_libraries(${PACKAGE_NAME}
        ${CMAKE_SOURCE_DIR}/../jniLibs/${ANDROID_ABI}/libv8android.so
        ${log-lib}
        ${android-lib}
        )
