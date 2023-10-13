#ifndef UTILS
#define UTILS

#include <jni.h>

typedef unsigned long DWORD;

DWORD findLibrary(const char *library);

DWORD getAbsoluteAddress(const char *libraryName, DWORD relativeAddr);

jboolean isGameLibLoaded(JNIEnv *env, jobject thiz);

bool isLibraryLoaded(const char *libraryName);

uintptr_t string2Offset(const char *c);

namespace ToastLength
{
inline const int LENGTH_LONG = 1;
inline const int LENGTH_SHORT = 0;
} // namespace ToastLength

#endif
