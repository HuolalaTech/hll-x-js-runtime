//
// Created by nieyinyin on 2023/10/30.
//

#ifndef JS_RUNTIME_V8_EXECUTOR_H
#define JS_RUNTIME_V8_EXECUTOR_H

#include <string>
#include "../../../../../common/src/main/cpp/include/java_function.h"
#include "../../../../../common/src/main/cpp/include/js_executor.h"
#include <cmath>
#include <jni.h>
#include <unordered_map>
#include "v8/v8.h"
#include "v8_utils.h"
#include "V8Runtime.h"
#include "JSBigString.h"

namespace paladin {

    class V8Executor : public JSExecutor {
    public:
        V8Executor(JNIEnv *env);
        ~V8Executor();
        bool evaluateVoidScript(const std::string &script,
                                const std::string &scriptUrl,
                                std::string *runException) override;

        bool evaluateStringScript(const std::string &script,
                                  const std::string &scriptUrl,
                                  std::string *result, std::string *runException) override;

        jobject callJavaScriptFunction(
                JNIEnv *env,
                const std::string &thisObj,
                const std::string &method,
                jobject pldNativeArray,
                std::string *exception) override;

        void registerFunction(JNIEnv *env,
                              const std::string &funcName,
                              jobject javaFunction,
                              std::string *exception) override;

        void protectJSObj(const std::string &objName, std::string *exception) override;

        void unprotectJSObj(const std::string &obj, std::string *exception) override;

        void gc() override;

        long createInspector(jobject inspectorDelegateObj, std::unique_ptr<v8::Platform> &v8Platform, const string &contextName);

        void dispatchProtocolMessage(const string &protocolMessage);

        void schedulePauseOnNextStatement(const string &reason);

    private:
        V8Runtime *runtime;
        V8Util *v8Util_;

        int uniqueId_; // current V8Executor unique identification

        std::unordered_map<std::string, JavaFunction*> registeredFuncMap_ = {};

        static void callJavaCallback(const FunctionCallbackInfo<Value>& args);

        static std::unique_ptr<JSBigBufferString> loadStartUpData(JNIEnv *env);
    };

    // every V8Executor object unique identification, used by js method callback
    static int globalExecutorId = 0;
    static std::unordered_map<int, paladin::V8Executor*> jsExecutorMap_ = {};

} // namespace paladin

#endif //JS_RUNTIME_V8_EXECUTOR_H
