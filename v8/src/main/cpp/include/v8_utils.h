//
// Created by nieyinyin on 2023/10/30.
//

#ifndef PALADIN_JS_RUNTIME_QJS_UTILS_H
#define PALADIN_JS_RUNTIME_QJS_UTILS_H

#include "jni.h"
#include "v8/v8.h"
#include "V8Runtime.h"

using namespace std;
using namespace v8;
namespace paladin {

    class V8Util {

    private:

        // primitive types
        jclass jBooleanCls_;
        jclass jIntegerCls_;
        jclass jDoubleCls_;
        jclass jStringCls_;

        // js values
        jclass jPLDJSObjectCls_;
        jclass jPLDJSArrayCls_;
        jclass jPLDJSValueCls_;
        jclass jPLDJSNumberCls_;
        jclass jPLDJSNullCls_;
        jclass jPLDJSUndefinedCls_;
        jclass jPLDJSBooleanCls_;
        jclass jPLDJSStringCls_;

        // others
        jclass jPLDJavaFunctionCls_;
        jclass jPLDNativeArrayCls_;

        JNIEnv *env_;

    public:

        V8Util(JNIEnv *env);

        virtual ~V8Util();

        static const char* ToCString(const String::Utf8Value& value);

        static ScriptOrigin* createScriptOrigin(JNIEnv * env, Isolate* isolate, string &scriptName,
                                                int lineNumber);

        static v8::Local<v8::String> createV8String(JNIEnv *env, Isolate *isolate, const string &str);

        static std::string createString(JNIEnv *env, Isolate *isolate, const string &str);

        static bool compileScript(JNIEnv *env, const Local<Context>& context, Isolate *isolate,
                           const string &std_script, string *scriptName, int lineNumber,
                           Local<Script> &script, TryCatch* tryCatch, std::string *exception);

        static bool runScript(const Local<Context>& context, Isolate* isolate,
                       JNIEnv *env, Local<Script> *script, TryCatch* tryCatch, std::string *exception);

        static bool runScript(const Local<Context>& context, Isolate* isolate,
                       JNIEnv *env, Local<Script> *script, TryCatch* tryCatch,
                       Local<Value> &result, std::string *exception);

        Local<v8::Value> toJSValue(JNIEnv *env, Isolate *isolate,
                                   const Local<Context> &ctx, jobject obj, std::string *exception);

        jobject toJavaObj(JNIEnv *env, Isolate *isolate,
                          const Local<Context> &ctx, Handle<v8::Value> arg, string *exception);

        /**
         * get data field for com.common.PLDNativeArray
         * @param pldNativeArr
         * @return
         */
        static jobjectArray getDataField(JNIEnv *env, jobject pldNativeArr);

        static void getException(const Local<Context> &context, Isolate *isolate,
                          TryCatch *tryCatch, string *exception);


    };

} //



#endif //PALADIN_JS_RUNTIME_QJS_UTILS_H
