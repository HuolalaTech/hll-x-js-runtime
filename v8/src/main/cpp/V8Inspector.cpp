//
// Created by nieyinyin on 2023/11/14.
//

#include "include/V8Inspector.h"

namespace paladin {
    V8Inspector::V8Inspector(JNIEnv *env) {
        this->env = env;
        this->v8InspectorCls = (jclass)env->NewGlobalRef((env)->FindClass("com/paladin/jsruntime/v8/inspector/V8Inspector"));
        this->v8InspectorDelegateCls = (jclass)env->NewGlobalRef((env)->FindClass("com/paladin/jsruntime/v8/inspector/V8InspectorDelegate"));
        this->v8InspectorDelegateOnResponseMethodID = env->GetMethodID(this->v8InspectorDelegateCls, "onResponse", "(Ljava/lang/String;)V");
        this->v8InspectorDelegateWaitFrontendMessageMethodID = env->GetMethodID(this->v8InspectorDelegateCls, "waitFrontendMessageOnPause", "()V");
    }
    void V8Inspector::onResponse(const std::string &message) {
        env->CallVoidMethod(delegate,  this->v8InspectorDelegateOnResponseMethodID, env->NewStringUTF(message.c_str()));
    }

    void V8Inspector::waitFrontendMessage() {
        env->CallVoidMethod(delegate, this->v8InspectorDelegateWaitFrontendMessageMethodID);
    }
}


