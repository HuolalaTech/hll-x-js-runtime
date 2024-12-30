//
// Created by nieyinyin on 2023/10/30.
//

#include <jni.h>
#include "log.h"

#ifdef __cplusplus
extern "C" {
#endif

static jlong
com_paladin_jsruntime_v8_V8Executor_native_init(
        JNIEnv * env, jclass clazz);

static void
com_paladin_jsruntime_v8_V8Executor_native_release(
        JNIEnv* env, jclass clazz, jlong ptr);

static void
com_paladin_jsruntime_v8_V8Executor_evaluateVoidScript(
        JNIEnv *env,
        jclass clazz,
        jlong ptr,
        jstring script,
        jstring scriptUrl);

static jstring
com_paladin_jsruntime_v8_V8Executor_evaluateStringScript(
        JNIEnv *env,
        jclass clazz,
        jlong ptr,
        jstring script,
        jstring scriptUrl);

static jobject
com_paladin_jsruntime_v8_V8Executor_callJavaScriptFunc(
        JNIEnv *env,
        jclass clazz,
        jlong ptr,
        jstring thisObj,
        jstring methodId,
        jobject pldNativeArray
);

static void
com_paladin_jsruntime_v8_V8Executor_registerFunc(
        JNIEnv *env,
        jclass clazz,
        jlong ptr,
        jstring funcName,
        jobject javaFunc
);

static void
com_paladin_jsruntime_v8_V8Executor_protectJSObj(
        JNIEnv *env,
        jclass clazz,
        jlong ptr,
        jstring objName);

static void
com_paladin_jsruntime_v8_V8Executor_unprotectJSObj(
        JNIEnv *env,
        jclass clazz,
        jlong ptr,
        jstring objName);

static void
com_paladin_jsruntime_v8_V8Executor_gc(
        JNIEnv *env,
        jclass clazz,
        jlong ptr);

static jlong
com_paladin_jsruntime_v8_V8Executor_createInspector(
        JNIEnv *env,
        jclass clazz,
        jlong ptr,
        jobject inspector,
        jstring protocolMessage);

static void
com_paladin_jsruntime_v8_V8Executor_dispatchProtocolMessage(
        JNIEnv *env,
        jclass clazz,
        jlong ptr,
        jlong inspectorPtr,
        jstring protocolMessage);

static void
com_paladin_jsruntime_v8_V8Executor_schedulePauseOnNextStatement(
        JNIEnv *env,
        jclass clazz,
        jlong ptr,
        jlong inspectorPtr,
        jstring reason);
#ifdef __cplusplus
}
#endif