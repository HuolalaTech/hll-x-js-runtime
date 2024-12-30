//
// Created by nieyinyin on 2021/11/10.
//

#ifndef JS_RUNTIME_JSC_UTILS_H
#define JS_RUNTIME_JSC_UTILS_H

#include <string>
#include <jni.h>
//#include "../include/JavaScriptCore/JSBase.h"
#include "JSBase.h"

namespace paladin {

class JSCUtils {
public:
    static bool JSValueToStdString(JSContextRef ctx, JSValueRef value, std::string *result) ;

    static bool JSStringToStdString(JSContextRef ctx, JSStringRef value, std::string *result) ;

    static void printJSPropertyNameArrayRef(JSContextRef ctx, JSObjectRef obj);


    static JSObjectRef toObjectFromValue(JSContextRef ctx, JSValueRef js_value);

    static JSValueRef getPropertyValueFromObject(JSContextRef ctx, const std::string &property_id, JSObjectRef object);

    static jobjectArray getJavaObjArrFromPLDNativeArray(JNIEnv *env, jobject pldNativeArray);

    static void javaObjArrayToJSValueArray(JNIEnv *env, JSContextRef ctx, jobjectArray args, JSValueRef *jsValues) ;

    static jobject jsValueToJavaObject(JNIEnv *env, JSContextRef ctx, JSValueRef jsValueResult);
};

} // namespace paladin

#endif //JS_RUNTIME_JSC_UTILS_H
