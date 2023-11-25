//
// Created by Perfare on 2020/7/4.
//

#ifndef ZYGISK_IL2CPPDUMPER_IL2CPP_DUMP_H
#define ZYGISK_IL2CPPDUMPER_IL2CPP_DUMP_H

#include "il2cpp-class.h"

namespace Il2cpp
{
    //@formatter:off

    void Init();
    // void Dump(JavaVM *jvm);
    void Dump(JNIEnv *env);
    bool EnsureAttached();
    void Detach();

    Il2CppDomain *GetDomain();
    Il2CppImage *GetImage(Il2CppAssembly *assembly);
    Il2CppImage *GetCorlib();
    Il2CppImage *GetImage(const char *assemblyName);
    Il2CppAssembly *GetAssembly(const char *name);
    Il2CppClass *GetClass(Il2CppImage *image, const char *name, int subClass = 0);
    const std::tuple<Il2CppAssembly **, size_t> &GetAssemblies();
    const std::vector<Il2CppImage *> &GetImages();

    // class
    FieldInfo *GetClassField(Il2CppClass *klass, const char *fieldName);
    FieldInfo *GetClassFields(Il2CppClass *klass, void **iter);
    MethodInfo *GetClassMethods(Il2CppClass *klass, void **iter);
    MethodInfo *GetClassMethod(Il2CppClass *klass, const char *name, int argsCount = -1);
    Il2CppImage *GetClassImage(Il2CppClass *klass);
    int32_t GetClassSize(Il2CppClass *klass);
    const char *GetClassName(Il2CppClass *klass);
    const char *GetClassNamespace(Il2CppClass *klass);
    std::vector<Il2CppClass *> GetClasses();
    std::vector<Il2CppClass *> GetClasses(Il2CppImage *image, const char *filter = nullptr);
    const std::tuple<Il2CppClass **, size_t> &GetSubClasses(Il2CppClass *klass);
    Il2CppType *GetClassType(Il2CppClass *klass);
    bool GetClassIsGeneric(Il2CppClass *klass);
    Il2CppClass *FindClass(const char *klassName);
    Il2CppClass *GetClassFromSystemType(Il2CppReflectionType *type);

    // object
    uint32_t GetObjectSize(Il2CppObject *object);
    Il2CppObject *NewObject(Il2CppClass *klass);

    // image
    const char *GetImageName(Il2CppImage *image);

    // method
    uint32_t GetMethodParamCount(MethodInfo *method);
    const char *GetMethodParamName(MethodInfo *method, uint32_t index);
    const char *GetMethodName(MethodInfo *method);
    Il2CppType *GetMethodReturnType(MethodInfo *method);
    Il2CppType *GetMethodParam(MethodInfo *method, uint32_t index);
    bool GetIsMethodGeneric(MethodInfo *method);
    bool GetIsMethodInflated(MethodInfo *method);
    Il2CppReflectionMethod *GetMethodObject(MethodInfo *method, Il2CppClass *refclass = nullptr);
    MethodInfo *GetMethodFromReflection(Il2CppReflectionMethod *method);
    uint32_t GetMethodGenericCount(MethodInfo *method);
    MethodInfo *FindMethod(const char *klassName, const char *methodName, size_t argsCount = -1);
    Il2CppClass *GetMethodClass(MethodInfo *method);

    // field
    void GetFieldValue(Il2CppObject *object, FieldInfo *field, void *outValue);
    void GetFieldStaticValue(FieldInfo *field, void *outValue);
    void SetFieldValue(Il2CppObject *object, FieldInfo *field, void *newValue);
    void SetFieldStaticValue(FieldInfo *field, void *outValue);
    //    Il2CppObject *GetFieldValueObect(Il2CppObject *object, FieldInfo *field);
    uintptr_t GetFieldOffset(FieldInfo *field);
    Il2CppType *GetFieldType(FieldInfo *field);
    const char *GetFieldName(FieldInfo *field);

    // type
    Il2CppClass *GetClassFromType(Il2CppType *type);
    Il2CppClass *GetTypeClass(Il2CppType *type);
    bool GetTypeIsPointer(Il2CppType *type);
    const char *GetTypeName(Il2CppType *type);
    Il2CppObject *GetTypeObject(Il2CppType *type);

    // string
    const char *GetChars(Il2CppString *str); // returns wide char
    Il2CppString *NewString(const char *str);

    // array
    uint32_t GetArrayLength(_Il2CppArray *array);
    _Il2CppArray *ArrayNew(Il2CppClass *elementTypeInfo, il2cpp_array_size_t length);
    template <typename T>
    Il2CppArray<T> *ArrayNewGeneric(Il2CppClass *elementTypeInfo, il2cpp_array_size_t length)
    {
        return static_cast<Il2CppArray<T> *>(ArrayNew(elementTypeInfo, length));
    }

    // other
    Il2CppObject *GetBoxedValue(Il2CppClass *klass, void *value);
#if __DEBUG__
    // this is a Debug function, it should be used as a tool only
    void Trace(Il2CppImage *image, std::function<bool(Il2CppClass *)> filterClasses,
               std::function<bool(MethodInfo *)> filterMethods = nullptr, bool nearBranchTrampoline = false,
               int maxSpam = -1);
#endif

    //@formatter:on
    // void il2cpp_dump(const char *outDir);
} // namespace Il2cpp

#endif // ZYGISK_IL2CPPDUMPER_IL2CPP_DUMP_H
