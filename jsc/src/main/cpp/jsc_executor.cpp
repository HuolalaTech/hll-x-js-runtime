//
// Created by nieyinyin on 2021/11/9.
//

#include "../../../../common/src/main/cpp/include/js_executor.h"
#include "jsc_executor.h"
#include "jsc_utils.h"
#include "JSStringRef.h"
#include "JSContextRef.h"
#include "../../../../common/src/main/cpp/include/log.h"
#include <cmath>
#include <vector>
#include "../../../../common/src/main/cpp/include/utils.h"
#include "../../../../common/src/main/cpp/include/java_function.h"
#include "../../../../common/src/main/cpp/include/constant.h"

using namespace std;

namespace paladin {

    JSCExecutor::JSCExecutor(JNIEnv *env) {
        env_ = env;
        contextGroup_ = JSContextGroupCreate();
        context_ = JSGlobalContextCreateInGroup(contextGroup_, nullptr);

        // set name for global context
        std::string prefix = "global_context_";
        prefix.append(std::to_string(contextId++));
        context_name_ = prefix;
        JSStringRef tmp = JSStringCreateWithUTF8CString(prefix.c_str());
        JSGlobalContextSetName(context_, tmp);
        JSStringRelease(tmp);

        // store this JSCExecutor into map.
        jsExecutorMap_[prefix] = this;

        LOGI("JSCExecutor, created success, current js global context's name: %s", prefix.c_str());
    }

    JSCExecutor::~JSCExecutor() {
        LOGI("JSCExecutor destructor, current context name is : %s", context_name_.c_str());
        jsExecutorMap_.erase(context_name_);
        JSGlobalContextRelease(context_);
        JSContextGroupRelease(contextGroup_);
        for (auto &v : registeredFuncMap_) {
            LOGI("JSCExecutor destructor: free registeredFuncMap_");
            if (v.second != nullptr) {
                env_->DeleteGlobalRef(v.second->javaFuncGlobalRef);
                delete v.second;
            }
        }
        registeredFuncMap_.clear();
//        delete funcMap_; // need it?
//        delete env_; // need it ?
    }

    bool JSCExecutor::evaluateVoidScript(const std::string &script,
                                         const std::string &scriptUrl,
                                         std::string *exception) {
        JSValueRef exceptionRef = nullptr;
        JSStringRef source = JSStringCreateWithUTF8CString(script.c_str());
        JSStringRef sourceUrl = JSStringCreateWithUTF8CString(scriptUrl.empty() ? "" : scriptUrl.c_str());
        LOGI("evaluateVoidScript, start");
        JSEvaluateScript(context_, source, nullptr, sourceUrl, 0, &exceptionRef);
        LOGI("evaluateVoidScript, end");
        JSStringRelease(source);
        JSStringRelease(sourceUrl);
        if (exceptionRef) {
            if (exception) JSCUtils::JSValueToStdString(context_, exceptionRef, exception);
            return false;
        }
        return true;
    }

    bool JSCExecutor::evaluateStringScript(const std::string &script,
                                           const std::string &scriptUrl,
                                                   std::string *result,
                                                   std::string *exception) {
        JSValueRef exceptionRef = nullptr;
        JSStringRef source = JSStringCreateWithUTF8CString(script.c_str());
        JSStringRef sourceUrl = JSStringCreateWithUTF8CString(scriptUrl.empty() ? "" : scriptUrl.c_str());
        LOGI("evaluateStringScript, start");
        JSValueRef retValue = JSEvaluateScript(context_, source, nullptr, sourceUrl, 0, &exceptionRef);
        LOGI("evaluateStringScript, end");
        if (result) {
            paladin::JSCUtils::JSValueToStdString(context_, retValue, result);
        }
        JSStringRelease(source);
        JSStringRelease(sourceUrl);
        if (exceptionRef) {
            if (exception) JSCUtils::JSValueToStdString(context_, exceptionRef, exception);
            return false;
        }
        return true;
    }


    jobject JSCExecutor::callJavaScriptFunction(JNIEnv *env,
                                             const std::string &thisObj,
                                             const std::string &func,
                                             jobject pldNativeArray,
                                             string *exception) {
        LOGI("callJavaScriptFunction, %s, %s", thisObj.c_str(), func.c_str());

        // generate params
        LOGI("callJavaScriptFunction, generate params");

        JSValueRef *jsValues = nullptr;
        jsize argsCount = 0;
        auto args = JSCUtils::getJavaObjArrFromPLDNativeArray(env, pldNativeArray);
        if (args != nullptr) argsCount = env->GetArrayLength(args);
        if (argsCount > 0) {
            jsValues = new JSValueRef[argsCount];
            JSCUtils::javaObjArrayToJSValueArray(env, context_, args, jsValues);
        }
        env->DeleteLocalRef(args);

        JSObjectRef globalObjRef = JSContextGetGlobalObject(context_);

        JSObjectRef targetObjectRef;

        // get thisObj ref
        LOGI("callJavaScriptFunction, get thisObj ref");

        if (thisObj.empty()) {
            targetObjectRef = globalObjRef;
        } else {
            JSValueRef targetValue = JSCUtils::getPropertyValueFromObject(context_, thisObj, globalObjRef);
            if (targetValue == nullptr) {
                if (exception) *exception = "callJavaScriptFunction, cannot find the this object";
                return nullptr;
            } else {
                targetObjectRef = JSCUtils::toObjectFromValue(context_, targetValue);
            }
        }

        if (targetObjectRef == nullptr) {
            if (exception) *exception = "callJavaScriptFunction: cannot find the targetObjectRef";
            return nullptr;
        }

        // get js func
        LOGI("callJavaScriptFunction, get js func");

        JSValueRef funcValueRef = JSCUtils::getPropertyValueFromObject(context_, func, targetObjectRef);
        if (funcValueRef == nullptr) {
            if (exception) {
                *exception = "callJavaScriptFunction: can not find the js function ";
                exception->append(thisObj).append("$").append(func);
            }
            return nullptr;
        }
        JSObjectRef funcObjectRef = JSCUtils::toObjectFromValue(context_, funcValueRef);
        if (!funcObjectRef || !JSObjectIsFunction(context_, funcObjectRef)) {
            if (exception) {
                *exception = "callJavaScriptFunction: find the js function error.";
                exception->append(thisObj).append("$").append(func);
            }
            return nullptr;
        }

        LOGI("callJavaScriptFunction, JSObjectCallAsFunction start");

        // call the func by target obj
        JSValueRef exceptionRef = nullptr;
        JSValueRef jsValueResult = JSObjectCallAsFunction(context_, funcObjectRef, targetObjectRef, argsCount, jsValues, &exceptionRef);

        // we have protected them in jsc_utils, so we must unprotect them.
        if (jsValues != nullptr) {
            for (int i = 0; i < argsCount; ++i) {
                JSValueUnprotect(context_, jsValues[i]);
            }
        }
        if (exceptionRef) {
            LOGE("callJavaScriptFunction, JSObjectCallAsFunction fail");
            if (exception) {
                JSCUtils::JSValueToStdString(context_, exceptionRef, exception);
            }

            // release
            delete [] jsValues;
            return nullptr;
        } else {
            LOGI("callJavaScriptFunction, JSObjectCallAsFunction success");
            jobject result = JSCUtils::jsValueToJavaObject(env, context_, jsValueResult);
            // release
            delete [] jsValues;
            return result;
        }

    }

    JSValueRef JSCExecutor::JSObjectCallAsFunctionCallback(JSContextRef ctx,
                                                     JSObjectRef function,
                                                     JSObjectRef thisObject,
                                                     size_t argumentCount,
                                                     const JSValueRef arguments[],
                                                     JSValueRef* exception) {

        LOGI("JSObjectCallAsFunctionCallback, start");

        JSStringRef contextName = JSGlobalContextCopyName((JSGlobalContextRef)ctx);
        std::string contextName_;
        JSCUtils::JSStringToStdString(ctx, contextName, &contextName_);
        JSStringRelease(contextName);


        JSCExecutor* jsExecutor = jsExecutorMap_[contextName_];
        if (jsExecutor == nullptr) {
            LOGE("JSObjectCallAsFunctionCallback, can not find JSCExecutor");
            return JSValueMakeUndefined(ctx);
        }
        JNIEnv *env = jsExecutor->env_;

        jclass jJSValueClazz = env->FindClass(kPLDJSValueClazzStr);
        jclass jJSArrayClazz = env->FindClass(kPLDJSArrayClazzStr);
        jclass jJavaFunctionClazz = env->FindClass(kPLDJavaFunctionStr);

        // construct exec's args.
        jobjectArray val = env->NewObjectArray(argumentCount, jJSValueClazz, nullptr);
        for (int i = 0; i < argumentCount; ++i) {
            jobject jobj = JSCUtils::jsValueToJavaObject(env, ctx, arguments[i]);
            env->SetObjectArrayElement(val, i, jobj);
            env->DeleteLocalRef(jobj);
        }
        jmethodID ctorMid = env->GetMethodID(jJSArrayClazz, "<init>",
                                                "([Lcom/paladin/jsruntime/common/jsvalue/JSValue;)V");
        jobject execArg = env->NewObject(jJSArrayClazz, ctorMid, val);

        auto* javaFunc = jsExecutor->registeredFuncMap_[function];

        LOGI("JSObjectCallAsFunctionCallback, JSContextRef's Name: %s, called function's name: %s", contextName_.c_str(), javaFunc->name.c_str());

        jmethodID exec = env->GetMethodID(jJavaFunctionClazz, "exec",
                                          "(Lcom/paladin/jsruntime/common/jsvalue/JSArray;)Lcom/paladin/jsruntime/common/PLDNativeArray;");
        // call JavaFunction's exec
        jobject result = env->CallObjectMethod(javaFunc->javaFuncGlobalRef, exec, execArg);
        jobjectArray resultArr = JSCUtils::getJavaObjArrFromPLDNativeArray(env, result);
        JSValueRef *jsValues = nullptr;
        jsize count;
        if (resultArr != nullptr) {
            count = env->GetArrayLength(resultArr);
            if (count > 0) {
                jsValues = new JSValueRef[count];
                JSCUtils::javaObjArrayToJSValueArray(env, ctx, resultArr, jsValues);
            }
        }

        env->DeleteLocalRef(jJSValueClazz);
        env->DeleteLocalRef(jJSArrayClazz);
        env->DeleteLocalRef(jJavaFunctionClazz);
        env->DeleteLocalRef(val);
        env->DeleteLocalRef(execArg);
        env->DeleteLocalRef(result);
        env->DeleteLocalRef(resultArr);

         if (jsValues == nullptr) {
             return JSValueMakeUndefined(ctx);
         } else {
             if (count > 1) {
                 return JSObjectMakeArray(ctx, argumentCount, jsValues, nullptr);
             } else {
                 return jsValues[0];
             }
         }
    }

    void JSCExecutor::protectJSObj(const string &objName, std::string *exception) {
        if (objName.empty()) {
            if (exception) *exception = "protectJSObj: objName is empty";
            return;
        }
        JSObjectRef  globalObj = JSContextGetGlobalObject(context_);
        JSValueRef targetValue = JSCUtils::getPropertyValueFromObject(context_, objName, globalObj);
        if (targetValue == nullptr) {
            if (exception) *exception = "protectJSObj: can not find object";
            return;
        }
        JSValueProtect(context_, targetValue);
    }

    void JSCExecutor::unprotectJSObj(const string &objName, std::string *exception) {
        if (objName.empty()) {
            if (exception) *exception = "unprotectJSObj: objName is empty";
            return;
        }
        JSObjectRef globalObj = JSContextGetGlobalObject(context_);
        JSValueRef target = JSCUtils::getPropertyValueFromObject(context_, objName, globalObj);
        if (target == nullptr) {
            if (exception) *exception = "unprotectJSObj: can not find object";
            return;
        }
        JSValueUnprotect(context_, target);
    }

    void JSCExecutor::gc() {
        JSGarbageCollect(context_);
    }

    void JSCExecutor::registerFunction(JNIEnv *env, const std::string &funcName, jobject javaFuncRef, std::string *exception) {
        JSObjectRef globalObjRef = JSContextGetGlobalObject(context_);
        innerRegisterFuc(globalObjRef, funcName, env->NewGlobalRef(javaFuncRef), exception);
    }

    void JSCExecutor::innerRegisterFuc(JSObjectRef globalObjRef, const string &funcName, jobject javaFuncGlobalFunc, std::string *exception) {
        if (globalObjRef == nullptr) {
            if (exception) *exception = "globalObjRef must not be nullptr";
            return;
        }
        auto* javaFunction = new JavaFunction(funcName, javaFuncGlobalFunc);
        JSStringRef funcNameRef = JSStringCreateWithUTF8CString(funcName.c_str());
        JSObjectRef funcObjRef = JSObjectMakeFunctionWithCallback(context_, funcNameRef, &JSObjectCallAsFunctionCallback);
        JSValueRef exceptionRef = nullptr;
        JSObjectSetProperty(context_, globalObjRef, funcNameRef, funcObjRef, kJSPropertyAttributeNone, &exceptionRef);
        if (exceptionRef && exception) {
            JSCUtils::JSValueToStdString(context_, exceptionRef, exception);
        }
        registeredFuncMap_[funcObjRef] = javaFunction;
    }


} // namespace paladin

