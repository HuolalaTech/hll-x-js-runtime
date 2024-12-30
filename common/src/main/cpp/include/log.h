//
// Created by nieyinyin on 2021/11/16.
//

#ifndef PALADINCORE_LOG_H
#define PALADINCORE_LOG_H

#include <jni.h>
#include <android/log.h>

#ifndef DEBUG
#define DEBUG 1
#endif

#ifdef DEBUG
#define APP_NAME "com.paladin"
#define LOGV(...) __android_log_print(2, APP_NAME, __VA_ARGS__)
#define LOGD(...) __android_log_print(3, APP_NAME, __VA_ARGS__)
#define LOGI(...) __android_log_print(4, APP_NAME, __VA_ARGS__)
#define LOGW(...) __android_log_print(5, APP_NAME, __VA_ARGS__)
#define LOGE(...) __android_log_print(6, APP_NAME, __VA_ARGS__)
#else
#define LOGV(...) {}
	#define LOGD(...) {}
	#define LOGI(...) {}
	#define LOGW(...) {}
	#define LOGE(...) {}
#endif

#endif //PALADINCORE_LOG_H
