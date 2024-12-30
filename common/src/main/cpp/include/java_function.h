//
// Created by nieyinyin on 2022/2/23.
//

#ifndef PALADINCORE_JAVA_FUNCTION_H
#define PALADINCORE_JAVA_FUNCTION_H

#include <string>
#include "log.h"

namespace paladin {

    struct JavaFunction {
        std::string name;
        jobject javaFuncGlobalRef;

        JavaFunction(const std::string &name, const jobject javaFuncGlobalRef)
                                    : name(name), javaFuncGlobalRef(javaFuncGlobalRef) {}

        virtual ~JavaFunction() {
            LOGI("JavaFunction destructor");
            // need JNIEnv to delete javaFuncGlobalRef, so do it in jsc_executor destructor
        }
    };

} // namespace paladin


#endif //PALADINCORE_JAVA_FUNCTION_H
