// Created by nieyinyin on 2023/10/30.
//

#include "../../../../../common/src/main/cpp/include/utils.h"
#include <v8_executor.h>
#include <libplatform.h>
#include "com_paladin_jsruntime_v8_V8Executor.h"
#include "../../../../../common/src/main/cpp/include/constant.h"
#include "v8.h"

using namespace paladin;

static JNINativeMethod gMethods[] = {
        {"nativeInit", "()J", (void *) com_paladin_jsruntime_v8_V8Executor_native_init},
        {"nativeRelease", "(J)V", (void *) com_paladin_jsruntime_v8_V8Executor_native_release},
        {"nativeEvaluateVoidScript", "(JLjava/lang/String;Ljava/lang/String;)V", (void *)com_paladin_jsruntime_v8_V8Executor_evaluateVoidScript},
        {"nativeEvaluateStringScript", "(JLjava/lang/String;Ljava/lang/String;)Ljava/lang/String;", (void *) com_paladin_jsruntime_v8_V8Executor_evaluateStringScript},
        {"nativeCallJavaScriptFunc", "(JLjava/lang/String;Ljava/lang/String;Lcom/paladin/jsruntime/common/PLDNativeArray;)Lcom/paladin/jsruntime/common/jsvalue/JSValue;", (void *) com_paladin_jsruntime_v8_V8Executor_callJavaScriptFunc},
        {"nativeRegisterFunc", "(JLjava/lang/String;Lcom/paladin/jsruntime/common/JavaFunction;)V", (void *) com_paladin_jsruntime_v8_V8Executor_registerFunc},
        {"nativeProtectJSObj", "(JLjava/lang/String;)V", (void *) com_paladin_jsruntime_v8_V8Executor_protectJSObj},
        {"nativeUnprotectJSObj", "(JLjava/lang/String;)V", (void *) com_paladin_jsruntime_v8_V8Executor_unprotectJSObj},
        {"nativeGc", "(J)V", (void *) com_paladin_jsruntime_v8_V8Executor_gc},
        {"nativeCreateInspector", "(JLcom/paladin/jsruntime/v8/inspector/V8InspectorDelegate;Ljava/lang/String;)J", (void *) com_paladin_jsruntime_v8_V8Executor_createInspector},
        {"nativeDispatchProtocolMessage", "(JJLjava/lang/String;)V", (void *) com_paladin_jsruntime_v8_V8Executor_dispatchProtocolMessage},
        {"nativeSchedulePauseOnNextStatement", "(JJLjava/lang/String;)V", (void *) com_paladin_jsruntime_v8_V8Executor_schedulePauseOnNextStatement},
};

static struct {
    jfieldID mPtr;   // native object attached to the DVM JSExecutor
} gJSExecutorClassInfo;

std::unique_ptr<v8::Platform> v8Platform = nullptr;

JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    JNIEnv* env;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }

    // init v8
    v8::V8::InitializeICU();
//    v8::V8::InitializeExternalStartupData("");
    v8Platform = v8::platform::NewDefaultPlatform();
    v8::V8::InitializePlatform(v8Platform.get());
    v8::V8::Initialize();

    jclass c = env->FindClass("com/paladin/jsruntime/v8/V8Executor");
    if (c == nullptr) return JNI_ERR;
    int rc = env->RegisterNatives(c, gMethods, sizeof(gMethods)/sizeof(JNINativeMethod));
    gJSExecutorClassInfo.mPtr = env->GetFieldID(c, "mPtr", "J");
    if (rc != JNI_OK) return rc;

    return JNI_VERSION_1_6;
}

JNIEXPORT void JNI_OnUnload(JavaVM *vm, void *reserved) {

    // dispose v8
    v8::V8::Dispose();
    v8::V8::DisposePlatform();
}

static jlong
com_paladin_jsruntime_v8_V8Executor_native_init(JNIEnv *env,
                                                          jclass thiz)
{
    return reinterpret_cast<jlong>(new V8Executor(env));
}

static void
com_paladin_jsruntime_v8_V8Executor_native_release(JNIEnv* env, jclass clazz, jlong ptr)
{
    auto* jsExecutor = reinterpret_cast<JSExecutor*>(ptr);
    jsExecutor->~JSExecutor();
}

static void
com_paladin_jsruntime_v8_V8Executor_evaluateVoidScript(
        JNIEnv *env,
        jclass clazz,
        jlong ptr,
        jstring script,
        jstring scriptUrl) {
    auto* jsExecutor = reinterpret_cast<JSExecutor*>(ptr);
    const char * _script = env->GetStringUTFChars(script, nullptr);
    const char * _scriptUrl = env->GetStringUTFChars(scriptUrl, nullptr);
    std::string exception;
    jsExecutor->evaluateVoidScript(std::string(_script), std::string(_scriptUrl), &exception);
    if (!exception.empty()) {
        LOGE("jni evaluateVoidScript error: %s", exception.c_str());
        paladin::Utils::throwJSRuntimeException(env, exception.c_str());
    }
    env->ReleaseStringUTFChars(script, _script);
    env->ReleaseStringUTFChars(script, _scriptUrl);
}

static jstring
com_paladin_jsruntime_v8_V8Executor_evaluateStringScript(
        JNIEnv *env,
        jclass clazz,
        jlong ptr,
        jstring script,
        jstring scriptUrl) {
    auto* jsExecutor = reinterpret_cast<JSExecutor*>(ptr);
    const char * _script = env->GetStringUTFChars(script, nullptr);
    const char * _scriptUrl = env->GetStringUTFChars(scriptUrl, nullptr);
    std::string result;
    std::string exception;
    bool isSuccess = jsExecutor->evaluateStringScript(std::string(_script), std::string(_scriptUrl), &result, &exception);
    env->ReleaseStringUTFChars(script, _script);
    env->ReleaseStringUTFChars(script, _scriptUrl);

    if (isSuccess) {
        return env->NewStringUTF(result.c_str());
    } else {
        if (!exception.empty()) {
            LOGE("jni evaluateStringScript, error: %s", exception.c_str());
            paladin::Utils::throwJSRuntimeException(env, exception.c_str());
        }
        return nullptr;
    }
}

static jobject
com_paladin_jsruntime_v8_V8Executor_callJavaScriptFunc(
        JNIEnv *env,
        jclass clazz,
        jlong ptr,
        jstring thisObj,
        jstring methodId,
        jobject args)
{
    auto* jsExecutor = reinterpret_cast<JSExecutor*>(ptr);
    const char *_thisObj = env->GetStringUTFChars(thisObj, nullptr);
    const char *_methodId = env->GetStringUTFChars(methodId, nullptr);
    const std::string cxx_thisObj = std::string(_thisObj);
    const std::string cxx_methodId = std::string(_methodId);
    std::string exception;
    LOGI("jni callJavaScriptFunc start, thisObj=%s, func=%s", _thisObj, _methodId);
    jobject result = jsExecutor->callJavaScriptFunction(env, cxx_thisObj, cxx_methodId, args, &exception);
    LOGI("jni callJavaScriptFunc, end");
    env->ReleaseStringUTFChars(thisObj, _thisObj);
    env->ReleaseStringUTFChars(methodId, _methodId);

    if (!exception.empty()) {
        LOGE("jni callJavaScriptFunc: %s", exception.c_str());
        paladin::Utils::throwJSRuntimeException(env, exception.c_str());
    }
    return result;
}

static void
com_paladin_jsruntime_v8_V8Executor_registerFunc(
        JNIEnv *env,
        jclass clazz,
        jlong ptr,
        jstring funcName,
        jobject javaFunc
) {
    auto* jsExecutor = reinterpret_cast<JSExecutor*>(ptr);
    const char * _funcName = env->GetStringUTFChars(funcName, nullptr);
    std::string exception;
    jsExecutor->registerFunction(env, std::string(_funcName), javaFunc, &exception);
    if (!exception.empty()) {
        LOGE("jni registerFunc: %s", exception.c_str());
        paladin::Utils::throwJSRuntimeException(env, exception.c_str());
    }
    env->ReleaseStringUTFChars(funcName, _funcName);
}

static void
com_paladin_jsruntime_v8_V8Executor_protectJSObj(
        JNIEnv *env,
        jclass clazz,
        jlong ptr,
        jstring objName) {
    auto* jsExecutor = reinterpret_cast<JSExecutor*>(ptr);
    const char * _objName = env->GetStringUTFChars(objName, nullptr);
    std::string exception;
    jsExecutor->protectJSObj(std::string(_objName), &exception);
    env->ReleaseStringUTFChars(objName, _objName);
    if (!exception.empty()) {
        LOGE("jni protectJSObj error: %s", exception.c_str());
        paladin::Utils::throwJSRuntimeException(env, exception.c_str());
    }
}

static void
com_paladin_jsruntime_v8_V8Executor_unprotectJSObj(
        JNIEnv *env,
        jclass clazz,
        jlong ptr,
        jstring objName) {
    auto* jsExecutor = reinterpret_cast<JSExecutor*>(ptr);
    const char *_objName = env->GetStringUTFChars(objName, nullptr);
    std::string exception;
    jsExecutor->unprotectJSObj(std::string(_objName), &exception);
    if (!exception.empty()) {
        LOGE("jni unprotectJSObj error: %s", exception.c_str());
        paladin::Utils::throwJSRuntimeException(env, exception.c_str());
    }
    env->ReleaseStringUTFChars(objName, _objName);
}

static void
com_paladin_jsruntime_v8_V8Executor_gc(
        JNIEnv *env,
        jclass clazz,
        jlong ptr
        ) {
    auto* jsExecutor = reinterpret_cast<JSExecutor*>(ptr);
    jsExecutor->gc();
}

static jlong
com_paladin_jsruntime_v8_V8Executor_createInspector(
        JNIEnv *env,
        jclass clazz,
        jlong ptr,
        jobject inspector,
        jstring contextName) {
    auto* v8 = reinterpret_cast<V8Executor*>(ptr);
    const char *_contextName = env->GetStringUTFChars(contextName, nullptr);
    long inspectorPtr = v8->createInspector(inspector, v8Platform, std::string(_contextName));
    env->ReleaseStringUTFChars(contextName, _contextName);
    return inspectorPtr;
}

static void
com_paladin_jsruntime_v8_V8Executor_dispatchProtocolMessage(
        JNIEnv *env,
        jclass clazz,
        jlong ptr,
        jlong inspectorPtr,
        jstring protocolMessage) {
    auto* v8 = reinterpret_cast<V8Executor*>(ptr);
    const char *_protocolMessage = env->GetStringUTFChars(protocolMessage, nullptr);
    v8->dispatchProtocolMessage(std::string(_protocolMessage));
    env->ReleaseStringUTFChars(protocolMessage, _protocolMessage);
}

static void
com_paladin_jsruntime_v8_V8Executor_schedulePauseOnNextStatement(
        JNIEnv *env,
        jclass clazz,
        jlong ptr,
        jlong inspectorPtr,
        jstring reason) {
    auto* v8 = reinterpret_cast<V8Executor*>(ptr);
    const char *_reason = env->GetStringUTFChars(reason, nullptr);
    v8->schedulePauseOnNextStatement(std::string(_reason));
    env->ReleaseStringUTFChars(reason, _reason);
}






