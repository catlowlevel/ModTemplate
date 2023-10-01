//
//  Logger.h
//
//  Created by MJ (Ruit) on 1/1/19.
//

#ifndef Logger_h
#define Logger_h

#include <jni.h>
#include <android/log.h>

#define LOG_TAG "Mod_Menu"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

#define LOGPTR(ptr) LOGD(#ptr " => %p", ptr)
#define LOGINT(var) LOGD(#var " => %d", var)
#define LOGSINGLE(var) LOGD(#var " => %f", var)
#define LOGSTR(il2cppstring) LOGD(#il2cppstring " => %s", il2cppstring->to_string().c_str())

#endif /* Logger_h */
