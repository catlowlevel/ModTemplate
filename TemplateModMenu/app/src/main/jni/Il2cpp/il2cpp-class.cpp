//
// Created by misman on 02/09/23.
//

#include <string>
#include <locale>
#include <codecvt>
#include <unordered_map>
#include "il2cpp-class.h"
#include "Il2cpp.h"

const char *Il2CppClass::getName()
{
    return Il2cpp::GetClassName(this);
}

std::string Il2CppClass::getFullName()
{
    auto typeName = Il2cpp::GetClassType(this)->getName();
    // im sure i've encounter a case where `typeName` is nullptr
    if (typeName && strlen(typeName) > 0)
    {
        return typeName;
    }
    else
    {
        std::string name = this->getName();
        std::string_view namespaze = getNamespace();
        if (!namespaze.empty())
        {
            name.insert(0, ".");
            name.insert(0, namespaze);
        }
        return name;
    }
}

Il2CppImage *Il2CppAssembly::getImage()
{
    return Il2cpp::GetImage(this);
}

Il2CppClass *Il2CppImage::getClass(const char *name)
{
    return Il2cpp::GetClass(this, name);
}

std::vector<Il2CppClass *> Il2CppImage::getClasses(const char *filter)
{
    return Il2cpp::GetClasses(this, filter);
}

const char *Il2CppImage::getName()
{
    return Il2cpp::GetImageName(this);
}

MethodInfo *Il2CppClass::getMethod(const char *name, size_t argsCount)
{
    return Il2cpp::GetClassMethod(this, name, argsCount);
}

MethodInfo *Il2CppClass::getMethod(const char *name, std::vector<std::string> args)
{

    for (auto m : this->getMethods())
    {
        int matched = 0;
        const char *methodName = m->getName();
        if (strcmp(methodName, name) == 0)
        {
            auto count = Il2cpp::GetMethodParamCount(m);
            for (int i = 0; i < args.size(); i++)
            {
                Il2CppType *arg = Il2cpp::GetMethodParam(m, i);
                if (arg)
                {
                    auto typeName = arg->getName();
                    if (strcmp(typeName, args[i].c_str()) == 0)
                    {
                        matched++;
                    }
                    else
                    {
                        LOGD("Argument at index %d didn't matched requested "
                             "argument!\n\tRequested: %s\n\tActual: "
                             "%s\nnSkipping function...",
                             i, args[i].c_str(), typeName);
                        matched = 0;
                        break;
                    }
                }
            }
        }
        if (matched == args.size())
        {
            LOGD("%s - [%s] %s::%s: %p", getImage()->getName(), getNamespace(), getName(), name, m);
            return m;
        }
    }
    return nullptr;
}

std::vector<MethodInfo *> Il2CppClass::getMethods(const char *filter)
{
    void *iter = nullptr;
    std::vector<MethodInfo *> methods{};
    while (auto method = Il2cpp::GetClassMethods(this, &iter))
    {
        if (filter != nullptr && strstr(method->getName(), filter) == nullptr)
        {
            continue;
        }
        methods.push_back(method);
    }
    return methods;
}

const char *MethodInfo::getName()
{
    return Il2cpp::GetMethodName(this);
}

Il2CppType *MethodInfo::getReturnType()
{
    return Il2cpp::GetMethodReturnType(this);
}

std::unordered_map<uintptr_t, intptr_t> alreadyHooked{};

bool MethodInfo::_isAlreadyHooked(uintptr_t ptr)
{
    if (alreadyHooked.find(ptr) != alreadyHooked.end())
    {
        return true;
    }
    return false;
}

void MethodInfo::_addToHookedMap(uintptr_t ptr, uintptr_t oPtr)
{
    alreadyHooked[ptr] = oPtr;
}

std::vector<std::pair<const char *, Il2CppType *>> MethodInfo::getParamsInfo()
{
    auto count = Il2cpp::GetMethodParamCount(this);
    std::vector<std::pair<const char *, Il2CppType *>> params{};
    for (int i = 0; i < count; i++)
    {
        params.emplace_back(Il2cpp::GetMethodParamName(this, i), Il2cpp::GetMethodParam(this, i));
    }
    return params;
}

MethodInfo *MethodInfo::inflate(std::initializer_list<Il2CppClass *> types)
{
    if (types.size() != Il2cpp::GetMethodGenericCount(this))
    {
        LOGE("Types generic count doesn't match");
        return nullptr;
    }
    static auto corlib = Il2cpp::GetCorlib();
    static auto systemType = corlib->getClass("System.Type");
    auto array = Il2cpp::ArrayNewGeneric<Il2CppObject *>(systemType, types.size());
    int i = 0;
    for (auto type : types)
    {
        auto typeObj = Il2cpp::GetTypeObject(Il2cpp::GetClassType(type));
        array->data[i] = typeObj;
        i++;
    }
    auto methodObj = this->getObject();
    auto result = methodObj->invoke_method<Il2CppReflectionMethod *>("MakeGenericMethod", array);
    return Il2cpp::GetMethodFromReflection(result);
}

Il2CppClass *Il2CppClass::inflate(std::initializer_list<Il2CppClass *> types)
{
    // TODO: check generic count
    // if (types.size() != Il2cpp::GetMethodGenericCount(this))
    // {
    //     LOGE("Types generic count doesn't match");
    //     return nullptr;
    // }
    static auto corlib = Il2cpp::GetCorlib();
    static auto systemType = corlib->getClass("System.Type");
    auto array = Il2cpp::ArrayNewGeneric<Il2CppObject *>(systemType, types.size());
    int i = 0;
    for (auto type : types)
    {
        auto typeObj = Il2cpp::GetTypeObject(Il2cpp::GetClassType(type));
        array->data[i] = typeObj;
        i++;
    }
    // auto methodObj = this->getObject();
    auto obj = Il2cpp::GetTypeObject(Il2cpp::GetClassType(this));
    auto result = obj->invoke_method<Il2CppReflectionType *>("MakeGenericType", array);
    return Il2cpp::GetClassFromSystemType(result);
}

Il2CppObject *MethodInfo::getObject()
{
    return (Il2CppObject *)Il2cpp::GetMethodObject(this);
}

uintptr_t MethodInfo::_getHookedMap(uintptr_t ptr)
{
    auto it = alreadyHooked.find(ptr);
    if (it != alreadyHooked.end())
    {
        return it->second;
    }
    return ptr;
}

uintptr_t Il2CppObject::_getFieldOffset(const char *name)
{
    return Il2cpp::GetFieldOffset(this->klass->getField(name));
}

FieldInfo *Il2CppClass::getField(const char *fieldName)
{
    return Il2cpp::GetClassField(this, fieldName);
}

size_t Il2CppClass::getSize()
{
    return Il2cpp::GetClassSize(this);
}

Il2CppImage *Il2CppClass::getImage()
{
    return Il2cpp::GetClassImage(this);
}

const char *Il2CppClass::getNamespace()
{
    return Il2cpp::GetClassNamespace(this);
}

std::vector<FieldInfo *> Il2CppClass::getFields()
{
    void *iter = nullptr;
    std::vector<FieldInfo *> fields{};
    while (auto field = Il2cpp::GetClassFields(this, &iter))
    {
        fields.push_back(field);
    }
    return fields;
}

MethodInfo *Il2CppClass::findMethod(const char *name, size_t idx)
{
    std::vector<MethodInfo *> found{};
    for (auto m : this->getMethods())
    {
        const char *methodName = m->getName();
        if (strstr(name, methodName) != nullptr)
        {
            found.push_back(m);
        }
    }

    if (idx >= found.size())
    {
        return found.back();
    }

    return found.at(idx);
}

bool Il2CppClass::isGeneric()
{
    return Il2cpp::GetClassIsGeneric(this);
}

Il2CppType *FieldInfo::getType()
{
    return Il2cpp::GetFieldType(this);
}

uintptr_t FieldInfo::getOffset()
{
    return Il2cpp::GetFieldOffset(this);
}

const char *FieldInfo::getName()
{
    return Il2cpp::GetFieldName(this);
}

bool Il2CppType::isPointer()
{
    return Il2cpp::GetTypeIsPointer(this);
}

const char *Il2CppType::getName()
{
    return Il2cpp::GetTypeName(this);
}

Il2CppClass *Il2CppType::getClass()
{
    return Il2cpp::GetClassFromType(this);
}

std::string Il2CppString::to_string()
{
    auto chars = Il2cpp::GetChars(this);
    std::u16string u16(reinterpret_cast<const char16_t *>(chars));
    return std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>{}.to_bytes(u16);
}

const char *Il2CppString::to_char()
{
    return this->to_string().c_str();
}

uint32_t _Il2CppArray::length()
{
    return Il2cpp::GetArrayLength(this);
}
Il2CppObject *Il2CppClass::New()
{
    return Il2cpp::NewObject(this);
}

Il2CppClass *MethodInfo::getClass()
{
    return Il2cpp::GetMethodClass(this);
}
