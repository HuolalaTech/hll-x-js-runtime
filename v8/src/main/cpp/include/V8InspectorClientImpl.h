//
// Created by nieyinyin on 2023/11/14.
//

#ifndef PALADIN_ANDROID_RUNTIME_V8INSPECTORCLIENTIMPL_H
#define PALADIN_ANDROID_RUNTIME_V8INSPECTORCLIENTIMPL_H


#include "InspectorDelegate.h"
#include "V8InspectorChannelImpl.h"
#include <libplatform.h>

using namespace std;

namespace paladin {
    class V8InspectorClientImpl final: public v8_inspector::V8InspectorClient {
    public:
        V8InspectorClientImpl(v8::Isolate *isolate, const std::unique_ptr<v8::Platform> &platform,
                              InspectorDelegate *inspectorDelegate, std::string contextName) {
            isolate_ = isolate;
            context_ = isolate->GetCurrentContext();
            platform_ = platform.get();
            channel_ = std::unique_ptr<V8InspectorChannelImpl>(
                    new V8InspectorChannelImpl(isolate, inspectorDelegate));
            inspector_ = v8_inspector::V8Inspector::create(isolate, this);
            session_ = inspector_->connect(kContextGroupId, channel_.get(), v8_inspector::StringView());
            context_->SetAlignedPointerInEmbedderData(1, this);

            inspector_->contextCreated(
                    v8_inspector::V8ContextInfo(isolate->GetCurrentContext(),
                                                kContextGroupId,
                                                convertSTDStringToStringView(contextName))
            );
        }

        void dispatchProtocolMessage(const v8_inspector::StringView &message_view) {
            session_->dispatchProtocolMessage(message_view);
        }

        void runMessageLoopOnPause(int) override {
            if (run_nested_loop_) {
                return;
            }
            terminated_ = false;
            run_nested_loop_ = true;
            while (!terminated_ && channel_->waitFrontendMessageOnPause()) {
                while (v8::platform::PumpMessageLoop(platform_, isolate_)) {}
            }
            terminated_ = true;
            run_nested_loop_ = false;
        }

        void quitMessageLoopOnPause() override {
            terminated_ = true;
        }

        void schedulePauseOnNextStatement(const v8_inspector::StringView &reason) {
            session_->schedulePauseOnNextStatement(reason, reason);
        }

    private:
        static const int kContextGroupId = 1;
        v8::Isolate *isolate_;
        v8::Handle<v8::Context> context_;
        v8::Platform *platform_;
        unique_ptr<v8_inspector::V8Inspector> inspector_;
        unique_ptr<v8_inspector::V8InspectorSession> session_;
        unique_ptr<V8InspectorChannelImpl> channel_;
        uint8_t terminated_ = 0;
        uint8_t run_nested_loop_ = 0;

        inline v8_inspector::StringView convertSTDStringToStringView(const std::string &str) {
            auto *stringView = reinterpret_cast<const uint8_t *>(str.c_str());
            return {stringView, str.length()};

        };

    };
}


#endif //PALADIN_ANDROID_RUNTIME_V8INSPECTORCLIENTIMPL_H
