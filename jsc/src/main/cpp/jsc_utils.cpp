//
// Created by nieyinyin on 2021/11/10.
//

#include "jsc_utils.h"
#include "JSStringRef.h"
#include "../../../../common/src/main/cpp/include/log.h"
#include "../../../../common/src/main/cpp/include/constant.h"
#include <iostream>
#include <JSObjectRef.h>

namespace paladin {
     bool paladin::JSCUtils::JSValueToStdString(JSContextRef ctx, JSValueRef value, std::string *result) {
        if (nullptr == value || JSValueIsNull(ctx, value)) {
            return false;
        }
        JSValueRef exc;
        JSStringRef str = JSValueToStringCopy(ctx, value, &exc);
        size_t max_bytes = JSStringGetMaximumUTF8CStringSize(str);
        result->resize(max_bytes);
        size_t bytes_written = JSStringGetUTF8CString(str, &(*result)[0], max_bytes);
        if (max_bytes == 0) {
            return false;
        }
        result->resize(bytes_written - 1);
        JSStringRelease(str);
        return true;
    }

    bool JSCUtils::JSStringToStdString(JSContextRef ctx, JSStringRef str, std::string *result) {
        if (str == nullptr) return false;

        size_t max_bytes = JSStringGetMaximumUTF8CStringSize(str);
        result->resize(max_bytes);
        size_t bytes_written = JSStringGetUTF8CString(str, &(*result)[0], max_bytes);
        if (max_bytes == 0) {
            return false;
        }
        result->resize(bytes_written - 1);
        JSStringRelease(str);
        return true;
    }

    /**
     * this function cost large time.
     * @param ctx
     * @param obj
     */
    void JSCUtils::printJSPropertyNameArrayRef(JSContextRef ctx, JSObjectRef obj) {
        JSPropertyNameArrayRef arrRef = JSObjectCopyPropertyNames(ctx, obj);
        size_t arrRefCount = JSPropertyNameArrayGetCount(arrRef);
        for (size_t i = 0; i < arrRefCount; ++i) {
            JSStringRef propertyName = JSPropertyNameArrayGetNameAtIndex(arrRef, i);
            std::string result;
            JSCUtils::JSStringToStdString(ctx, propertyName, &result);
            LOGE("--------->%s", result.c_str());
        }
        JSPropertyNameArrayRelease(arrRef);

    }

    JSObjectRef JSCUtils::toObjectFromValue(JSContextRef ctx, JSValueRef js_value) {
        if (nullptr == js_value) {
            LOGE("toObjectFromValue: js_value must not be nullptr!");
            return nullptr;
        }
        JSValueRef exc = nullptr;
        JSObjectRef convertObjectRef = JSValueToObject(ctx, js_value, &exc);
        if (convertObjectRef) {
            return convertObjectRef;
        }
        return nullptr;
    }

    JSValueRef JSCUtils::getPropertyValueFromObject(JSContextRef ctx, const std::string& property_id, JSObjectRef object) {
        if (nullptr == object) {
            LOGE("getPropertyValueFromObject check object failed from id:%s",
                 property_id.c_str());
            return nullptr;
        }
        JSStringRef str = JSStringCreateWithUTF8CString(property_id.c_str());
        JSValueRef exc = nullptr;
        JSValueRef resultValue = JSObjectGetProperty(ctx, object, str, &exc);
//        JSValueRef resultValue = JSObjectGetPropertyForKey(context_, object, JSValueMakeString(context_, str), &exc);
        if (exc) {
            std::string exception;
            JSCUtils::JSValueToStdString(ctx, exc, &exception);
            LOGE("GetPropertyValueFromObject fail, exec:%s", exception.c_str());
        }
        JSStringRelease(str);
        if (resultValue && !JSValueIsUndefined(ctx, resultValue)) {
            return resultValue;
        }
        return nullptr;
    }

    void JSCUtils::javaObjArrayToJSValueArray(JNIEnv *env, JSContextRef ctx, jobjectArray args, JSValueRef *jsValues) {
        jsize argsCount;
        if (args == nullptr) LOGE("callJavaScriptFunction: args is nullptr");

        if (args != nullptr) {
            argsCount = env->GetArrayLength(args);
            jclass integerClass = env->FindClass(kjIntegerClazzStr);
            jclass doubleClass = env->FindClass(kjDoubleClazzStr);
            jclass jStringClass = env->FindClass(kjStringClazzStr);
            jclass jJSObjectClass = env->FindClass(kPLDJSObjectClazzStr);
            jclass jBooleanClazz = env->FindClass(kjBooleanClazzStr);

            for (int i = 0; i < argsCount; ++i) {
                jobject valueObj = env->GetObjectArrayElement(args, i);
                if (env->IsInstanceOf(valueObj, jBooleanClazz)) {
                    jmethodID  mid = env->GetMethodID(jBooleanClazz, "booleanValue", "()Z");
                    jboolean value = env->CallBooleanMethod(valueObj, mid);
                    jsValues[i] = JSValueMakeBoolean(ctx, value);
                } else if (env->IsInstanceOf(valueObj, integerClass)) { // Integer
                    jmethodID intValueID =
                            env->GetMethodID(integerClass, "intValue", "()I");
                    jint value = env->CallIntMethod(valueObj, intValueID);
                    LOGI("javaObjArrayToJSValueArray, the %dth arg is : %d", i, value);
                    jsValues[i] = JSValueMakeNumber(ctx, value);
                } else if (env->IsInstanceOf(valueObj, doubleClass)) { // Double
                    jmethodID doubleValueID =
                            env->GetMethodID(doubleClass, "doubleValue", "()D");
                    jdouble value = env->CallDoubleMethod(valueObj, doubleValueID);
                    LOGI("javaObjArrayToJSValueArray, the %dth arg is : %f", i, value);
                    jsValues[i] = JSValueMakeNumber(ctx, value);
                } else if (env->IsInstanceOf(valueObj, jStringClass)) { // String
                    const char *value = env->GetStringUTFChars((jstring)(valueObj), nullptr);
                    if (value == nullptr) {
                        jsValues[i] = JSValueMakeNull(ctx);
                    } else {
                        JSStringRef str = JSStringCreateWithUTF8CString(value);
                        jsValues[i] = JSValueMakeString(ctx, str);
                        JSStringRelease(str);
                    }
                    env->ReleaseStringUTFChars((jstring)(valueObj), value);
                    LOGI("javaObjArrayToJSValueArray, the %dth arg is : %s", i, value);
                } else if (env->IsInstanceOf(valueObj, jJSObjectClass)) { // JSObject
                    jmethodID mid = env->GetMethodID(jJSObjectClass, "value", "()Ljava/lang/String;");
                    jobject obj = env->CallObjectMethod(valueObj, mid);
                    const char *value = env->GetStringUTFChars((jstring)(obj), nullptr);
                    if (value == nullptr) {
                        jsValues[i] = JSValueMakeNull(ctx);
                    } else {
                        JSStringRef str = JSStringCreateWithUTF8CString(value);
                        jsValues[i] = JSValueMakeFromJSONString(ctx, str);
                        JSStringRelease(str);
                    }
                    LOGI("javaObjArrayToJSValueArray, the %dth arg is JSObject, it's value is %s", i, value);
                    env->ReleaseStringUTFChars((jstring)(obj), value);
                    env->DeleteLocalRef(obj);
                }
                else { //other

                }

                env->DeleteLocalRef(valueObj);

                JSValueProtect(ctx, jsValues[i]);

            }

            env->DeleteLocalRef(integerClass);
            env->DeleteLocalRef(doubleClass);
            env->DeleteLocalRef(jStringClass);
            env->DeleteLocalRef(jJSObjectClass);
            env->DeleteLocalRef(jBooleanClazz);
        }
    }

    /**
     * @param env
     * @param ctx
     * @param jsValueResult
     * @return
     */
    jobject JSCUtils::jsValueToJavaObject(JNIEnv *env, JSContextRef ctx, JSValueRef jsValueRef) {
        // convert JSValue to jobject
        if (JSValueIsObject(ctx, jsValueRef)) { // array and object
            std::string jsonStr;
            JSStringRef jsString = JSValueCreateJSONString(ctx, jsValueRef, 0, nullptr);
            JSCUtils::JSStringToStdString(ctx, jsString, &jsonStr);
            jclass jsObjClazz = env->FindClass(kPLDJSObjectClazzStr);
            jmethodID  jsObjConstructor = env->GetMethodID(jsObjClazz, "<init>","(Ljava/lang/String;)V");
            jstring val = env->NewStringUTF(jsonStr.c_str());
            LOGI("jsValueToJavaObject, jsValueResult is object, value is: %s", jsonStr.c_str());
            if (JSValueIsArray(ctx, jsValueRef)) {
                LOGI("jsValueToJavaObject, jsValueResult is array");
            }
            return env->NewObject(jsObjClazz, jsObjConstructor, val);
        } else if (JSValueIsNumber(ctx, jsValueRef)) {
            double d = JSValueToNumber(ctx, jsValueRef, nullptr);
            jclass clazz = env->FindClass(kPLDJSNumberClazzStr);
            jmethodID ctor = env->GetMethodID(clazz, "<init>", "(D)V");
            LOGI("jsValueToJavaObject, jsValueResult is number, value is : %f", d);
            return env->NewObject(clazz, ctor, d);
        } else if (JSValueIsNull(ctx, jsValueRef)) {
            jclass clazz = env->FindClass(kPLDJSNullClazzStr);
            jmethodID ctor = env->GetMethodID(clazz, "<init>", "()V");
            LOGI("jsValueToJavaObject, jsValueResult is NULL");
            return env->NewObject(clazz, ctor);
        } else if (JSValueIsBoolean(ctx, jsValueRef)) {
            bool b = JSValueToBoolean(ctx, jsValueRef);
            jclass clazz = env->FindClass(kPLDJSBooleanClazzStr);
            jmethodID ctor = env->GetMethodID(clazz, "<init>", "(Z)V");
            LOGI("jsValueToJavaObject, jsValueResult is boolean, the value is : %d", b);
            return env->NewObject(clazz, ctor, b);
        } else if (JSValueIsString(ctx, jsValueRef)) {
            jclass clazz = env->FindClass(kPLDJSStringClazzStr);
            jmethodID ctor = env->GetMethodID(clazz, "<init>", "(Ljava/lang/String;)V");
            std::string str;
            JSCUtils::JSValueToStdString(ctx, jsValueRef, &str);
            LOGI("jsValueToJavaObject, jsValueResult is string, the value is : %s", str.c_str());
            return env->NewObject(clazz, ctor, env->NewStringUTF(str.c_str()));
        } else if (JSValueIsUndefined(ctx, jsValueRef)) {
            // do not support it.
        } else if (JSValueIsSymbol(ctx, jsValueRef)) {
            // do not support it.
        } else {
            // do not support it.
        }
        return nullptr;
    }

    jobjectArray JSCUtils::getJavaObjArrFromPLDNativeArray(JNIEnv *env, jobject pldNativeArray) {
        if (env == nullptr || pldNativeArray == nullptr) return nullptr;
        jclass jPLDNativeArray = env->FindClass(kPLDNativeArrayStr);
        jfieldID dataFieldId = env->GetFieldID(jPLDNativeArray, "data","[Ljava/lang/Object;");
        jobject objects = env->GetObjectField(pldNativeArray, dataFieldId);
        env->DeleteLocalRef(jPLDNativeArray);
        auto args = reinterpret_cast<jobjectArray>(objects);
        return args;
    }


} // namespace paladin
