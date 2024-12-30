//
// Created by nieyinyin on 2021/11/9.
//

#include "qjs_executor.h"
#include "qjs_utils.h"
#include <cmath>
#include <vector>
#include "../../../../common/src/main/cpp/include/log.h"
#include "../../../../common/src/main/cpp/include/utils.h"
#include "../../../../common/src/main/cpp/include/java_function.h"
#include "../../../../common/src/main/cpp/include/constant.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "quickjs/quickjs.h"
#ifdef __cplusplus
}
#endif

using namespace std;

namespace paladin {

    QJSExecutor::QJSExecutor(JNIEnv *env) {

        env_ = env;

        rt_ = JS_NewRuntime();
        Utils::checkNullPointer(env, rt_, kMsgNullJSRuntime);
        ctx_ = JS_NewContext(rt_);
        Utils::checkNullPointer(env, ctx_, kMsgNullJSContext);
        qjsUtil_ = new QJSUtil(env, ctx_);
        Utils::checkNullPointer(env, qjsUtil_, kMsgNullQJSUtil);

        // this is important!!
        uniqueId_ = globalExecutorId++;
        jsExecutorMap_[uniqueId_]= this;

        LOGI("finish to construct QJSExecutor, its id = %d", uniqueId_);
    }

    QJSExecutor::~QJSExecutor() {
        JS_FreeContext(ctx_);
        JS_FreeRuntime(rt_);
        jsExecutorMap_.erase(uniqueId_);
        for (auto &v : registeredFuncMap_) {
            if (v.second != nullptr) {
                env_->DeleteGlobalRef(v.second->javaFuncGlobalRef);
                delete v.second;
            }
        }
        registeredFuncMap_.clear();
        delete qjsUtil_;

        LOGI("finish to destruct QJSExecutor, its id = %d", uniqueId_);
    }

    bool QJSExecutor::evaluateVoidScript(const std::string &script,
                                         const std::string &scriptUrl,
                                         std::string *exception) {
        LOGI("evaluateVoidScript, script = %s", script.c_str());
        JSValueConst res = JS_Eval(ctx_, script.c_str(), script.length(), scriptUrl.empty() ? "" : scriptUrl.c_str(), 0);
        bool hasException = qjsUtil_->checkJSException(res, exception);
        JS_FreeValue(ctx_, res);
        return !hasException;
    }

    bool QJSExecutor::evaluateStringScript(const std::string &script,
                                           const std::string &scriptUrl,
                                                   std::string *result,
                                                   std::string *exception) {

        LOGI("evaluateStringScript, script = %s", script.c_str());
        JSValueConst res = JS_Eval(ctx_, script.c_str(), script.length(), scriptUrl.empty() ? "" : scriptUrl.c_str(), 0);
        bool ret = false;
        bool hasException = qjsUtil_->checkJSException(res, exception);
        if (!hasException) {
            if (JS_IsString(res)){
                ret = true;
                LOGI("evaluateStringScript, res is string");
                if (result) {
                    const char *str = JS_ToCString(ctx_, res);
                    if (str) {
                        *result = string(str);
                        JS_FreeCString(ctx_, str);
                    }

                    JS_FreeValue(ctx_, res);
                }
            } else {
                if (exception) *exception = std::string("evaluateStringScript, result's data type is not string");
            }
        }
        return ret;
    }

    void QJSExecutor::registerFunction(JNIEnv *env, const string &funcName, jobject javaFunction,
                                       std::string *exception) {

        // store java function
        auto *javaFunc = new JavaFunction(funcName, env->NewGlobalRef(javaFunction));
        registeredFuncMap_[funcName] = javaFunc;

        // inject function into js engine's global object
        JSValue func = createJSFunction(uniqueId_, funcName);
        JSValue globalObj = JS_GetGlobalObject(ctx_);

        JS_SetPropertyStr(ctx_, globalObj, funcName.c_str(), JS_DupValue(ctx_, func));

        // free js value
        JS_FreeValue(ctx_, globalObj);
        JS_FreeValue(ctx_, func);
    }



    jobject
    QJSExecutor::callJavaScriptFunction(JNIEnv *env, const string &thisObjStr, const string &funObjStr,
                                        jobject pldNativeArray, std::string *exception) {

        LOGI("callJavaScriptFunction, thisObjStr=%s, funObjStr=%s", thisObjStr.c_str(), funObjStr.c_str());

        // generate js function args
        jobjectArray arr = qjsUtil_->getDataField(pldNativeArray);
        int len = 0;
        if (arr != nullptr) len = env->GetArrayLength(arr);

        LOGI("callJavaScriptFunction, pldNativeArray's length  = %d", len);
        JSValue args[len];
        for (int i = 0; i < len; ++i) {
            jobject item = env->GetObjectArrayElement(arr, i);
            string excep;
            args[i] = qjsUtil_->toJSValue(item, &excep);
            if (!excep.empty()) {
                if (exception) *exception = excep;
                return nullptr;
            }
        }

        // make sure this_obj
        JSValueConst global = JS_GetGlobalObject(ctx_);
        JSValue thisObj;
        if (thisObjStr.empty()) {
            thisObj = global;
        } else {
            thisObj = JS_GetPropertyStr(ctx_, global, thisObjStr.c_str());
        }

        // make sure func_obj
        JSValue funObj = JS_GetPropertyStr(ctx_, thisObj, funObjStr.c_str());

        // call js function
        LOGI("callJavaScriptFunction, JS_Call begin...");
        JSValue result = JS_Call(ctx_, funObj, thisObj, len, args);
        LOGI("callJavaScriptFunction, JS_Call end...");

        // check exception, if exist, dump it.
        bool hasException = qjsUtil_->checkJSException(result, exception);

        // construct return value
        jobject ret = nullptr;
        if (!hasException) ret = qjsUtil_->toJavaObj(result, exception);

        // free js value
        for (int i = 0; i < len; ++i) {
            JS_FreeValue(ctx_, args[i]);
        }
        if (QJSUtil::JSEquals(global, thisObj)) {
            JS_FreeValue(ctx_, global);
        } else {
            JS_FreeValue(ctx_, global);
            JS_FreeValue(ctx_, thisObj);
        }
        JS_FreeValue(ctx_, funObj);
        JS_FreeValue(ctx_, result);
        return ret;
    }

    JSValue
    QJSExecutor::callJavaCallback(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv, int magic,
                     JSValue *func_data) {

        if (func_data == nullptr) {
            LOGE("callJavaCallback, func_data is null");
            return JS_UNDEFINED;
        }
        int currentId = JS_VALUE_GET_INT(func_data[0]);
        LOGI("callJavaCallback, current executor id = %d", currentId);

        QJSExecutor *qjsExecutor = jsExecutorMap_[currentId];
        if (qjsExecutor == nullptr) {
            LOGE("callJavaCallback, qjsExecutor is null");
            return JS_UNDEFINED;
        }
        const char *funcName = JS_ToCString(ctx, func_data[1]);

        if (funcName == nullptr) {
            LOGE("callJavaCallback, funcName is null");
            return JS_UNDEFINED;
        }

        LOGI("callJavaCallback, funcName is %s", funcName);

        JNIEnv *env = qjsExecutor->env_;
        QJSUtil *qjsUtil = qjsExecutor->qjsUtil_;
        auto *javaFunc = qjsExecutor->registeredFuncMap_[funcName];

        jclass jJSValueClazz = env->FindClass(kPLDJSValueClazzStr);
        jclass jJSArrayClazz = env->FindClass(kPLDJSArrayClazzStr);
        jclass jJavaFunctionClazz = env->FindClass(kPLDJavaFunctionStr);

        jobjectArray args = nullptr;
        if (argc > 0) {
            args = env->NewObjectArray(argc, jJSValueClazz, nullptr);
            for (int i = 0; i < argc; ++i) {
                string excep;
                jobject element = qjsUtil->toJavaObj(argv[i], &excep);
                if (!excep.empty()) {
                    LOGE("callJavaCallback, toJavaObj fail, exception is %s", excep.c_str());
                }
                env->SetObjectArrayElement(args, i, element);
                env->DeleteLocalRef(element);
            }
        }

        jmethodID ctorMid = env->GetMethodID(jJSArrayClazz, "<init>",
                                             "([Lcom/paladin/jsruntime/common/jsvalue/JSValue;)V");
        // construct PLDNativeArray
        jobject execArg = env->NewObject(jJSArrayClazz, ctorMid, args);
        jmethodID exec = env->GetMethodID(jJavaFunctionClazz, "exec",
                                          "(Lcom/paladin/jsruntime/common/jsvalue/JSArray;)Lcom/paladin/jsruntime/common/PLDNativeArray;");
        // call JavaFunction's exec
        jobject result = env->CallObjectMethod(javaFunc->javaFuncGlobalRef, exec, execArg);
        jobjectArray resultArr = qjsUtil->getDataField(result);
        unique_ptr<JSValue> jsValues = nullptr;
        jsize count;
        if (resultArr != nullptr) {
            count = env->GetArrayLength(resultArr);
            if (count > 0) {
                jsValues = unique_ptr<JSValue>(new JSValueConst[count]);
                for (int i = 0; i < count; ++i) {
                    string excep;
                    jsValues.get()[i] = qjsUtil->toJSValue(env->GetObjectArrayElement(resultArr, i), &excep);
                    if (!excep.empty()) {
                        LOGE("callJavaCallback, toJSValue fail, exception is %s", excep.c_str());
                    }
                }
            }
        }
        env->DeleteLocalRef(jJSValueClazz);
        env->DeleteLocalRef(jJSArrayClazz);
        env->DeleteLocalRef(jJavaFunctionClazz);
        env->DeleteLocalRef(args);
        env->DeleteLocalRef(execArg);
        env->DeleteLocalRef(result);
        env->DeleteLocalRef(resultArr);

        if (jsValues == nullptr) {
            return JS_UNDEFINED;
        } else {
            if (count > 1) {
                JSValueConst jsValueArr = JS_NewArray(ctx);
                for (int i = 0; i < count; ++i) {
                    JS_SetPropertyUint32(ctx, jsValueArr, i, JS_DupValue(ctx, jsValues.get()[i]));
                }
                return jsValueArr;
            } else {
                return jsValues.get()[0];
            }
        }
    }

    void QJSExecutor::protectJSObj(const string &objName, std::string *exception) {

    }

    void QJSExecutor::unprotectJSObj(const string &obj, std::string *exception) {

    }

    void QJSExecutor::gc() {

    }

    inline JSValue QJSExecutor::createJSFunction(const int targetExecutor, const std::string &funcName) {
        JSValueConst func_data[2];
        func_data[0] = JS_NewInt32(ctx_, targetExecutor);
        func_data[1] = JS_NewString(ctx_, funcName.c_str());
        JSValue func = JS_NewCFunctionData(ctx_, callJavaCallback, 1, 0, 2, func_data);
        return func;
    }

} // namespace paladin

