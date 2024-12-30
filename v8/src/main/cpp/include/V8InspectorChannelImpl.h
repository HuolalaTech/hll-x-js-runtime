//
// Created by nieyinyin on 2023/11/14.
//

#ifndef PALADIN_ANDROID_RUNTIME_V8INSPECTORCHANNELIMPL_H
#define PALADIN_ANDROID_RUNTIME_V8INSPECTORCHANNELIMPL_H

#include <v8-inspector.h>

namespace paladin {
    class V8InspectorChannelImpl final: public v8_inspector::V8Inspector::Channel
    {
    public:
        V8InspectorChannelImpl(v8::Isolate* isolate, InspectorDelegate* inspectorDelegate) {
            isolate_ = isolate;
            inspectorDelegate_ = inspectorDelegate;
        }

        void sendResponse(int, std::unique_ptr<v8_inspector::StringBuffer> message) override {
            const std::string response = convertStringViewToSTDString(isolate_, message->string());
            inspectorDelegate_->emitOnResponse(response);
        }

        void sendNotification(std::unique_ptr<v8_inspector::StringBuffer> message) override {
            const std::string notification = convertStringViewToSTDString(isolate_, message->string());
            inspectorDelegate_->emitOnResponse(notification);
        }

        void flushProtocolNotifications() override {}

        uint8_t waitFrontendMessageOnPause() {
            inspectorDelegate_->emitWaitFrontendMessage();
            return 1;
        }

    private:
        v8::Isolate* isolate_;
        InspectorDelegate* inspectorDelegate_;

        inline std::string convertStringViewToSTDString(v8::Isolate* isolate, const v8_inspector::StringView stringView) {
            int length = static_cast<int>(stringView.length());
            v8::Local<v8::String> message = (
                    stringView.is8Bit()
                    ? v8::String::NewFromOneByte(isolate, reinterpret_cast<const uint8_t*>(stringView.characters8()), v8::NewStringType::kNormal, length)
                    : v8::String::NewFromTwoByte(isolate, reinterpret_cast<const uint16_t*>(stringView.characters16()), v8::NewStringType::kNormal, length)
            ).ToLocalChecked();
            v8::String::Utf8Value result(isolate, message);
            return *result;
        }
    };
}




#endif //PALADIN_ANDROID_RUNTIME_V8INSPECTORCHANNELIMPL_H
