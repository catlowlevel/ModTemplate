//
// Created by misman on 02/09/23.
//

#include "il2cpp-class.h"
#include "Il2cpp.h"

const char *Il2CppClass::getName() {
    return Il2cpp::GetClassName(this);
}

Il2CppImage *Il2CppAssembly::getImage() {
    return Il2cpp::GetImage(this);
}

Il2CppClass *Il2CppImage::getClass(const char *name) {
    return Il2cpp::GetClass(this, name);
}

MethodInfo *Il2CppClass::getMethod(const char *name) {
    return Il2cpp::GetClassMethod(this, name);
}

std::vector<MethodInfo *> Il2CppClass::getMethods() {
    void *iter = nullptr;
    std::vector<MethodInfo *> methods{};
    while (auto method = Il2cpp::GetMethods(this, &iter)) {
        methods.push_back(method);
    }
    return methods;
}

const char *MethodInfo::getName() {
    return Il2cpp::GetMethodName(this);
}

Il2CppType *MethodInfo::getReturnType() {
    return Il2cpp::GetMethodReturnType(this);
}

uintptr_t Il2CppObject::_getFieldOffset(const char *name) {
    return Il2cpp::GetFieldOffset(this->klass->getField(name));
}

FieldInfo *Il2CppClass::getField(const char *fieldName) {
    return Il2cpp::GetField(this, fieldName);
}

size_t Il2CppClass::getSize() {
    return Il2cpp::GetInstanceSize(this);
}

uintptr_t FieldInfo::getValue(Il2CppObject *instance) {
    uintptr_t value;
    Il2cpp::GetFieldValue(instance, this, &value);
    return value;
}

Il2CppType *FieldInfo::getType() {
    return Il2cpp::GetFieldType(this);
}

bool Il2CppType::isPointer() {
    return Il2cpp::GetTypeIsPointer(this);
}

const char *Il2CppType::getName() {
    return Il2cpp::GetTypeName(this);
}
