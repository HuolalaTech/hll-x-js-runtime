//
// Created by nieyinyin on 2022/2/23.
//

#include "include/utils.h"

namespace paladin {
    const char* Utils::getJavaObjName(JNIEnv *env, jobject obj) {
        jclass cls = env->GetObjectClass(obj);

        // First get the class object
        jmethodID mid = env->GetMethodID(cls, "getClass", "()Ljava/lang/Class;");
        jobject clsObj = env->CallObjectMethod(obj, mid);

        // Now get the class object's class descriptor
        cls = env->GetObjectClass(clsObj);

        // Find the getName() method on the class object
        mid = env->GetMethodID(cls, "getName", "()Ljava/lang/String;");

        // Call the getName() to get a jstring object back
        auto strObj = (jstring)env->CallObjectMethod(clsObj, mid);

        // Now get the c string from the java jstring object
        const char* str = env->GetStringUTFChars(strObj, NULL);
        env->ReleaseStringUTFChars(strObj, str);

        return str;
    }

    jint Utils::throwJSRuntimeException(JNIEnv *env, const char *message) {
        jclass exClass;
        const char *className = "com/paladin/jsruntime/common/JSRuntimeException";

        exClass = env->FindClass(className);
        if (exClass == nullptr) {
            LOGE("Utils::throwJSRuntimeException, cannot find the JSRuntimeException class.");
            return -1;
        }
        return env->ThrowNew(exClass, message);
    }

    jint Utils::throwJSDataException(JNIEnv *env, const char *message) {
        jclass exClass;
        const char *className = "com/paladin/jsruntime/common/JSDataException";

        exClass = env->FindClass(className);
        if (exClass == nullptr) {
            LOGE("Utils::throwJSRuntimeException, cannot find the JSDataException class.");
            return -1;
        }
        return env->ThrowNew(exClass, message);
    }

    void Utils::checkNullPointer(JNIEnv *env, void *p, const char *message) {
        if (p == nullptr) {
            throwJSRuntimeException(env, message);
        }
    }
} // namespace paladin


