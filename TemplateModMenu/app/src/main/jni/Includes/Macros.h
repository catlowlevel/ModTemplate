// thanks to shmoo and joeyjurjens for the usefull stuff under this comment.
#ifndef ANDROID_MOD_MENU_MACROS_H
#define ANDROID_MOD_MENU_MACROS_H

#define REPLACE_NAME_METHOD(methodInfo, name, method)                                                                  \
    [&]                                                                                                                \
    {                                                                                                                  \
        void *old = methodInfo->methodPointer;                                                                         \
        void *n = methodInfo->replace(method);                                                                         \
        LOGD(OBFUSCATE("%s::%s (%p -> %p) HOOKED"), methodInfo->getClass()->getFullName().c_str(),                     \
             methodInfo->getName(), old, n);                                                                           \
        return n;                                                                                                      \
    }();
// clang-format off
#define REPLACE_NAME_METHOD_ORIG(methodInfo, method, orig)  orig = (decltype(orig))REPLACE_NAME_METHOD(methodInfo, method)
#define REPLACE_NAME_KLASS(klass, name, method)             REPLACE_NAME_METHOD(klass->getMethod(OBFUSCATE(name)), (const char *)OBFUSCATE(name), method)
#define REPLACE_NAME_KLASS_ORIG(klass, name, method, orig)  REPLACE_NAME_METHOD_ORIG(klass->getMethod(OBFUSCATE(name)), (const char *)OBFUSCATE(name), method, orig)
#define REPLACE_KLASS(klass, method)                        REPLACE_NAME_KLASS(klass, #method, method)
#define REPLACE_NAME(className, name, method)               REPLACE_NAME_KLASS(g_Image->getClass((const char *)OBFUSCATE(className)), name, method)
#define REPLACE_NAME_ORIG(className, name, method, orig)    REPLACE_NAME_KLASS_ORIG(g_Image->getClass(OBFUSCATE(className)), name, method, orig)
#define REPLACE(className, method)                          REPLACE_NAME(className, #method, method)
#define REPLACE_ORIG(className, method, orig)               REPLACE_NAME_ORIG(className, #method, method, orig)

#define PATCH(offset, hex) patchOffset(targetLibName, string2Offset(OBFUSCATE(offset)), OBFUSCATE(hex), true)
#define PATCH_LIB(lib, offset, hex) patchOffset(OBFUSCATE(lib), string2Offset(OBFUSCATE(offset)), OBFUSCATE(hex), true)

#define PATCH_SYM(sym, hex) patchOffset(dlsym(dlopen(targetLibName, 4), OBFUSCATE(sym)), OBFUSCATE(hex), true)
#define PATCH_LIB_SYM(lib, sym, hex) patchOffset(dlsym(dlopen(lib, 4), OBFUSCATE(sym)), OBFUSCATE(hex), true)

#define PATCH_SWITCH(offset, hex, boolean) patchOffset(targetLibName, string2Offset(OBFUSCATE(offset)), OBFUSCATE(hex), boolean)
#define PATCH_LIB_SWITCH(lib, offset, hex, boolean) patchOffset(OBFUSCATE(lib), string2Offset(OBFUSCATE(offset)), OBFUSCATE(hex), boolean)

#define PATCH_SYM_SWITCH(sym, hex, boolean) patchOffsetSym((uintptr_t)dlsym(dlopen(targetLibName, 4), OBFUSCATE(sym)), OBFUSCATE(hex), boolean)
#define PATCH_LIB_SYM_SWITCH(lib, sym, hex, boolean) patchOffsetSym((uintptr_t)dlsym(dlopen(lib, 4), OBFUSCATE(sym)), OBFUSCATE(hex), boolean)

#define RESTORE(offset) patchOffset(targetLibName, string2Offset(OBFUSCATE(offset)), "", false)
#define RESTORE_LIB(lib, offset) patchOffset(OBFUSCATE(lib), string2Offset(OBFUSCATE(offset)), "", false)

#define RESTORE_SYM(sym) patchOffsetSym((uintptr_t)dlsym(dlopen(targetLibName, 4), OBFUSCATE(sym)), "", false)
#define RESTORE_LIB_SYM(lib, sym) patchOffsetSym((uintptr_t)dlsym(dlopen(lib, 4), OBFUSCATE(sym)), "", false)

#endif //ANDROID_MOD_MENU_MACROS_H