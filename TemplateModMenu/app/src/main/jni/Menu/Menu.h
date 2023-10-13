#pragma once
#include <jni.h>

// Big letter cause crash
void setText(JNIEnv *env, jobject obj, const char *text);

jstring Icon(JNIEnv *env, jobject thiz);

jstring IconWebViewData(JNIEnv *env, jobject thiz);

jobjectArray SettingsList(JNIEnv *env, jobject activityObject);

void setInitValid(bool valid);
bool getInitValid();
