//
// Created by nieyinyin on 2023/11/14.
//

#ifndef PALADIN_ANDROID_RUNTIME_INSPECTORDELEGATE_H
#define PALADIN_ANDROID_RUNTIME_INSPECTORDELEGATE_H

#include <functional>
#include <string>

using namespace std;
namespace paladin {
    class InspectorDelegate {
    public:
        InspectorDelegate(const std::function<void(std::string)> &onResponse, const std::function<void(void)> &waitFrontendMessage) {
            onResponse_ = onResponse;
            waitFrontendMessage_ = waitFrontendMessage;
        }

        void emitOnResponse(std::string message) {
            onResponse_(message);
        }

        void emitWaitFrontendMessage() {
            waitFrontendMessage_();
        }

    private:
        std::function<void(std::string)> onResponse_;
        std::function<void(void)> waitFrontendMessage_;
    };
}



#endif //PALADIN_ANDROID_RUNTIME_INSPECTORDELEGATE_H
