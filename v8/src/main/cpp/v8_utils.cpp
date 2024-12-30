//
// Created by nieyinyin on 2023/10/30.
//

#include <string>
#include "v8_utils.h"
#include "constant.h"
#include "v8_executor.h"

using namespace v8;

namespace paladin {

    V8Util::V8Util(JNIEnv *env) {
        this->env_ = env;
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

    V8Util::~V8Util() {
        // need to delete? why j2v8 have not done this?
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

    // Extracts a C string from a V8 Utf8Value.
    const char* V8Util::ToCString(const v8::String::Utf8Value& value) {
        return *value ? *value : "<string conversion failed>";
    }

    Local<String> V8Util::createV8String(JNIEnv *env, v8::Isolate *isolate, const string &str) {
        // TODO utf8 or utf 16?
        v8::MaybeLocal<v8::String> twoByteString = v8::String::NewFromUtf8(isolate,
                                                                              str.c_str(),
                                                                              v8::NewStringType::kNormal,
                                                                              str.length());
        if (twoByteString.IsEmpty()) {
            LOGI("twoByteString is empty");
            return v8::Local<v8::String>();
        }
        v8::Local<v8::String> result = twoByteString.ToLocalChecked();
        return result;
    }

    std::string V8Util::createString(JNIEnv *env, v8::Isolate *isolate, const string &str) {
        v8::Local<v8::String> v8Str = createV8String(env, isolate, str);
        v8::String::Utf8Value stdString(isolate, v8Str);
        return ToCString(stdString);
    }

    ScriptOrigin* V8Util::createScriptOrigin(JNIEnv * env, Isolate* isolate, string &stdScriptName, int lineNumber = 0) {
        Local<String> scriptName = createV8String(env, isolate, stdScriptName);
        return new ScriptOrigin(isolate, scriptName, lineNumber);
    }

    // if compile error, will throw ParseException
    bool V8Util::compileScript(JNIEnv *env, const Local<Context>& context, Isolate *isolate,
                               const string &std_script, string *scriptName, int lineNumber,
                               Local<Script> &script, TryCatch* tryCatch, std::string *exception) {
        v8::Local<v8::String> source = createV8String(env, isolate, std_script);
        v8::ScriptOrigin* scriptOriginPtr = nullptr;
        if (scriptName != nullptr) {
            scriptOriginPtr = createScriptOrigin(env, isolate, *scriptName, lineNumber);
        }
        v8::MaybeLocal<v8::Script> script_result = v8::Script::Compile(context, source, scriptOriginPtr);
        if (!script_result.IsEmpty()) {
            script = script_result.ToLocalChecked();
            if (scriptOriginPtr != nullptr) {
                delete(scriptOriginPtr);
            }
        }
        if (tryCatch->HasCaught()) {
            getException(context, isolate, tryCatch, exception);
            return false;
        }
        return true;
    }

    bool V8Util::runScript(const v8::Local<v8::Context>& context, v8::Isolate* isolate, JNIEnv *env,
                           v8::Local<v8::Script> *script, v8::TryCatch* tryCatch, std::string *exception) {
        v8::MaybeLocal<v8::Value> local_result = (*script)->Run(context);
        if (!local_result.IsEmpty()) {
            v8::String::Utf8Value unicodeString(isolate, local_result.ToLocalChecked());
            LOGI("V8Util::runScript, return value: %s", ToCString(unicodeString));
        }
        if (tryCatch->HasCaught()) {
            getException(context, isolate, tryCatch, exception);
            return false;
        }
        return true;
    }

    bool V8Util::runScript(const v8::Local<v8::Context>& context, v8::Isolate* isolate, JNIEnv *env,
                           v8::Local<v8::Script> *script, v8::TryCatch* tryCatch,
                           v8::Local<v8::Value> &result, std::string *exception) {
        v8::MaybeLocal<v8::Value> local_result = (*script)->Run(context);
        if (!local_result.IsEmpty()) {
            result = local_result.ToLocalChecked();
            return true;
        }
        if (tryCatch->HasCaught()) {
            getException(context, isolate, tryCatch, exception);
            return false;
        }
        return true;
    }

    void V8Util::getException(const Local<Context> &context, Isolate* isolate,
                              TryCatch* tryCatch, string *excep) {
        if (!tryCatch->HasCaught()) return;
        v8::HandleScope handle_scope(isolate);
        v8::String::Utf8Value exception(isolate, tryCatch->Exception());
        const char* exception_string = ToCString(exception);
        (*excep).append(exception_string);
    }

    Local<v8::Value> V8Util::toJSValue(JNIEnv * env, Isolate *isolate, const Local<Context> &ctx, jobject obj, std::string *exception) {
        const char * logTag = "toJSValue";
        Local<v8::Value> ret = Undefined(isolate);
        if (env_->IsInstanceOf(obj, jBooleanCls_)) { // boolean
            LOGI("%s, obj is boolean", logTag);
            jmethodID mid = env->GetMethodID(jBooleanCls_, "booleanValue", "()Z");
            jboolean value = env->CallBooleanMethod(obj, mid);
            ret = Boolean::New(isolate, value);
        } else if (env->IsInstanceOf(obj, jIntegerCls_)) { // integer
            LOGI("%s, obj is integer", logTag);
            jmethodID intValueID = env->GetMethodID(jIntegerCls_, "intValue", "()I");
            jint value = env->CallIntMethod(obj, intValueID);
            ret = Int32::New(isolate, value);
        } else if (env->IsInstanceOf(obj, jDoubleCls_)) { // double
            LOGI("%s, obj is double", logTag);
            jmethodID doubleValueID = env->GetMethodID(jDoubleCls_, "doubleValue", "()D");
            jdouble value = env->CallDoubleMethod(obj, doubleValueID);
            ret = Number::New(isolate, value);
        } else if (env->IsInstanceOf(obj, jStringCls_)) { // string
            LOGI("%s, obj is string", logTag);
            const char *value = env->GetStringUTFChars((jstring) (obj), nullptr);
            ret = createV8String(env, isolate, string(value));
        } else if (env->IsInstanceOf(obj, jPLDJSObjectCls_)) { // JSObject
            LOGI("%s, obj is JSObject", logTag);
            jmethodID mid = env->GetMethodID(jPLDJSObjectCls_, "value", "()Ljava/lang/String;");
            jobject tmp = env->CallObjectMethod(obj, mid);
            const char *value = env->GetStringUTFChars((jstring) (tmp), nullptr);
            ret = v8::JSON::Parse(ctx, createV8String(env, isolate, string(value)))
                    .ToLocalChecked();
            env->DeleteLocalRef(tmp);
        } else if (env->IsInstanceOf(obj, jPLDJSUndefinedCls_)) {
            LOGI("%s, obj is JSUndefined", logTag);
            ret = Undefined(isolate);
        } else {
            if (exception) *exception = "toJSValue, do not support this type";
            LOGE("toJSValue, do not support this type");
        }
        return ret;
    }

    jobject V8Util::toJavaObj(JNIEnv *env, Isolate *isolate, const Local<Context> &ctx, v8::Handle<v8::Value> arg, string *exception) {
        const char * logTag = "toJavaObj";
        if (arg.IsEmpty()) {
            *exception = "arg is empty";
        } else if (arg->IsBoolean()) {
            jmethodID ctor = env->GetMethodID(jPLDJSBooleanCls_, "<init>", "(Z)V");
            return env->NewObject(jPLDJSBooleanCls_, ctor, arg->BooleanValue(isolate));
        } else if (arg->IsNumber()) {
            jmethodID ctor = env->GetMethodID(jPLDJSNumberCls_, "<init>", "(D)V");
            return env->NewObject(jPLDJSNumberCls_, ctor, arg->NumberValue(ctx).FromMaybe(0));
        } else if (arg->IsNull()) {
            jmethodID ctor = env->GetMethodID(jPLDJSNullCls_, "<init>", "()V");
            return env->NewObject(jPLDJSNullCls_, ctor);
        } else if (arg->IsString()) {
            jmethodID ctor = env->GetMethodID(jPLDJSStringCls_, "<init>", "(Ljava/lang/String;)V");
            String::Value unicodeString(isolate, arg);
            return env->NewObject(jPLDJSStringCls_, ctor, env->NewString(*unicodeString, unicodeString.length()));
        } else if (arg->IsArray() || arg->IsObject()) {
            MaybeLocal<String> str = v8::JSON::Stringify(ctx, arg);
            String::Value unicodeStr(isolate, str.ToLocalChecked());
            jmethodID ctor = env->GetMethodID(jPLDJSObjectCls_, "<init>", "(Ljava/lang/String;)V");
            jobject ret = env->NewObject(jPLDJSObjectCls_, ctor, env->NewString(*unicodeStr, unicodeStr.length()));
            return ret;
        } else if (arg->IsUndefined()) {
            jmethodID  ctor = env->GetMethodID(jPLDJSUndefinedCls_, "<init>", "()V");
            return env->NewObject(jPLDJSUndefinedCls_, ctor);
        } else {
            LOGI("%s, arg is other type, do not support this type", logTag);
            *exception = "arg is other type, do not support this type";
        }
        return nullptr;
    }

    jobjectArray V8Util::getDataField(JNIEnv *env, jobject pldNativeArr) {
        if (env == nullptr || pldNativeArr == nullptr) return nullptr;
        jclass jPLDNativeArray = env->FindClass(kPLDNativeArrayStr);
        jfieldID dataFieldId = env->GetFieldID(jPLDNativeArray, "data", "[Ljava/lang/Object;");
        jobject objects = env->GetObjectField(pldNativeArr, dataFieldId);
        env->DeleteLocalRef(jPLDNativeArray);
        auto args = reinterpret_cast<jobjectArray>(objects);
        return args;
    }


} // namespace paladin

