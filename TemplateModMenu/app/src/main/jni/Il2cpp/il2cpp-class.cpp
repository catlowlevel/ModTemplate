//
// Created by misman on 02/09/23.
//

#include <algorithm>
#include <string>
#include <locale>
#include <codecvt>
#include <unordered_map>
#include "il2cpp-class.h"
#include "Il2cpp.h"
#include "Il2cpp/il2cpp-tabledefs.h"
#include "sstream"

std::string toHex(uintptr_t my_integer)
{

    std::stringstream sstream;
    sstream << "0x" << std::uppercase << std::hex << my_integer;
    return sstream.str();
}

std::string extractClassNameFromTypename(const char *typeName)
{
    std::string nameStr{typeName};
    size_t dotIndex = nameStr.find_last_of('.');
    size_t ltIndex = nameStr.find_first_of("<");
    std::string classNamespace;

    if (dotIndex == std::string::npos)
    {
        classNamespace = "";
    }
    else
    {
        if (ltIndex == std::string::npos)
        {
            classNamespace = nameStr.substr(0, dotIndex);
        }
        else
        {
            if (dotIndex > ltIndex)
            {
                dotIndex = nameStr.find_last_of('.', ltIndex);
                classNamespace = nameStr.substr(0, dotIndex);
            }
        }
    }
    return nameStr.substr(dotIndex + 1);
}

bool isVisited(std::vector<uintptr_t> &visited, Il2CppObject *object)
{
    return visited.size() > 0 && std::find(visited.begin(), visited.end(), (uintptr_t)object) != visited.end();
}

std::vector<std::function<nlohmann::ordered_json(Il2CppObject *, Il2CppType *, size_t)>> callbacks;
void AddCustomDumpHandler(std::function<nlohmann::ordered_json(Il2CppObject *, Il2CppType *, size_t)> handler)
{
    callbacks.push_back(handler);
}

nlohmann::ordered_json Handler(Il2CppObject *object, Il2CppType *type, size_t maxDepth)
{
    for (auto callback : callbacks)
    {
        auto res = callback(object, type, maxDepth);
        if (!res.empty())
        {
            return res;
        }
    }
    return nlohmann::ordered_json{};
}

// object may be null
nlohmann::ordered_json DumpObject(Il2CppObject *object, Il2CppType *type, std::vector<uintptr_t> &visited,
                                  size_t maxDepth)
{
// LOGD(("DumpObject %llX %s"), object, type->getName());
#define MAX_LISTARRAY 5

    if (type->isPointer())
    {
        return "(unhandled-pointer)";
    }

    auto custom = Handler(object, type, maxDepth);
    if (!custom.empty())
    {
        return custom;
    }

    // THE ORDER OF THIS IS IMPORTANT!
    if (std::string(type->getName()) == "System.String")
    {
        auto strObj = (Il2CppString *)object;
        if (strObj)
        {
            return strObj->to_string();
        }
        else
        {
            return "(null-string)";
        }
    }
    else if (type->isPrimitive())
    {
        auto str = object->invoke_method<Il2CppString *>("ToString")->to_string();
        return str;
    }
    else if (type->isEnum())
    {
        if (object)
        {
            auto str = object->invoke_method<Il2CppString *>("ToString")->to_string();
            return str;
        }
        else
        {
            return "(null-enum)";
        }
    }
    else if (type->isValueType())
    {
        if (object)
        {
            return object->dump(visited, maxDepth - 1);
        }
        else
        {
            return "(null-value-type)";
        }
    }
    else if (type->isObject() && !(type->isList() || type->isArray()))
    {
        if (object)
        {
            // addVisited(visited, object);
            return object->dump(visited, maxDepth - 1);
        }
        else
        {
            return "(null-object)";
        }
    }
    else if (type->isArray())
    {
        if (object)
        {
            // addVisited(visited, object);
            auto arr = (Il2CppArray<ValueType<uintptr_t>> *)object;
            nlohmann::ordered_json array = nlohmann::ordered_json::array();
            auto arrLen = arr->length();
            for (int i = 0; i < std::min(arrLen, uint32_t(MAX_LISTARRAY)); i++)
            {
                auto current = arr->invoke_method<Il2CppObject *>("System.Collections.IList.get_Item", i);
                if (current)
                {
                    auto currentType = Il2cpp::GetClassType(current->klass);
                    if (currentType->isPrimitive() || currentType->isEnum())
                    {
                        array.push_back(DumpObject(current, currentType, visited, maxDepth - 1));
                    }
                    else
                    {

                        // FIXME: "System.Int32[]": "(no-fields)"
                        array.push_back(current->dump(visited, maxDepth - 1));
                    }
                }
                else
                {
                    array.push_back("(null-array-item)");
                }
            }

            if (arrLen > MAX_LISTARRAY)
            {
                array.push_back(std::to_string(arrLen - MAX_LISTARRAY) + " more...");
            }
            return array;
        }
        else
        {
            return "(null-array)";
        }
    }
    else if (type->isList())
    {
        auto list = (List<uintptr_t> *)object;
        if (list)
        {
            // addVisited(visited, object);
            nlohmann::ordered_json arr = nlohmann::ordered_json::array();
            auto listLen = list->size();
            for (int i = 0; i < std::min(listLen, MAX_LISTARRAY); i++)
            {
                auto current = list->invoke_method<Il2CppObject *>("System.Collections.IList.get_Item", i);
                if (current)
                {
                    auto currentType = Il2cpp::GetClassType(current->klass);
                    if (currentType->isPrimitive() || currentType->isEnum())
                    {
                        arr.push_back(DumpObject(current, currentType, visited, maxDepth - 1));
                    }
                    else
                    {
                        arr.push_back({{currentType->getName(), current->dump(visited, maxDepth - 1)}});
                    }
                    // arr.push_back(DumpObject(current, currentType, maxDepth - 1));
                }
                else
                {
                    arr.push_back("(null-list-item)");
                }
            }
            if (listLen > MAX_LISTARRAY)
            {
                arr.push_back(std::to_string(listLen - MAX_LISTARRAY) + " more...");
            }
            return arr;
        }
        else
        {
            return "(null-list)";
        }
    }
    // return "(unhandled) (" + std ::string(to_hex(type->type)) + ")";
    // return "(unhandled) (" + std ::string(type->getName()) + ")";
    return "(unhandled)";
}

nlohmann::ordered_json Il2CppObject::dump(std::vector<uintptr_t> &visited, int maxDepth)
{
    if (isVisited(visited, this))
    {
        return "(circular-reference (" + toHex((uintptr_t)this) + "))";
    }
    else
    {
        visited.push_back((uintptr_t)this);
    }

    if (maxDepth == 0)
        return "(" + toHex((uintptr_t)this) + ")";

    auto thisType = Il2cpp::GetClassType(this->klass);

    // if (thisType->isPrimitive())
    // {
    //     auto str = this->invoke_method<Il2CppString *>("ToString")->to_string();
    //     return str;
    // }
    // else if (thisType->isValueType())
    // {
    //     // auto str = this->invoke_method<Il2CppString *>("ToString")->to_string();
    //     // return str;
    //     return "(value-type)";
    // }
    // else if (std::string(thisType->getName()) == "System.String")
    // {
    //     auto str = ((Il2CppString *)this)->to_string();
    //     return str;
    // }

    auto fields = this->klass->getFields();
    if (fields.size() == 0)
        return "(no-fields)";

    nlohmann::ordered_json j;
    for (auto field : fields)
    {

        auto fieldType = field->getType();
        auto fieldName = field->getName();

        if (Il2cpp::GetTypeIsStatic(fieldType) || Il2cpp::GetFieldFlags(field) & FIELD_ATTRIBUTE_STATIC)
        {
            continue;
        }

        auto jsonKey = extractClassNameFromTypename(fieldType->getName()) + " " + fieldName;
        auto fieldObject = Il2cpp::GetFieldValueObject(this, field);
        j[jsonKey] = DumpObject(fieldObject, fieldType, visited, maxDepth);
    }
    return j;
}

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

bool Il2CppType::isPrimitive()
{
    static std::vector<const char *> CSPrimitive = {
        "System.Boolean", "System.Char",   "System.SByte", "System.Byte",   "System.Int16",  "System.UInt16",
        "System.Int32",   "System.UInt32", "System.Int64", "System.UInt64", "System.Single", "System.Double"};
    return std::find_if(CSPrimitive.begin(), CSPrimitive.end(),
                        [this](const char *primitiveName)
                        { return strcmp(this->getName(), primitiveName) == 0; }) != CSPrimitive.end();
}

bool Il2CppType::isValueType()
{
    return Il2cpp::GetClassIsValueType(this->getClass());
}

bool Il2CppType::isEnum()
{
    return Il2cpp::GetClassIsEnum(this->getClass());
}

bool Il2CppType::isList()
{
    return std::string(this->getName()).starts_with("System.Collections.Generic.List");
}

bool Il2CppType::isArray()
{
    switch (this->type)
    {
        case IL2CPP_TYPE_SZARRAY:
        case IL2CPP_TYPE_ARRAY:
            return true;
        default:
            return false;
    }
}

bool Il2CppType::isObject()
{
    switch (this->type)
    {
        case IL2CPP_TYPE_STRING:
        case IL2CPP_TYPE_SZARRAY:
        case IL2CPP_TYPE_CLASS:
        case IL2CPP_TYPE_OBJECT:
        case IL2CPP_TYPE_ARRAY:
        case IL2CPP_TYPE_GENERICINST:
            return true;
        default:
            return false;
    }
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
