//
// Created by nieyinyin on 2023/11/1.
//

#ifndef PALADIN_ANDROID_RUNTIME_V8RUNTIME_H
#define PALADIN_ANDROID_RUNTIME_V8RUNTIME_H


#include "V8Inspector.h"

namespace paladin {
    class V8Runtime {
    public:
        v8::Isolate* isolate;
        v8::Persistent<v8::Context> context_;
        v8::Persistent<v8::Object>* globalObject;
        jthrowable pendingException;
        V8Inspector* inspector;
    };
}



#endif //PALADIN_ANDROID_RUNTIME_V8RUNTIME_H
