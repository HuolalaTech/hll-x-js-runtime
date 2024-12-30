//
// Created by nieyinyin on 2021/11/9.
//

#ifndef JSRUNTIME_JSEXECUTOR_H
#define JSRUNTIME_JSEXECUTOR_H

#include <string>
#include <unordered_map>
#include "jni.h"

namespace paladin {


class JSExecutor {

public:
    JNIEnv *env_;

    virtual ~JSExecutor() = default;
    virtual bool evaluateVoidScript(
            const std::string &script,
            const std::string &scriptUrl,
            std::string *runException) = 0;

    virtual bool evaluateStringScript(
            const std::string &script,
            const std::string &scriptUrl,
            std::string *result,
            std::string *runException) = 0;

    virtual jobject callJavaScriptFunction(
            JNIEnv *env,
            const std::string &thisObj,
            const std::string &method,
            jobject pldNativeArray,
            std::string *exception) = 0;

    virtual void registerFunction(JNIEnv *env,
                                  const std::string &funcName,
                                  jobject javaFunction,
                                  std::string *exception) = 0;

    virtual void protectJSObj(const std::string &objName, std::string *exception) = 0;

    virtual void unprotectJSObj(const std::string &obj, std::string *exception) = 0;

    virtual void gc() = 0;

//    virtual void injectJavaScriptObject(
//            const std::string
//            ) = 0;
};



} // namespace paladin
#endif //JSRUNTIME_JSEXECUTOR_H
