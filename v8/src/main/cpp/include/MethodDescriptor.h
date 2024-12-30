//
// Created by nieyinyin on 2023/11/1.
//

#ifndef PALADIN_ANDROID_RUNTIME_METHODDESCRIPTOR_H
#define PALADIN_ANDROID_RUNTIME_METHODDESCRIPTOR_H

#include <string>

class MethodDescriptor {
public:
    std::string funcName;
    int targetExecutor;
};


#endif //PALADIN_ANDROID_RUNTIME_METHODDESCRIPTOR_H
