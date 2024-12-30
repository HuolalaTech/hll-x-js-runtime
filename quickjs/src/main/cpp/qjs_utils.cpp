//
// Created by nieyinyin on 2022/3/23.
//

#include <string>
#include "qjs_utils.h"
#include "../../../../../common/src/main/cpp/include/constant.h"

namespace paladin {

    QJSUtil::QJSUtil(JNIEnv *env, JSContext *ctx) {
        this->env_ = env;
        this->ctx_ = ctx;

        this->jBooleanCls_ = (jclass)(env->NewGlobalRef(env->FindClass(kjBooleanClazzStr)));
        this->jIntegerCls_ = (jclass)(env->NewGlobalRef(env->FindClass(kjIntegerClazzStr)));
        this->jDoubleCls_ = (jclass)(env->NewGlobalRef(env->FindClass(kjDoubleClazzStr)));
        this->jStringCls_ = (jclass)(env->NewGlobalRef(env->FindClass(kjStringClazzStr)));

        this->jPLDJSObjectCls_ = (jclass)(env->NewGlobalRef(env->FindClass(kPLDJSObjectClazzStr)));
        this->jPLDJSArrayCls_ = (jclass)(env->NewGlobalRef(env->FindClass(kPLDJSArrayClazzStr)));
        this->jPLDJSValueCls_ = (jclass)(env->NewGlobalRef(env->FindClass(kPLDJSValueClazzStr)));
        this->jPLDJSNumberCls_ = (jclass)(env->NewGlobalRef(env->FindClass(kPLDJSNumberClazzStr)));
        this->jPLDJSNullCls_ = (jclass)(env->NewGlobalRef(env->FindClass(kPLDJSNullClazzStr)));
        this->jPLDJSUndefinedCls_ = (jclass)(env->NewGlobalRef(env->FindClass(kPLDJSUndefinedClazzStr)));
        this->jPLDJSBooleanCls_ = (jclass)(env->NewGlobalRef(env->FindClass(kPLDJSBooleanClazzStr)));
        this->jPLDJSStringCls_ = (jclass)(env->NewGlobalRef(env->FindClass(kPLDJSStringClazzStr)));

        this->jPLDJavaFunctionCls_ = (jclass)(env->NewGlobalRef(env->FindClass(kPLDJavaFunctionStr)));
        this->jPLDNativeArrayCls_ = (jclass)(env->NewGlobalRef(env->FindClass(kPLDNativeArrayStr)));
    }

    QJSUtil::~QJSUtil() {
        env_->DeleteGlobalRef(jBooleanCls_);
        env_->DeleteGlobalRef(jIntegerCls_);
        env_->DeleteGlobalRef(jDoubleCls_);
        env_->DeleteGlobalRef(jStringCls_);

        env_->DeleteGlobalRef(jPLDJSObjectCls_);
        env_->DeleteGlobalRef(jPLDJSArrayCls_);
        env_->DeleteGlobalRef(jPLDJSValueCls_);
        env_->DeleteGlobalRef(jPLDJSNumberCls_);
        env_->DeleteGlobalRef(jPLDJSNullCls_);
        env_->DeleteGlobalRef(jPLDJSUndefinedCls_);
        env_->DeleteGlobalRef(jPLDJSBooleanCls_);
        env_->DeleteGlobalRef(jPLDJSStringCls_);

        env_->DeleteGlobalRef(jPLDJavaFunctionCls_);
        env_->DeleteGlobalRef(jPLDNativeArrayCls_);
    }

    JSValueConst QJSUtil::toJSValue(jobject obj, std::string *exception) {
        const char * logTag = "toJSValue";
        JSValue ret = JS_UNDEFINED;
        if (env_->IsInstanceOf(obj, jBooleanCls_)) { // boolean
            LOGI("%s, obj is boolean", logTag);
            jmethodID mid = env_->GetMethodID(jBooleanCls_, "booleanValue", "()Z");
            jboolean value = env_->CallBooleanMethod(obj, mid);
            ret = JS_NewBool(ctx_, value);
        } else if (env_->IsInstanceOf(obj, jIntegerCls_)) { // integer
            LOGI("%s, obj is integer", logTag);
            jmethodID intValueID = env_->GetMethodID(jIntegerCls_, "intValue", "()I");
            jint value = env_->CallIntMethod(obj, intValueID);
            ret = JS_NewInt32(ctx_, value);
        } else if (env_->IsInstanceOf(obj, jDoubleCls_)) { // double
            LOGI("%s, obj is double", logTag);
            jmethodID doubleValueID = env_->GetMethodID(jDoubleCls_, "doubleValue", "()D");
            jdouble value = env_->CallDoubleMethod(obj, doubleValueID);
            ret = JS_NewFloat64(ctx_, value);
        } else if (env_->IsInstanceOf(obj, jStringCls_)) { // string
            LOGI("%s, obj is string", logTag);
            const char *value = env_->GetStringUTFChars((jstring) (obj), nullptr);
            if (value != nullptr) {
                ret = JS_NewString(ctx_, value);
            } else {
                ret = JS_NewString(ctx_, "");
            }
        } else if (env_->IsInstanceOf(obj, jPLDJSObjectCls_)) { // JSObject
            LOGI("%s, obj is JSObject", logTag);
            jmethodID mid = env_->GetMethodID(jPLDJSObjectCls_, "value", "()Ljava/lang/String;");
            jobject tmp = env_->CallObjectMethod(obj, mid);
            const char *value = env_->GetStringUTFChars((jstring) (tmp), nullptr);
            if (value != nullptr) {
                ret = JS_ParseJSON(ctx_, value, env_->GetStringUTFLength((jstring)tmp), nullptr);
                string exception_val;
                bool hasException = checkJSException(ret, &exception_val);
                if (hasException) {
                    LOGE("%s, JS_ParseJSON has exception: %s", logTag, exception_val.c_str());
                    if (exception) *exception = exception_val;
                }
            } else {
                ret = JS_NewString(ctx_, "");
            }
            env_->DeleteLocalRef(tmp);
        } else if (env_->IsInstanceOf(obj, jPLDJSUndefinedCls_)) {
            LOGI("%s, obj is JSUndefined", logTag);
            ret = JS_UNDEFINED;
        }
        else {
            if (exception) *exception = "toJSValue, do not support this type";
            LOGE("toJSValue, do not support this type");
        }
        return ret;
    }

    jobject QJSUtil::toJavaObj(JSValue arg, string *exception) {
        const char * logTag = "toJavaObj";
        if (JS_IsBool(arg)) { // bool
            LOGI("%s, arg is bool", logTag);
            jmethodID ctor = env_->GetMethodID(jPLDJSBooleanCls_, "<init>", "(Z)V");
            return env_->NewObject(jPLDJSBooleanCls_, ctor, JS_VALUE_GET_BOOL(arg) > 0);
        } else if (JS_IsNumber(arg)) { // number
            LOGI("%s, arg is number", logTag);
            int tag = JS_VALUE_GET_TAG(arg);
            jmethodID ctor = env_->GetMethodID(jPLDJSNumberCls_, "<init>", "(D)V");
            return env_->NewObject(jPLDJSNumberCls_, ctor, (tag == JS_TAG_INT) ? JS_VALUE_GET_INT(arg) : JS_VALUE_GET_FLOAT64(arg));
        } else if (JS_IsNull(arg)) { // null
            LOGI("%s, arg is null", logTag);
            jmethodID ctor = env_->GetMethodID(jPLDJSNullCls_, "<init>", "()V");
            return env_->NewObject(jPLDJSNullCls_, ctor);
        } else if (JS_IsString(arg)) { // string
            LOGI("%s, arg is string", logTag);
            jmethodID ctor = env_->GetMethodID(jPLDJSStringCls_, "<init>", "(Ljava/lang/String;)V");
            const char *tmp = JS_ToCString(ctx_, arg);
            jobject ret = env_->NewObject(jPLDJSStringCls_, ctor, env_->NewStringUTF(tmp));
            JS_FreeCString(ctx_, tmp);
            return ret;
        } else if (JS_IsArray(ctx_, arg) || JS_IsObject(arg)) { // object or array
            if (JS_IsArray(ctx_, arg)) {
                LOGI("%s, arg is array", logTag);
            }
            LOGI("%s, arg is object", logTag);
            JSValue result = JS_JSONStringify(ctx_, arg, JS_UNDEFINED, JS_UNDEFINED);
            string exception_val;
            bool hasException = checkJSException(result, &exception_val);
            const char *tmp = "" ;
            if (hasException) {
                LOGE("%s, JS_JSONStringify fail, exception occurred, exception is %s", logTag, exception_val.c_str());
                if (exception) {
                    *exception = exception_val;
                }
            } else {
                tmp = JS_ToCString(ctx_, result);
            }
            jmethodID ctor = env_->GetMethodID(jPLDJSObjectCls_, "<init>", "(Ljava/lang/String;)V");
            LOGI("%s, current js object str: %s", logTag, tmp);
            jobject ret = env_->NewObject(jPLDJSObjectCls_, ctor, env_->NewStringUTF(tmp));
            JS_FreeCString(ctx_, tmp);
            JS_FreeValue(ctx_, result);
            return ret;
        } else if (JS_IsNull(arg)) {
            LOGI("%s, arg is null", logTag);
            jmethodID ctor = env_->GetMethodID(jPLDJSNullCls_, "<init>", "()V");
            return env_->NewObject(jPLDJSNullCls_, ctor);
        } else if (JS_IsUndefined(arg)) {
            LOGI("%s, arg is undefined", logTag);
            jmethodID  ctor = env_->GetMethodID(jPLDJSUndefinedCls_, "<init>", "()V");
            return env_->NewObject(jPLDJSUndefinedCls_, ctor);
        } else if (JS_IsException(arg)) {
            LOGI("%s, arg is exception, do not support this type", logTag);
            *exception = "arg is exception, do not support this type";
        } else if (JS_IsError(ctx_, arg)) {
            LOGI("%s, arg is error, do not support this type", logTag);
            *exception = "arg is error, do not support this type";
        } else {
            LOGI("%s, arg is other type, do not support this type", logTag);
            *exception = "arg is other type, do not support this type";
        }
        return nullptr;
    }

    jobjectArray QJSUtil::getDataField(jobject pldNativeArr) {
        if (env_ == nullptr || pldNativeArr == nullptr) return nullptr;
        jclass jPLDNativeArray = env_->FindClass(kPLDNativeArrayStr);
        jfieldID dataFieldId = env_->GetFieldID(jPLDNativeArray, "data", "[Ljava/lang/Object;");
        jobject objects = env_->GetObjectField(pldNativeArr, dataFieldId);
        env_->DeleteLocalRef(jPLDNativeArray);
        auto args = reinterpret_cast<jobjectArray>(objects);
        return args;
    }

    bool QJSUtil::JSEquals(JSValue v1, JSValue v2) {
        #if defined(JS_NAN_BOXING)
        return v1 == v2;
        #else
        return v1.tag == v2.tag && v1.u.int32 == v2.u.int32 && v1.u.float64 == v2.u.float64 && v1.u.ptr == v2.u.ptr;
        #endif
    }

    /**
     *
     * @param exception_val
     * @param exception
     * @return  whether exception occur or not
     */
    bool QJSUtil::checkJSException(JSValueConst arg, std::string *exception) {
        const char * logTag = "checkJSException";
        bool ret = false;
        if (JS_IsException(arg)) {
            LOGI("%s, arg is Exception", logTag);
            ret = true;
            if (exception) {
                JSValueConst exception_val = JS_GetException(ctx_);
                const char * str = JS_ToCString(ctx_, exception_val);
                JS_FreeValue(ctx_, exception_val);
                if (str) {
                    *exception = std::string(str);
                    JS_FreeCString(ctx_, str);
                }
            }
        } else if (JS_IsError(ctx_, arg)) {
            LOGI("%s, arg is Error", logTag);
            ret = true;
            if (exception) {
                const char *str;
                JSValueConst exception_val = JS_GetException(ctx_);
                JSValueConst error_val = JS_GetPropertyStr(ctx_, exception_val, "stack");
                if (JS_IsUndefined(error_val)) {
                    str = JS_ToCString(ctx_, exception_val);
                } else {
                    str = JS_ToCString(ctx_, error_val);
                }
                JS_FreeValue(ctx_, exception_val);
                JS_FreeValue(ctx_, error_val);
                if (str) {
                    *exception = std::string(str);
                    JS_FreeCString(ctx_, str);
                }
            }
        } else {
            LOGI("%s, arg is not Exception or Error", logTag);
        }
        return ret;
    }
} // namespace paladin

