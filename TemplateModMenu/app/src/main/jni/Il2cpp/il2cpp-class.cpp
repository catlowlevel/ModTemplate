//
// Created by misman on 02/09/23.
//

#include <string>
#include <locale>
#include <codecvt>
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

void FieldInfo::setValue(Il2CppObject *instance, void *value) {
    Il2cpp::SetFieldValue(instance, this, &value);
}

uintptr_t FieldInfo::getOffset() {
    return Il2cpp::GetFieldOffset(this);
}

uintptr_t FieldInfo::getStaticValue() {
    uintptr_t value;
    Il2cpp::GetStaticFieldValue(this, &value);
    return value;
}

void FieldInfo::setStaticValue(void *value) {
    Il2cpp::SetStaticFieldValue(this, &value);
}

bool Il2CppType::isPointer() {
    return Il2cpp::GetTypeIsPointer(this);
}

const char *Il2CppType::getName() {
    return Il2cpp::GetTypeName(this);
}

std::string Il2CppString::to_string() {
    auto chars = Il2cpp::GetChars(this);
    std::u16string u16(reinterpret_cast<const char16_t *>(chars));
    return std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>{}.to_bytes(u16);
}

const char *Il2CppString::to_char() {
    return this->to_string().c_str();
}
