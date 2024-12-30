//
// Created by nieyinyin on 2022/8/22.
//

#ifndef PALADIN_ANDROID_RUNTIME_CONSTANT_H
#define PALADIN_ANDROID_RUNTIME_CONSTANT_H

#include "jni.h"
#include "log.h"

namespace paladin {

    // ----------------------------------------------------------
    static const constexpr char * kjBooleanClazzStr  = "java/lang/Boolean";
    static const constexpr char * kjIntegerClazzStr = "java/lang/Integer";
    static const constexpr char * kjDoubleClazzStr = "java/lang/Double";
    static const constexpr char * kjStringClazzStr  = "java/lang/String";

    static const constexpr char * kPLDJSObjectClazzStr = "com/paladin/jsruntime/common/jsvalue/JSObject";
    static const constexpr char * kPLDJSArrayClazzStr = "com/paladin/jsruntime/common/jsvalue/JSArray";
    static const constexpr char * kPLDJSValueClazzStr = "com/paladin/jsruntime/common/jsvalue/JSValue";
    static const constexpr char * kPLDJSNumberClazzStr = "com/paladin/jsruntime/common/jsvalue/JSNumber";
    static const constexpr char * kPLDJSNullClazzStr = "com/paladin/jsruntime/common/jsvalue/JSNull";
    static const constexpr char * kPLDJSUndefinedClazzStr = "com/paladin/jsruntime/common/jsvalue/JSUndefined";
    static const constexpr char * kPLDJSBooleanClazzStr = "com/paladin/jsruntime/common/jsvalue/JSBoolean";
    static const constexpr char * kPLDJSStringClazzStr = "com/paladin/jsruntime/common/jsvalue/JSString";

    static const constexpr char * kPLDJavaFunctionStr = "com/paladin/jsruntime/common/JavaFunction";
    static const constexpr char * kPLDNativeArrayStr = "com/paladin/jsruntime/common/PLDNativeArray";

    // -----------------------------------------------------------
    static const constexpr char * kMsgOOM = "Out of memory";
    static const constexpr char * kMsgNullJSRuntime= "Null JSRuntime";
    static const constexpr char * kMsgNullJSContext= "Null JSContext";
    static const constexpr char * kMsgNullQJSUtil= "Null QJSUtil";
    static const constexpr char * KMsgNullJSValue = "Null JSValue";

} // namespace paladin

#endif //PALADIN_ANDROID_RUNTIME_CONSTANT_H
