//
// Created by nieyinyin on 2022/2/23.
//

#ifndef PALADINCORE_UTILS_H
#define PALADINCORE_UTILS_H

#include <jni.h>
#include "log.h"

namespace paladin {
    class Utils {

    public:
        static const char* getJavaObjName(JNIEnv *env, jobject obj);

        static jint throwJSRuntimeException(JNIEnv *env, const char* message);

        static jint throwJSDataException(JNIEnv *env, const char* message);

        static void checkNullPointer(JNIEnv *env, void *p, const char *message);
    };

} //namespace paladin




#endif //PALADINCORE_UTILS_H
