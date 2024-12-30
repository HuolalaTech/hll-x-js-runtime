//
// Created by nieyinyin on 2022/3/23.
//

#ifndef PALADIN_JS_RUNTIME_QJS_UTILS_H
#define PALADIN_JS_RUNTIME_QJS_UTILS_H

#include "jni.h"
#include "quickjs/quickjs.h"

using namespace std;
namespace paladin {

    class QJSUtil {

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


        JSContext *ctx_;
        JNIEnv *env_;

    public:
        QJSUtil(JNIEnv *env, JSContext *ctx);

        virtual ~QJSUtil();

        /**
         * covert js value to java object
         * @param jsValue
         * @param exception
         * @return
         */
        jobject toJavaObj(JSValueConst jsValue, string *exception);

        /**
         * convert java obj to js value
         * @param obj  java obj at package com.common.jsvalue
         * @param exception
         * @return
         */
        JSValueConst toJSValue(jobject obj, string *exception);

        /**
         * get data field for com.common.PLDNativeArray
         * @param pldNativeArr
         * @return
         */
        jobjectArray getDataField(jobject pldNativeArr);

        /**
         * compare v1 to v2, if equals, return true, or return false
         * @param v1
         * @param v2
         * @return
         */
        static bool JSEquals(JSValue v1, JSValue v2);

        /**
         * check whether arg is exception or not. if yes, dump it to string exception
         * @param arg
         * @param exception
         * @return
         */
        bool checkJSException(JSValueConst arg, string *exception);
    };

} //



#endif //PALADIN_JS_RUNTIME_QJS_UTILS_H
