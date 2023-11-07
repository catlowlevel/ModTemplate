// thanks to shmoo and joeyjurjens for the usefull stuff under this comment.
#ifndef ANDROID_MOD_MENU_MACROS_H
#define ANDROID_MOD_MENU_MACROS_H

#define REPLACE_METHOD(methodInfo, to)                                                                                 \
    [&]                                                                                                                \
    {                                                                                                                  \
        void *old = methodInfo->methodPointer;                                                                         \
        void *n = methodInfo->replace(to);                                                                             \
        LOGD(OBFUSCATE("%s::%s (%p -> %p) HOOKED"), methodInfo->getClass()->getFullName().c_str(),                     \
             methodInfo->getName(), old, n);                                                                           \
        return n;                                                                                                      \
    }();
// clang-format off
#define REPLACE_NAME_METHOD_ORIG(methodInfo, to, orig)  orig = (decltype(orig))REPLACE_METHOD(methodInfo, to)
#define REPLACE_NAME_KLASS(klass, methodName, to)             REPLACE_METHOD(klass->getMethod(OBFUSCATE(methodName)), to)
#define REPLACE_NAME_KLASS_ORIG(klass, methodName, to, orig)  REPLACE_NAME_METHOD_ORIG(klass->getMethod(OBFUSCATE(methodName)), to, orig)
#define REPLACE_KLASS(klass, to)                        REPLACE_NAME_KLASS(klass, #to, to)
#define REPLACE_NAME(className, methodName, to)               REPLACE_NAME_KLASS(g_Image->getClass((const char *)OBFUSCATE(className)), methodName, to)
#define REPLACE_NAME_ORIG(className, methodName, to, orig)    REPLACE_NAME_KLASS_ORIG(g_Image->getClass(OBFUSCATE(className)), methodName, to, orig)
#define REPLACE(className, to)                          REPLACE_NAME(className, #to, to)
#define REPLACE_ORIG(className, to, orig)               REPLACE_NAME_ORIG(className, #to, to, orig)

#define REPLACE_NAMESPACE_NAME(namespace,classname,name,method) REPLACE_NAME(namespace "." classname, name, method)
#define REPLACE_NAMESPACE_NAME_ORIG(namespace,classname,name,method,orig) REPLACE_NAME_ORIG(namespace "." classname, name, method, orig)
#define REPLACE_NAMESPACE(namespace,classname,method) REPLACE(namespace "." classname,method)
#define REPLACE_NAMESPACE_ORIG(namespace,classname,method,orig) REPLACE_ORIG(namespace "." classname,method,orig)

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