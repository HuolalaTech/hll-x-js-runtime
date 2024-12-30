//
// Created by nieyinyin on 2021/11/12.
//

#include <jni.h>
#include "log.h"

#ifdef __cplusplus
extern "C" {
#endif

static jlong
com_paladin_jsruntime_qjs_QJSExecutor_native_init(
        JNIEnv * env, jclass clazz);

static void
com_paladin_jsruntime_qjs_QJSExecutor_native_release(
        JNIEnv* env, jclass clazz, jlong ptr);

static void
com_paladin_jsruntime_qjs_QJSExecutor_evaluateVoidScript(
        JNIEnv *env,
        jclass clazz,
        jlong ptr,
        jstring script,
        jstring scriptUrl);

static jstring
com_paladin_jsruntime_qjs_QJSExecutor_evaluateStringScript(
        JNIEnv *env,
        jclass clazz,
        jlong ptr,
        jstring script,
        jstring scriptUrl);

static jobject
com_paladin_jsruntime_qjs_QJSExecutor_callJavaScriptFunc(
        JNIEnv *env,
        jclass clazz,
        jlong ptr,
        jstring thisObj,
        jstring methodId,
        jobject pldNativeArray
);

static void
com_paladin_jsruntime_qjs_QJSExecutor_registerFunc(
        JNIEnv *env,
        jclass clazz,
        jlong ptr,
        jstring funcName,
        jobject javaFunc
);

static void
com_paladin_jsruntime_qjs_QJSExecutor_protectJSObj(
        JNIEnv *env,
        jclass clazz,
        jlong ptr,
        jstring objName);

static void
com_paladin_jsruntime_qjs_QJSExecutor_unprotectJSObj(
        JNIEnv *env,
        jclass clazz,
        jlong ptr,
        jstring objName);

static void
com_paladin_jsruntime_qjs_QJSExecutor_gc(
        JNIEnv *env,
        jclass clazz,
        jlong ptr);
#ifdef __cplusplus
}
#endif