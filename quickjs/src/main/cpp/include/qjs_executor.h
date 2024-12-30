//
// Created by nieyinyin on 2021/11/10.
//

#ifndef JS_RUNTIME_QJS_EXECUTOR_H
#define JS_RUNTIME_QJS_EXECUTOR_H

#include <string>
#include "../../../../../common/src/main/cpp/include/java_function.h"
#include "../../../../../common/src/main/cpp/include/js_executor.h"
#include <cmath>
#include <jni.h>
#include <unordered_map>
#include "quickjs/quickjs.h"
#include "qjs_utils.h"

namespace paladin {

    class QJSExecutor : public JSExecutor {
    public:

        QJSExecutor(JNIEnv *env);
        ~QJSExecutor();
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

    private:
        JSRuntime *rt_ = nullptr;
        JSContext *ctx_ = nullptr;
        QJSUtil *qjsUtil_ = nullptr;

        int uniqueId_; // current QJSExecutor unique identification

        std::unordered_map<std::string, JavaFunction*> registeredFuncMap_ = {};
        
        inline JSValue createJSFunction(const int targetExecutor, const std::string &funcName);

        static JSValue
        callJavaCallback(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv, int magic,
                         JSValue *func_data);
    };

    // every QJSExecutor object unique identification, used by js method callback
    static int globalExecutorId = 0;
    static std::unordered_map<int, paladin::QJSExecutor*> jsExecutorMap_ = {};

} // namespace paladin

#endif //JS_RUNTIME_QJS_EXECUTOR_H
