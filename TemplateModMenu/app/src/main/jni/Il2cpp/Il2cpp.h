//
// Created by Perfare on 2020/7/4.
//

#ifndef ZYGISK_IL2CPPDUMPER_IL2CPP_DUMP_H
#define ZYGISK_IL2CPPDUMPER_IL2CPP_DUMP_H

#include "il2cpp-class.h"

namespace Il2cpp {
    //@formatter:off

    void Init();
    bool EnsureAttached();
    Il2CppDomain *GetDomain();
    Il2CppImage *GetImage(Il2CppAssembly *assembly);
    Il2CppAssembly *GetAssembly(const char *name);
    Il2CppClass *GetClass(Il2CppImage *image, const char *name);

    //class
    FieldInfo *GetField(Il2CppClass *klass, const char *fieldName);
    FieldInfo *GetFields(Il2CppClass *klass, void **iter);
    MethodInfo *GetMethods(Il2CppClass *klass, void **iter);
    MethodInfo *GetClassMethod(Il2CppClass *klass, const char *name, int argsCount = -1);
    int32_t GetInstanceSize(Il2CppClass *klass);
    const char* GetClassName(Il2CppClass *klass);
    std::vector<Il2CppClass *> GetClasses();

    //object
    uint32_t GetObjectSize(Il2CppObject *object);

    //method
    uint32_t GetParamCount(MethodInfo* method);
    const char* GetParamName(MethodInfo* method, uint32_t index);
    const char* GetMethodName(MethodInfo* method);
    Il2CppType* GetMethodReturnType(MethodInfo* method);

    //field
    void GetFieldValue(Il2CppObject *object, FieldInfo *field, void *outValue);
    Il2CppObject *GetFieldValueObect(Il2CppObject *object, FieldInfo *field);
    FieldInfo * GetField(Il2CppClass *klass, const char *fieldName);
    uintptr_t GetFieldOffset(FieldInfo* field);
    Il2CppType* GetFieldType(FieldInfo* field);

    //type
    Il2CppClass * GetClassFromType(Il2CppType *type);
    bool GetTypeIsPointer(Il2CppType *type);
    const char* GetTypeName(Il2CppType* type);


    //@formatter:on
//void il2cpp_dump(const char *outDir);


}

#endif //ZYGISK_IL2CPPDUMPER_IL2CPP_DUMP_H
