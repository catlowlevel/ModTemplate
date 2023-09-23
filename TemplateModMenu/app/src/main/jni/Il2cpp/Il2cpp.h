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
    Il2CppImage *GetCorlib();
    Il2CppImage *GetImage(const char *assemblyName);
    Il2CppAssembly *GetAssembly(const char *name);
    Il2CppClass *GetClass(Il2CppImage *image, const char *name);

    //class
    FieldInfo *GetClassField(Il2CppClass *klass, const char *fieldName);
    FieldInfo *GetClassFields(Il2CppClass *klass, void **iter);
    MethodInfo *GetClassMethods(Il2CppClass *klass, void **iter);
    MethodInfo *GetClassMethod(Il2CppClass *klass, const char *name, int argsCount = -1);
    Il2CppImage* GetClassImage(Il2CppClass *klass);
    int32_t GetClassSize(Il2CppClass *klass);
    const char* GetClassName(Il2CppClass *klass);
    const char *GetClassNamespace(Il2CppClass *klass);
    std::vector<Il2CppClass *> GetClasses();
    Il2CppType* GetClassType(Il2CppClass* klass);

    //object
    uint32_t GetObjectSize(Il2CppObject *object);

    //image
    const char *GetImageName(Il2CppImage *image);

    //method
    uint32_t GetMethodParamCount(MethodInfo* method);
    const char* GetMethodParamName(MethodInfo* method, uint32_t index);
    const char* GetMethodName(MethodInfo* method);
    Il2CppType* GetMethodReturnType(MethodInfo* method);
    Il2CppType* GetMethodParam(MethodInfo* method, uint32_t index);
    bool GetIsMethodGeneric(MethodInfo* method);
    bool GetIsMethodInflated(MethodInfo* method);

    //field
    void GetFieldValue(Il2CppObject *object, FieldInfo *field, void *outValue);
    void GetFieldStaticValue(FieldInfo *field, void *outValue);
    void SetFieldValue(Il2CppObject *object, FieldInfo *field, void *newValue);
    void SetFieldStaticValue(FieldInfo *field, void *outValue);
//    Il2CppObject *GetFieldValueObect(Il2CppObject *object, FieldInfo *field);
    uintptr_t GetFieldOffset(FieldInfo* field);
    Il2CppType* GetFieldType(FieldInfo* field);
    const char* GetFieldName(FieldInfo* field);

    //type
    Il2CppClass * GetClassFromType(Il2CppType *type);
    bool GetTypeIsPointer(Il2CppType *type);
    const char* GetTypeName(Il2CppType* type);
    Il2CppObject * GetTypeObject(Il2CppType* type);

    //string
    const char* GetChars(Il2CppString* str); //returns wide char

    //array
    uint32_t GetArrayLength(_Il2CppArray* array);


    //@formatter:on
//void il2cpp_dump(const char *outDir);
}

#endif //ZYGISK_IL2CPPDUMPER_IL2CPP_DUMP_H
