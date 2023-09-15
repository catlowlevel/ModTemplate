//
// Created by misman on 02/09/23.
//

#include <string>
#include <locale>
#include <codecvt>
#include <unordered_map>
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

const char *Il2CppImage::getName() {
    return Il2cpp::GetImageName(this);
}

MethodInfo *Il2CppClass::getMethod(const char *name) {
    return Il2cpp::GetClassMethod(this, name);
}

std::vector<MethodInfo *> Il2CppClass::getMethods() {
    void *iter = nullptr;
    std::vector<MethodInfo *> methods{};
    while (auto method = Il2cpp::GetClassMethods(this, &iter)) {
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

std::unordered_map<uintptr_t, bool> alreadyHooked{};

bool MethodInfo::_isAlreadyHooked(uintptr_t ptr) {
    if (alreadyHooked.find(ptr) != alreadyHooked.end()) {
        return true;
    }
    return false;
}

void MethodInfo::_addToHookedMap(uintptr_t ptr) {
    alreadyHooked[ptr] = true;
}

std::vector<std::pair<const char *, Il2CppType *>> MethodInfo::getParamsInfo() {
    auto count = Il2cpp::GetMethodParamCount(this);
    std::vector<std::pair<const char *, Il2CppType *>> params{};
    for (int i = 0; i < count; i++) {
        params.emplace_back(Il2cpp::GetMethodParamName(this, i), Il2cpp::GetMethodParam(this, i));
    }
    return params;
}

uintptr_t Il2CppObject::_getFieldOffset(const char *name) {
    return Il2cpp::GetFieldOffset(this->klass->getField(name));
}

FieldInfo *Il2CppClass::getField(const char *fieldName) {
    return Il2cpp::GetClassField(this, fieldName);
}

size_t Il2CppClass::getSize() {
    return Il2cpp::GetClassSize(this);
}

Il2CppImage *Il2CppClass::getImage() {
    return Il2cpp::GetClassImage(this);
}

const char *Il2CppClass::getNamespace() {
    return Il2cpp::GetClassNamespace(this);
}

std::vector<FieldInfo *> Il2CppClass::getFields() {
    void *iter = nullptr;
    std::vector<FieldInfo *> fields{};
    while (auto field = Il2cpp::GetClassFields(this, &iter)) {
        fields.push_back(field);
    }
    return fields;
}

MethodInfo *Il2CppClass::findMethod(const char *name, size_t idx) {
    std::vector<MethodInfo *> found{};
    for (auto m: this->getMethods()) {
        const char *methodName = m->getName();
        if (strstr(name, methodName) != nullptr) {
            found.push_back(m);
        }

    }

    if (idx >= found.size()) {
        return found.back();
    }

    return found.at(idx);
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
    Il2cpp::GetFieldStaticValue(this, &value);
    return value;
}

void FieldInfo::setStaticValue(void *value) {
    Il2cpp::SetFieldStaticValue(this, &value);
}

const char *FieldInfo::getName() {
    return Il2cpp::GetFieldName(this);
}

bool Il2CppType::isPointer() {
    return Il2cpp::GetTypeIsPointer(this);
}

const char *Il2CppType::getName() {
    return Il2cpp::GetTypeName(this);
}

Il2CppClass *Il2CppType::getClass() {
    return Il2cpp::GetClassFromType(this);
}

std::string Il2CppString::to_string() {
    auto chars = Il2cpp::GetChars(this);
    std::u16string u16(reinterpret_cast<const char16_t *>(chars));
    return std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>{}.to_bytes(u16);
}

const char *Il2CppString::to_char() {
    return this->to_string().c_str();
}

uint32_t _Il2CppArray::length() {
    return Il2cpp::GetArrayLength(this);
}
