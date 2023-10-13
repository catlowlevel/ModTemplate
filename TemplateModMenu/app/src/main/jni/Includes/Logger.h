#pragma once
#include <jni.h>
#include <android/log.h>

#define LOG_TAG "MXP"
#ifdef __DEBUG__
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#else
#define LOGD(...)
#define LOGW(...)
#define LOGE(...)
#define LOGI(...)
#endif // DEBUG

#define LOGPTR(ptr) LOGD(#ptr " => 0x%llX", ptr)
#define LOGINT(var) LOGD(#var " => %d", var)
#define LOGSINGLE(var) LOGD(#var " => %f", ptr)
#define LOGSTR(il2cppstring) LOGD(#il2cppstring " => %s", il2cppstring->to_string().c_str())
