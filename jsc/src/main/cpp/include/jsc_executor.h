//
// Created by nieyinyin on 2021/11/10.
//

#ifndef JSRUNTIME_JSC_EXECUTOR_H
#define JSRUNTIME_JSC_EXECUTOR_H

#include <string>
#include "JSBase.h"
#include "../../../../../common/src/main/cpp/include/java_function.h"
#include "../../../../../common/src/main/cpp/include/js_executor.h"
#include <cmath>
#include <jni.h>
#include <unordered_map>


namespace paladin {

    class JSCExecutor : public JSExecutor {
    public:
        JSCExecutor(JNIEnv *env);
        ~JSCExecutor();
        bool evaluateVoidScript(const std::string &script,
                                const std::string &scriptUrl,
                                std::string *runException) override;
        bool evaluateStringScript(const std::string &script,
                                  const std::string &scriptUrl,
                                  std::string *result, std::string *runException) override;
        jobject callJavaScriptFunction(JNIEnv *env,
                const std::string &thisObj,
                const std::string &method,
                jobject pldNativeArray,
                std::string *exception) override;

        void protectJSObj(const std::string &objName, std::string *exception) override;

        void unprotectJSObj(const std::string &objName, std::string *exception) override;

        void gc() override;

        void registerFunction(JNIEnv *env,
                              const std::string &funcName,
                              jobject javaFunction,
                              std::string *exception) override;

        static JSValueRef JSObjectCallAsFunctionCallback(JSContextRef ctx,
                                                         JSObjectRef function,
                                                         JSObjectRef thisObject,
                                                         size_t argumentCount,
                                                         const JSValueRef arguments[],
                                                         JSValueRef* exception);
    private:
        std::unordered_map<JSObjectRef, JavaFunction*> registeredFuncMap_ = {};

        JSContextGroupRef contextGroup_;
        JSGlobalContextRef context_;
        std::string context_name_;

        void innerRegisterFuc(JSObjectRef globalObjRef,
                              const std::string &funcName,
                              jobject javaFuncGlobalFunc,
                              std::string *exception);
    };

    static int contextId = 0;
    static std::unordered_map<std::string, paladin::JSCExecutor*> jsExecutorMap_ = {};


} // namespace paladin

#endif //JSRUNTIME_JSC_EXECUTOR_H
