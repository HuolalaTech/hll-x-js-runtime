//
// Created by nieyinyin on 2023/11/14.
//

#ifndef PALADIN_ANDROID_RUNTIME_V8INSPECTOR_H
#define PALADIN_ANDROID_RUNTIME_V8INSPECTOR_H


#include "V8InspectorClientImpl.h"
#include "jni.h"

namespace paladin {
    class V8Inspector {
    public:
        V8Inspector(JNIEnv *pEnv);

        jobject delegate = nullptr;
        V8InspectorClientImpl* client = nullptr;

        void dispatchProtocolMessage(const std::string &message) {
            if (client == nullptr) {
                return;
            }
            v8_inspector::StringView protocolMessage = convertSTDStringToStringView(message);
            client->dispatchProtocolMessage(protocolMessage);
        }

        void schedulePauseOnNextStatement(const std::string reason) {
            if (client == nullptr) {
                return;
            }
            auto reason_ = convertSTDStringToStringView(reason);
            client->schedulePauseOnNextStatement(reason_);
        }

        void onResponse(const std::string& message);

        void waitFrontendMessage();

    private:
        JNIEnv *env;
        jclass v8InspectorCls = nullptr;
        jclass v8InspectorDelegateCls = nullptr;
        jmethodID v8InspectorDelegateOnResponseMethodID = nullptr;
        jmethodID v8InspectorDelegateWaitFrontendMessageMethodID = nullptr;

        inline v8_inspector::StringView convertSTDStringToStringView(const std::string &str) {
            auto* stringView = reinterpret_cast<const uint8_t*>(str.c_str());
            return { stringView, str.length() };
        }
    };
}



#endif //PALADIN_ANDROID_RUNTIME_V8INSPECTOR_H
