#pragma once
#include <sstream>
#include "Menu/Menu.h"

// Jni stuff from MrDarkRX https://github.com/MrDarkRXx/DarkMod-Floating
void setDialog(jobject ctx, JNIEnv *env, const char *title, const char *msg);

void Toast(JNIEnv *env, jobject thiz, const char *text, int length);

void startActivityPermisson(JNIEnv *env, jobject ctx);

void startService(JNIEnv *env, jobject ctx);

void *exit_thread(void *);

// Needed jclass parameter because this is a static java method
void CheckOverlayPermission(JNIEnv *env, jclass thiz, jobject ctx);

void Init(JNIEnv *env, jobject thiz, jobject ctx, jobject title, jobject subtitle);
