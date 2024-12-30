//
// Created by nieyinyin on 2023/10/30.
//

#include "v8_executor.h"
#include "v8_utils.h"
#include <cmath>
#include <vector>
#include "cppgc/libplatform/libplatform.h"
#include "log.h"
#include "utils.h"
#include "java_function.h"
#include "constant.h"
#include "v8.h"
#include "MethodDescriptor.h"
#include "InspectorDelegate.h"
#include "V8InspectorClientImpl.h"
#include <android/asset_manager_jni.h>
#include "V8Runtime.h"
#include "V8Inspector.h"

using namespace v8;

#define SETUP(runtimePtr) v8::Isolate* isolate = (runtimePtr)->isolate; \
    v8::Locker locker(isolate);\
    Isolate::Scope isolateScope(isolate);\
    HandleScope handle_scope(isolate);\
    Local<Context> context = Local<Context>::New(isolate,runtime->context_);\
    Context::Scope context_scope(context);

namespace paladin {



    V8Executor::V8Executor(JNIEnv *env) {
        env_ = env;

        // init V8Runtime, default global alias is nullptr
        runtime = new V8Runtime();
        v8::Isolate::CreateParams create_params;

        LOGI("construct V8Executor 1 ");

        // https://github.com/v8/v8/blob/main/samples/shell.cc  need to allocator?
        create_params.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
        unique_ptr<v8::StartupData> startupData = std::make_unique<v8::StartupData>();
        std::unique_ptr<JSBigBufferString> buf = loadStartUpData(env);
        startupData->data = buf->c_str();
        startupData->raw_size = buf->size();
        create_params.snapshot_blob = startupData.get();

        runtime->isolate = v8::Isolate::New(create_params);
        v8::Locker locker(runtime->isolate);
        v8::Isolate::Scope isolate_scope(runtime->isolate);
        runtime->pendingException = nullptr;

        v8::HandleScope handle_scope(runtime->isolate);
        v8::Handle<v8::ObjectTemplate> globalObject = v8::ObjectTemplate::New(runtime->isolate);
        v8::Handle<v8::Context> context = v8::Context::New(runtime->isolate, nullptr, globalObject);
        runtime->context_.Reset(runtime->isolate, context);
        runtime->globalObject = new v8::Persistent<v8::Object>;
        runtime->globalObject->Reset(runtime->isolate, context->Global()->GetPrototype()->ToObject(context).ToLocalChecked());

        // create V8Util
        v8Util_ = new V8Util(env);

        // this is important!!
        uniqueId_ = globalExecutorId++;
        jsExecutorMap_[uniqueId_]= this;

        LOGI("finish to construct V8Executor, its id = %d", uniqueId_);
    }

    V8Executor::~V8Executor() {
        // erase this executor from global map
        jsExecutorMap_.erase(uniqueId_);

        // release runtime
        {
            if (runtime != nullptr) {
                // release global object
                runtime->globalObject->Reset();
                delete(runtime->globalObject);

                // release isolate
                runtime->context_.Reset();
                runtime->isolate->Dispose();
                delete(runtime);
            }
        }

        // delete all function object
        for (auto &v : registeredFuncMap_) {
            if (v.second != nullptr) {
                env_->DeleteGlobalRef(v.second->javaFuncGlobalRef);
                delete v.second;
            }
        }
        registeredFuncMap_.clear();

        // delete v8 util
        delete v8Util_;

        LOGI("finish to destruct V8Executor, its id = %d", uniqueId_);
    }

    bool V8Executor::evaluateVoidScript(const std::string &script0,
                                        const std::string &script0Url,
                                        std::string *exception) {
        LOGI("evaluateVoidScript, script = %s, scriptUrl = %s", script0.c_str(), script0Url.c_str());
        SETUP(runtime)
        v8::TryCatch tryCatch(isolate);
        v8::Local<v8::Script> script;
        std::string scriptName = script0Url;
        if (!paladin::V8Util::compileScript(env_, context, isolate, script0,
                                    &scriptName, 0, script, &tryCatch, exception))
            return false;
        return paladin::V8Util::runScript(context, isolate, env_, &script, &tryCatch, exception);
    }

    bool V8Executor::evaluateStringScript(const std::string &script0,
                                          const std::string &script0Url,
                                                   std::string *result0,
                                                   std::string *exception) {
        LOGI("evaluateVoidScript, script = %s, scriptUrl = %s", script0.c_str(), script0Url.c_str());
        SETUP(runtime)
        v8::TryCatch tryCatch(isolate);
        v8::Local<v8::Script> script;
        v8::Local<v8::Value> result;
        std::string scriptName = script0Url;
        if (!paladin::V8Util::compileScript(env_, context, isolate, script0,
                                    &scriptName, 0, script, &tryCatch, exception))
            return false;
        bool ret = paladin::V8Util::runScript(context, isolate, env_, &script, &tryCatch, result, exception);
        if (ret) {
            v8::String::Utf8Value unicodeString(isolate, result);
            *result0 = std::string(paladin::V8Util::ToCString(unicodeString));
        }
        return ret;
    }

    void V8Executor::registerFunction(JNIEnv *env, const string &funcName, jobject javaFunction,
                                       std::string *exception) {
        LOGI("registerFunction, funcName = %s", funcName.c_str());
        // store java func
        auto *javaFunc = new JavaFunction(funcName, env->NewGlobalRef(javaFunction));
        registeredFuncMap_[funcName] = javaFunc;

        auto *md = new MethodDescriptor();
        md->funcName = funcName;
        md->targetExecutor = uniqueId_;

        SETUP(runtime)
        Local<External> ext = External::New(runtime->isolate, md);

        // register func into js engine
        v8::MaybeLocal<v8::String> func_str =
                v8::String::NewFromUtf8(runtime->isolate,
                                        funcName.c_str(),
                                        NewStringType::kNormal,
                                        static_cast<int>(funcName.length()));

        FunctionCallback callback = callJavaCallback;
        MaybeLocal<Function> func = Function::New(context, callback, ext);
        Maybe<bool> unusedResult = context->Global()->Set(context, func_str.ToLocalChecked(), func.ToLocalChecked());
        unusedResult.Check();
    }

    void V8Executor::callJavaCallback(const FunctionCallbackInfo<Value> &args) {

        Local<External> data = Local<External>::Cast(args.Data());
        void *methodDescriptorPtr = data->Value();
        auto* md = static_cast<MethodDescriptor*>(methodDescriptorPtr);

        V8Executor *v8Executor = jsExecutorMap_[md->targetExecutor];
        JNIEnv *env = v8Executor->env_;
        V8Util *v8Util = v8Executor->v8Util_;
        Isolate *isolate = v8Executor->runtime->isolate;

        Isolate::Scope isolateScope(isolate);
        Locker locker(isolate);
        HandleScope handle_scope(isolate);
        Local<Context> ctx = Local<Context>::New(isolate, v8Executor->runtime->context_);
        Context::Scope context_scope(ctx);

        auto *javaFunc = v8Executor->registeredFuncMap_[md->funcName];

        jclass jJSValueClazz = env->FindClass(kPLDJSValueClazzStr);
        jclass jJSArrayClazz = env->FindClass(kPLDJSArrayClazzStr);
        jclass jJavaFunctionClazz = env->FindClass(kPLDJavaFunctionStr);

        jobjectArray arr = nullptr;
        if (args.Length() > 0) {
            arr = env->NewObjectArray(args.Length(), jJSValueClazz, nullptr);
            for (int i = 0; i < args.Length(); ++i) {
                string excep;
                jobject element = v8Util->toJavaObj(env, isolate, ctx, args[i], &excep);
                if (!excep.empty()) {
                    LOGE("callJavaCallback, toJavaObj fail, exception is %s", excep.c_str());
                }
                env->SetObjectArrayElement(arr, i, element);
                env->DeleteLocalRef(element);
            }
        }

        jmethodID ctorMid = env->GetMethodID(jJSArrayClazz, "<init>",
                                             "([Lcom/paladin/jsruntime/common/jsvalue/JSValue;)V");
        // construct PLDNativeArray
        jobject execArg = env->NewObject(jJSArrayClazz, ctorMid, arr);
        jmethodID exec = env->GetMethodID(jJavaFunctionClazz, "exec",
                                          "(Lcom/paladin/jsruntime/common/jsvalue/JSArray;)Lcom/paladin/jsruntime/common/PLDNativeArray;");
        // call JavaFunction's exec
        jobject result = env->CallObjectMethod(javaFunc->javaFuncGlobalRef, exec, execArg);
        jobjectArray resultArr = paladin::V8Util::getDataField(env, result);
        Local<Object> jsValues;
        jsize count;
        if (resultArr != nullptr) {
            count = env->GetArrayLength(resultArr);
            if (count > 0) {
                jsValues = Array::New(isolate, count);
                for (int i = 0; i < count; ++i) {
                    string excep;
                    jsValues->Set(ctx, i, v8Util->toJSValue(env, isolate, ctx, env->GetObjectArrayElement(resultArr, i), &excep));
                    if (!excep.empty()) {
                        LOGE("callJavaCallback, toJSValue fail, exception is %s", excep.c_str());
                    }
                }
            }
        }

        // delete local ref
        env->DeleteLocalRef(jJSValueClazz);
        env->DeleteLocalRef(jJSArrayClazz);
        env->DeleteLocalRef(jJavaFunctionClazz);
        env->DeleteLocalRef(arr);
        env->DeleteLocalRef(execArg);
        env->DeleteLocalRef(result);
        env->DeleteLocalRef(resultArr);

        // set return value
        if (jsValues.IsEmpty()) {
            args.GetReturnValue().SetUndefined();
        } else {
            if (count > 1) {
                args.GetReturnValue().Set(jsValues);
            } else {
                args.GetReturnValue().Set(jsValues->Get(ctx, 0).ToLocalChecked());
            }
        }
    }


    jobject
    V8Executor::callJavaScriptFunction(JNIEnv *env, const string &thisObjStr, const string &funObjStr,
                                        jobject pldNativeArray, std::string *exception) {

        LOGI("callJavaScriptFunction, thisObjStr=%s, funObjStr=%s", thisObjStr.c_str(), funObjStr.c_str());
        SETUP(runtime)
        TryCatch tryCatch(isolate);

        // 1. generate js function args
        jobjectArray arr = paladin::V8Util::getDataField(env, pldNativeArray);
        int len = 0;
        if (arr != nullptr) len = env->GetArrayLength(arr);

        LOGI("callJavaScriptFunction, pldNativeArray's length  = %d", len);
        Local<v8::Value> args[len];
        for (int i = 0; i < len; ++i) {
            jobject item = env->GetObjectArrayElement(arr, i);
            string excep;
            args[i] = v8Util_->toJSValue(env, runtime->isolate, context, item, &excep);
            if (!excep.empty()) {
                if (exception) *exception = excep;
                return nullptr;
            }
        }

        // 2. make sure this obj
        Local<Object> thisObj;
        if (thisObjStr.empty()) {
            thisObj = context->Global();
        } else {
            Local<String> key = paladin::V8Util::createV8String(env, isolate, thisObjStr);
            if (context->Global()->Has(context, key).FromJust()) {
                MaybeLocal<Value> maybeLocalThisObj = context->Global()->Get(context, key);
                if (!maybeLocalThisObj.IsEmpty() && maybeLocalThisObj.ToLocalChecked()->IsObject()) {
                    thisObj = Local<Object>::Cast(maybeLocalThisObj.ToLocalChecked());
                }
            }
        }

        // 3. make sure func_obj
        Handle<Function> func;
        Local<String> v8FunObjStr = paladin::V8Util::createV8String(env, isolate, funObjStr);
        if (!thisObj.IsEmpty() && thisObj->Has(context, v8FunObjStr).ToChecked()) {
            MaybeLocal<Value> maybeLocalFunc = thisObj->Get(context, v8FunObjStr).ToLocalChecked();
            if (!maybeLocalFunc.IsEmpty() && maybeLocalFunc.ToLocalChecked()->IsFunction()) {
                func = Handle<Function>::Cast(maybeLocalFunc.ToLocalChecked());
            }
        }

        // 4. call js function
        bool hasException = false;
        LOGI("callJavaScriptFunction, v8 func->Call begin...");
        MaybeLocal<Value> function_call_result;
        if (thisObj.IsEmpty()) {
            hasException = true;
            (*exception).append("ReferenceError: ").append(thisObjStr).append(" is not defined \n");
        } else if (func.IsEmpty()) {
            hasException = true;
            // TODO append效率如何？ 还有哪些string format的方式？
            (*exception).append("TypeError: ").append(funObjStr).append(" is not a function\n");
        } else {
            function_call_result = func->Call(context, thisObj, len, args);
        }
        LOGI("callJavaScriptFunction, v8 func->Call  end...");

        // 5. check exception
        if (tryCatch.HasCaught()) {
            hasException = true;
            paladin::V8Util::getException(context, isolate, &tryCatch, exception);
        }

        // 6. construct return value
        jobject ret = nullptr;
        if (!hasException && !function_call_result.IsEmpty()) {
            Handle<Value> result = function_call_result.ToLocalChecked();
            ret = v8Util_->toJavaObj(env, isolate, context, result, exception);
        }

        return ret;
    }

    void V8Executor::protectJSObj(const string &objName, std::string *exception) {

    }

    void V8Executor::unprotectJSObj(const string &obj, std::string *exception) {

    }

    void V8Executor::gc() {

    }

    std::unique_ptr<JSBigBufferString> V8Executor::loadStartUpData(JNIEnv *env) {
        // load snapshot_blob.bin
        jclass c = env->FindClass("com/paladin/jsruntime/v8/V8Executor");
        jmethodID id = env->GetStaticMethodID(c, "loadDefaultSnapshotBlobPath", "()Ljava/lang/String;");
        jfieldID amId = env->GetStaticFieldID(c, "am", "Landroid/content/res/AssetManager;");
        jobject am = env->GetStaticObjectField(c, amId);
        jstring assetName = static_cast<jstring>(env->CallStaticObjectMethod(c, id));

        const char * fileName = env->GetStringUTFChars(assetName, nullptr);
        auto asset = AAssetManager_open(
                AAssetManager_fromJava(env, am),
                fileName,
                AASSET_MODE_STREAMING);
        unique_ptr<JSBigBufferString> buf = nullptr;
        if (asset) {
            buf = std::make_unique<JSBigBufferString>(AAsset_getLength(asset));
            size_t offset = 0;
            int readbytes;
            while ((readbytes = AAsset_read(
                    asset, buf->data() + offset, buf->size() - offset)) > 0) {
                offset += readbytes;
            }
            AAsset_close(asset);
        }
        env->ReleaseStringUTFChars(assetName, fileName);
        env->DeleteLocalRef(c);
        env->DeleteLocalRef(am);
        env->DeleteLocalRef(assetName);
        LOGI("load v8 start up data finished!!");
        return std::move(buf); // TODO why need to std::move?
    }

    long V8Executor::createInspector(jobject inspectorDelegateObj, std::unique_ptr<v8::Platform> &v8Platform, const string &contextName) {
        SETUP(runtime)
        runtime->inspector = new V8Inspector(env_);
        runtime->inspector->delegate = env_->NewGlobalRef(inspectorDelegateObj);

        auto* delegate = new InspectorDelegate(
                [ObjectPtr = runtime->inspector](auto && PH1) { ObjectPtr->onResponse(std::forward<decltype(PH1)>(PH1)); },
                [ObjectPtr = runtime->inspector] { ObjectPtr->waitFrontendMessage(); }
        );

        std::string contextName_ = v8Util_->createString(env_, runtime->isolate, contextName);
        if (v8Platform == nullptr) LOGE("v8 platform is nullptr");
        runtime->inspector->client = new V8InspectorClientImpl(runtime->isolate, v8Platform, delegate, contextName_);
        return reinterpret_cast<long>(runtime->inspector);
    }

    void V8Executor::dispatchProtocolMessage(const string &protocolMessage) {
        SETUP(this->runtime)
        this->runtime->inspector->dispatchProtocolMessage(protocolMessage);
    }

    void V8Executor::schedulePauseOnNextStatement(const string &reason) {
        SETUP(this->runtime)
        this->runtime->inspector->schedulePauseOnNextStatement(reason);
    }


} // namespace paladin

