//
// Created by Perfare on 2020/7/4.
//

#include "Il2cpp.h"
#include <chrono>
#include <dlfcn.h>
#include <cstdlib>
#include <cstring>
#include <cinttypes>
#include <jni.h>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <unistd.h>
#include <unordered_map>
#include <thread>
#include "dobby.h"
#include "il2cpp-tabledefs.h"
#include "il2cpp-class.h"

#include "xdl/include/xdl.h"

#define DO_API(r, n, p) r(*n) p

#include "il2cpp-api-functions.h"

#undef DO_API

static uint64_t il2cpp_base = 0;

void init_il2cpp_api(void *handle){
#define DO_API(r, n, p)                                                                                                \
    {                                                                                                                  \
        n = (r(*) p)xdl_sym(handle, #n, nullptr);                                                                      \
    }

#include "il2cpp-api-functions.h"

#undef DO_API
}

std::string get_method_modifier(uint32_t flags)
{
    std::stringstream outPut;
    auto access = flags & METHOD_ATTRIBUTE_MEMBER_ACCESS_MASK;
    switch (access)
    {
        case METHOD_ATTRIBUTE_PRIVATE:
            outPut << "private ";
            break;
        case METHOD_ATTRIBUTE_PUBLIC:
            outPut << "public ";
            break;
        case METHOD_ATTRIBUTE_FAMILY:
            outPut << "protected ";
            break;
        case METHOD_ATTRIBUTE_ASSEM:
        case METHOD_ATTRIBUTE_FAM_AND_ASSEM:
            outPut << "internal ";
            break;
        case METHOD_ATTRIBUTE_FAM_OR_ASSEM:
            outPut << "protected internal ";
            break;
    }
    if (flags & METHOD_ATTRIBUTE_STATIC)
    {
        outPut << "static ";
    }
    if (flags & METHOD_ATTRIBUTE_ABSTRACT)
    {
        outPut << "abstract ";
        if ((flags & METHOD_ATTRIBUTE_VTABLE_LAYOUT_MASK) == METHOD_ATTRIBUTE_REUSE_SLOT)
        {
            outPut << "override ";
        }
    }
    else if (flags & METHOD_ATTRIBUTE_FINAL)
    {
        if ((flags & METHOD_ATTRIBUTE_VTABLE_LAYOUT_MASK) == METHOD_ATTRIBUTE_REUSE_SLOT)
        {
            outPut << "sealed override ";
        }
    }
    else if (flags & METHOD_ATTRIBUTE_VIRTUAL)
    {
        if ((flags & METHOD_ATTRIBUTE_VTABLE_LAYOUT_MASK) == METHOD_ATTRIBUTE_NEW_SLOT)
        {
            outPut << "virtual ";
        }
        else
        {
            outPut << "override ";
        }
    }
    if (flags & METHOD_ATTRIBUTE_PINVOKE_IMPL)
    {
        outPut << "extern ";
    }
    return outPut.str();
}

bool _il2cpp_type_is_byref(Il2CppType *type)
{
    auto byref = type->byref;
    if (il2cpp_type_is_byref)
    {
        byref = il2cpp_type_is_byref(type);
    }
    return byref;
}

std::string dump_method(Il2CppClass *klass)
{
    std::stringstream outPut;
    outPut << "\n\t// Methods";
    void *iter = nullptr;
    while (auto method = il2cpp_class_get_methods(klass, &iter))
    {
        // TODO attribute
        // if (method->methodPointer)
        // {
        //     outPut << "\t// RVA: 0x";
        //     outPut << std::hex << (uint64_t)method->methodPointer - il2cpp_base;
        //     outPut << " VA: 0x";
        //     outPut << std::hex << (uint64_t)method->methodPointer;
        // }
        // else
        // {
        //     outPut << "\t// RVA: 0x VA: 0x0";
        // }
        /*if (method->slot != 65535) {
            outPut << " Slot: " << std::dec << method->slot;
        }*/
        outPut << "\n\t";
        uint32_t iflags = 0;
        auto flags = il2cpp_method_get_flags(method, &iflags);
        outPut << get_method_modifier(flags);
        // TODO genericContainerIndex
        auto return_type = il2cpp_method_get_return_type(method);
        if (_il2cpp_type_is_byref(return_type))
        {
            outPut << "ref ";
        }
        auto return_class = il2cpp_class_from_type(return_type);
        outPut << il2cpp_class_get_name(return_class) << " " << il2cpp_method_get_name(method) << "(";
        auto param_count = il2cpp_method_get_param_count(method);
        for (int i = 0; i < param_count; ++i)
        {
            auto param = il2cpp_method_get_param(method, i);
            auto attrs = param->attrs;
            if (_il2cpp_type_is_byref(param))
            {
                if (attrs & PARAM_ATTRIBUTE_OUT && !(attrs & PARAM_ATTRIBUTE_IN))
                {
                    outPut << "out ";
                }
                else if (attrs & PARAM_ATTRIBUTE_IN && !(attrs & PARAM_ATTRIBUTE_OUT))
                {
                    outPut << "in ";
                }
                else
                {
                    outPut << "ref ";
                }
            }
            else
            {
                if (attrs & PARAM_ATTRIBUTE_IN)
                {
                    outPut << "[In] ";
                }
                if (attrs & PARAM_ATTRIBUTE_OUT)
                {
                    outPut << "[Out] ";
                }
            }
            auto parameter_class = il2cpp_class_from_type(param);
            outPut << il2cpp_class_get_name(parameter_class) << " " << il2cpp_method_get_param_name(method, i);
            outPut << ", ";
        }
        if (param_count > 0)
        {
            outPut.seekp(-2, outPut.cur);
        }
        outPut << "); // ";
        if (method->methodPointer)
        {
            outPut << "0x";
            outPut << std::hex << (uint64_t)method->methodPointer - il2cpp_base;
        }
        else
        {
            outPut << "0x0";
        }
        // TODO GenericInstMethod
    }
    return outPut.str();
}

std::string dump_property(Il2CppClass *klass)
{
    std::stringstream outPut;
    outPut << "\n\t// Properties\n";
    void *iter = nullptr;
    while (auto prop_const = il2cpp_class_get_properties(klass, &iter))
    {
        // TODO attribute
        auto prop = const_cast<PropertyInfo *>(prop_const);
        auto get = il2cpp_property_get_get_method(prop);
        auto set = il2cpp_property_get_set_method(prop);
        auto prop_name = il2cpp_property_get_name(prop);
        outPut << "\t";
        Il2CppClass *prop_class = nullptr;
        uint32_t iflags = 0;
        if (get)
        {
            outPut << get_method_modifier(il2cpp_method_get_flags(get, &iflags));
            prop_class = il2cpp_class_from_type(il2cpp_method_get_return_type(get));
        }
        else if (set)
        {
            outPut << get_method_modifier(il2cpp_method_get_flags(set, &iflags));
            auto param = il2cpp_method_get_param(set, 0);
            prop_class = il2cpp_class_from_type(param);
        }
        if (prop_class)
        {
            outPut << il2cpp_class_get_name(prop_class) << " " << prop_name << " { ";
            if (get)
            {
                outPut << "get; ";
            }
            if (set)
            {
                outPut << "set; ";
            }
            outPut << "}\n";
        }
        else
        {
            if (prop_name)
            {
                outPut << " // unknown property " << prop_name;
            }
        }
    }
    return outPut.str();
}

std::string dump_field(Il2CppClass *klass)
{
    std::stringstream outPut;
    outPut << "\n\t// Fields\n";
    auto is_enum = il2cpp_class_is_enum(klass);
    void *iter = nullptr;
    while (auto field = il2cpp_class_get_fields(klass, &iter))
    {
        // TODO attribute
        outPut << "\t";
        auto attrs = il2cpp_field_get_flags(field);
        auto access = attrs & FIELD_ATTRIBUTE_FIELD_ACCESS_MASK;
        switch (access)
        {
            case FIELD_ATTRIBUTE_PRIVATE:
                outPut << "private ";
                break;
            case FIELD_ATTRIBUTE_PUBLIC:
                outPut << "public ";
                break;
            case FIELD_ATTRIBUTE_FAMILY:
                outPut << "protected ";
                break;
            case FIELD_ATTRIBUTE_ASSEMBLY:
            case FIELD_ATTRIBUTE_FAM_AND_ASSEM:
                outPut << "internal ";
                break;
            case FIELD_ATTRIBUTE_FAM_OR_ASSEM:
                outPut << "protected internal ";
                break;
        }
        if (attrs & FIELD_ATTRIBUTE_LITERAL)
        {
            outPut << "const ";
        }
        else
        {
            if (attrs & FIELD_ATTRIBUTE_STATIC)
            {
                outPut << "static ";
            }
            if (attrs & FIELD_ATTRIBUTE_INIT_ONLY)
            {
                outPut << "readonly ";
            }
        }
        auto field_type = il2cpp_field_get_type(field);
        auto field_class = il2cpp_class_from_type(field_type);
        outPut << il2cpp_class_get_name(field_class) << " " << il2cpp_field_get_name(field);
        // TODO 获取构造函数初始化后的字段值
        if (attrs & FIELD_ATTRIBUTE_LITERAL && is_enum)
        {
            uint64_t val = 0;
            il2cpp_field_static_get_value(field, &val);
            outPut << " = " << std::dec << val;
        }
        outPut << "; // 0x" << std::hex << il2cpp_field_get_offset(field) << "\n";
    }
    return outPut.str();
}

std::string dump_type(Il2CppType *type)
{
    std::stringstream outPut;
    auto *klass = il2cpp_class_from_type(type);
    // outPut << "\n// Namespace: " << il2cpp_class_get_namespace(klass) << "\n";
    auto flags = il2cpp_class_get_flags(klass);
    if (flags & TYPE_ATTRIBUTE_SERIALIZABLE)
    {
        outPut << "[Serializable]\n";
    }
    // TODO attribute
    auto is_valuetype = il2cpp_class_is_valuetype(klass);
    auto is_enum = il2cpp_class_is_enum(klass);
    auto visibility = flags & TYPE_ATTRIBUTE_VISIBILITY_MASK;
    switch (visibility)
    {
        case TYPE_ATTRIBUTE_PUBLIC:
        case TYPE_ATTRIBUTE_NESTED_PUBLIC:
            outPut << "public ";
            break;
        case TYPE_ATTRIBUTE_NOT_PUBLIC:
        case TYPE_ATTRIBUTE_NESTED_FAM_AND_ASSEM:
        case TYPE_ATTRIBUTE_NESTED_ASSEMBLY:
            outPut << "internal ";
            break;
        case TYPE_ATTRIBUTE_NESTED_PRIVATE:
            outPut << "private ";
            break;
        case TYPE_ATTRIBUTE_NESTED_FAMILY:
            outPut << "protected ";
            break;
        case TYPE_ATTRIBUTE_NESTED_FAM_OR_ASSEM:
            outPut << "protected internal ";
            break;
    }
    if (flags & TYPE_ATTRIBUTE_ABSTRACT && flags & TYPE_ATTRIBUTE_SEALED)
    {
        outPut << "static ";
    }
    else if (!(flags & TYPE_ATTRIBUTE_INTERFACE) && flags & TYPE_ATTRIBUTE_ABSTRACT)
    {
        outPut << "abstract ";
    }
    else if (!is_valuetype && !is_enum && flags & TYPE_ATTRIBUTE_SEALED)
    {
        outPut << "sealed ";
    }
    if (flags & TYPE_ATTRIBUTE_INTERFACE)
    {
        outPut << "interface ";
    }
    else if (is_enum)
    {
        outPut << "enum ";
    }
    else if (is_valuetype)
    {
        outPut << "struct ";
    }
    else
    {
        outPut << "class ";
    }
    auto namespaze = il2cpp_class_get_namespace(klass);
    if (namespaze && strlen(namespaze) > 0)
    {
        outPut << namespaze << ".";
    }
    outPut << il2cpp_class_get_name(klass); // TODO genericContainerIndex
    std::vector<std::string> extends;
    auto parent = il2cpp_class_get_parent(klass);
    if (!is_valuetype && !is_enum && parent)
    {
        auto parent_type = il2cpp_class_get_type(parent);
        if (parent_type->type != IL2CPP_TYPE_OBJECT)
        {
            extends.emplace_back(il2cpp_class_get_name(parent));
        }
    }
    void *iter = nullptr;
    while (auto itf = il2cpp_class_get_interfaces(klass, &iter))
    {
        extends.emplace_back(il2cpp_class_get_name(itf));
    }
    if (!extends.empty())
    {
        outPut << " : " << extends[0];
        for (int i = 1; i < extends.size(); ++i)
        {
            outPut << ", " << extends[i];
        }
    }
    outPut << "\n{";
    outPut << dump_field(klass);
    // outPut << dump_property(klass);
    outPut << dump_method(klass);
    // TODO EventInfo
    outPut << "\n}\n";
    return outPut.str();
}

void il2cpp_dump(const char *outDir)
{
    LOGI("dumping...");
    size_t size;
    auto domain = il2cpp_domain_get();
    auto assemblies = il2cpp_domain_get_assemblies(domain, &size);
    std::stringstream imageOutput;
    // for (int i = 0; i < size; ++i)
    // {
    //     auto image = il2cpp_assembly_get_image(assemblies[i]);
    //     imageOutput << "// Image " << i << ": " << il2cpp_image_get_name(image) << "\n";
    // }
    std::vector<std::string> outPuts;
    if (il2cpp_image_get_class)
    {
        LOGI("Version greater than 2018.3");
        // 使用il2cpp_image_get_class
        for (int i = 0; i < size; ++i)
        {
            auto image = il2cpp_assembly_get_image(assemblies[i]);
            std::stringstream imageStr;
            imageStr << "\n// " << il2cpp_image_get_name(image) << "\n";
            auto classCount = il2cpp_image_get_class_count(image);
            for (int j = 0; j < classCount; ++j)
            {
                auto klass = il2cpp_image_get_class(image, j);
                auto type = il2cpp_class_get_type(const_cast<Il2CppClass *>(klass));
                // LOGD("type name : %s", il2cpp_type_get_name(type));
                auto outPut = imageStr.str() + dump_type(type);
                outPuts.push_back(outPut);
            }
        }
    }
    else
    {
        LOGI("Version less than 2018.3");
        // 使用反射
        auto corlib = il2cpp_get_corlib();
        auto assemblyClass = il2cpp_class_from_name(corlib, "System.Reflection", "Assembly");
        auto assemblyLoad = il2cpp_class_get_method_from_name(assemblyClass, "Load", 1);
        auto assemblyGetTypes = il2cpp_class_get_method_from_name(assemblyClass, "GetTypes", 0);
        if (assemblyLoad && assemblyLoad->methodPointer)
        {
            LOGI("Assembly::Load: %p", assemblyLoad->methodPointer);
        }
        else
        {
            LOGI("miss Assembly::Load");
            return;
        }
        if (assemblyGetTypes && assemblyGetTypes->methodPointer)
        {
            LOGI("Assembly::GetTypes: %p", assemblyGetTypes->methodPointer);
        }
        else
        {
            LOGI("miss Assembly::GetTypes");
            return;
        }
        typedef void *(*Assembly_Load_ftn)(void *, Il2CppString *, void *);
        // typedef _Il2CppArray *(*Assembly_GetTypes_ftn)(void *, void *);
        using Assembly_GetTypes_ftn = Il2CppArray<void *> *(*)(void *, void *);
        for (int i = 0; i < size; ++i)
        {
            auto image = il2cpp_assembly_get_image(assemblies[i]);
            std::stringstream imageStr;
            auto image_name = il2cpp_image_get_name(image);
            imageStr << "\n// " << image_name;
            // LOGD("image name : %s", image->name);
            auto imageName = std::string(image_name);
            auto pos = imageName.rfind('.');
            auto imageNameNoExt = imageName.substr(0, pos);
            auto assemblyFileName = il2cpp_string_new(imageNameNoExt.data());
            auto reflectionAssembly =
                ((Assembly_Load_ftn)assemblyLoad->methodPointer)(nullptr, assemblyFileName, nullptr);
            auto reflectionTypes =
                ((Assembly_GetTypes_ftn)assemblyGetTypes->methodPointer)(reflectionAssembly, nullptr);
            auto items = reflectionTypes->data;
            for (int j = 0; j < reflectionTypes->max_length; ++j)
            {
                auto klass = il2cpp_class_from_system_type((Il2CppReflectionType *)items[j]);
                auto type = il2cpp_class_get_type(klass);
                // LOGD("type name : %s", il2cpp_type_get_name(type));
                auto outPut = imageStr.str() + dump_type(type);
                outPuts.push_back(outPut);
            }
        }
    }
    LOGI("write dump file");
    auto outPath = std::string(outDir).append("/dump.cs");
    std::ofstream outStream(outPath);
    outStream << imageOutput.str();
    auto count = outPuts.size();
    for (int i = 0; i < count; ++i)
    {
        outStream << outPuts[i];
    }
    outStream.close();
    LOGI("dump done! %s", outPath.c_str());
}

void il2cpp_api_init(void *handle)
{
    LOGI("il2cpp_handle: %p", handle);
    init_il2cpp_api(handle);
    if (il2cpp_domain_get_assemblies)
    {
        Dl_info dlInfo;
        if (dladdr((void *)il2cpp_domain_get_assemblies, &dlInfo))
        {
            il2cpp_base = reinterpret_cast<uint64_t>(dlInfo.dli_fbase);
        }
        LOGI("il2cpp_base: %" PRIx64 "", il2cpp_base);
    }
    else
    {
        LOGE("Failed to initialize il2cpp api.");
        return;
    }
    while (!il2cpp_is_vm_thread(nullptr))
    {
        LOGI("Waiting for il2cpp_init...");
        sleep(1);
    }
    //    auto domain = il2cpp_domain_get();
    //    il2cpp_thread_attach(domain);
}

bool g_DoLog = true;
class PauseLog
{
  public:
    PauseLog()
    {
        g_DoLog = false;
    }
    ~PauseLog()
    {
        g_DoLog = true;
    }
};
#define PAUSE_LOG PauseLog _

namespace Il2cpp
{
    void Init()
    {
        auto handle = xdl_open("libil2cpp.so", 0);
        il2cpp_api_init(handle);
        xdl_close(handle);
    }

    void Dump(JNIEnv *env)
    {
        jclass clazz = env->FindClass("com/android/support/Preferences");
        if (!clazz)
        {
            LOGE("Failed to find class: com/android/support/Preferences");
            return;
        }

        jfieldID contextField = env->GetStaticFieldID(clazz, "context", "Landroid/content/Context;");
        if (!contextField)
        {
            LOGE("Failed to find field: context");
            env->DeleteLocalRef(clazz);
            return;
        }

        jobject context = env->GetStaticObjectField(clazz, contextField);
        if (!context)
        {
            LOGE("Failed to get context");
            env->DeleteLocalRef(clazz);
            return;
        }

        jclass contextClass = env->GetObjectClass(context);
        jmethodID getExternalFilesDir =
            env->GetMethodID(contextClass, "getExternalFilesDir", "(Ljava/lang/String;)Ljava/io/File;");
        if (!getExternalFilesDir)
        {
            LOGE("Failed to get method: getExternalFilesDir");
            env->DeleteLocalRef(contextClass);
            env->DeleteLocalRef(context);
            env->DeleteLocalRef(clazz);
            return;
        }

        jstring type = env->NewStringUTF("");
        jobject file = env->CallObjectMethod(context, getExternalFilesDir, type);
        if (!file)
        {
            LOGE("Failed to get external files directory");
            env->DeleteLocalRef(type);
            env->DeleteLocalRef(contextClass);
            env->DeleteLocalRef(context);
            env->DeleteLocalRef(clazz);
            return;
        }

        jclass fileClass = env->GetObjectClass(file);
        jmethodID getAbsolutePath = env->GetMethodID(fileClass, "getAbsolutePath", "()Ljava/lang/String;");
        if (!getAbsolutePath)
        {
            LOGE("Failed to get method: getAbsolutePath");
            env->DeleteLocalRef(file);
            env->DeleteLocalRef(type);
            env->DeleteLocalRef(contextClass);
            env->DeleteLocalRef(context);
            env->DeleteLocalRef(clazz);
            return;
        }

        jstring path = (jstring)env->CallObjectMethod(file, getAbsolutePath);
        const char *str = env->GetStringUTFChars(path, nullptr);
        il2cpp_dump(str);
        env->ReleaseStringUTFChars(path, str);
        env->DeleteLocalRef(path);
        env->DeleteLocalRef(file);
        env->DeleteLocalRef(type);
        env->DeleteLocalRef(contextClass);
        env->DeleteLocalRef(context);
        env->DeleteLocalRef(clazz);
    }

    bool EnsureAttached()
    {
        auto curr = il2cpp_thread_current();
        if (!curr)
        {
            LOGI("Foreign thread!");
        }
        else
        {
            LOGI("Already Attached -> %llX", curr);
            return true;
        }
        LOGI("Attaching Thread");
        auto *thread = il2cpp_thread_attach(il2cpp_domain_get());
        while (!il2cpp_is_vm_thread(thread))
        {
            LOGI("Waiting...");
            sleep(1);
        }
        if (!thread)
        {
            LOGE("Attaching Failed");
            return false;
        }
        LOGI("Thread Attached");
        return true;
    }

    void Detach()
    {
        auto curr = il2cpp_thread_current();
        if (!curr)
        {
            LOGI("Foreign thread!");
            return;
        }
        LOGI("Detaching Thread");
        il2cpp_thread_detach(curr);
        LOGI("Thread Detached");
    }

    Il2CppDomain *GetDomain()
    {
        return il2cpp_domain_get();
    }

    Il2CppAssembly *GetAssembly(const char *name)
    {
        auto result = il2cpp_domain_assembly_open(il2cpp_domain_get(), name);
        if (!result && g_DoLog)
            LOGE("There's no assembly : %s", name);
        return result;
    }

    Il2CppImage *GetImage(Il2CppAssembly *assembly)
    {
        auto result = il2cpp_assembly_get_image(assembly);
        if (!result && g_DoLog)
            LOGE("GetImage return nullptr");
        return result;
    }

    Il2CppImage *GetCorlib()
    {
        return il2cpp_get_corlib();
    }

    Il2CppImage *GetImage(const char *assemblyName)
    {
        return GetImage(GetAssembly(assemblyName));
    }

    // Example.Game.Class.SubClass.ExtraSubClass
    // in case class name is "Example.Game.Class.SubClass", then subClass = 1
    Il2CppClass *GetClass(Il2CppImage *image, const char *name, int subClass)
    {
        std::string nameStr = name;
        size_t dotIndex = nameStr.find_last_of('.');
        std::string classNamespace = (dotIndex == std::string::npos) ? "" : nameStr.substr(0, dotIndex);
        bool isSubclass = subClass > 0;
        const std::string className = nameStr.substr(dotIndex + 1);
        if (isSubclass) // not fully tested
        {
            auto klass = GetClass(image, classNamespace.c_str(), subClass - 1);
            auto &[subKlass, len] = GetSubClasses(klass);
            for (int i = 0; i < len; ++i)
            {
                if (strcmp(subKlass[i]->getName(), className.c_str()) == 0)
                    return subKlass[i];
            }
            LOGE("There's no subclass : %s", name);
            return nullptr;
        }
        auto result = il2cpp_class_from_name(image, classNamespace.c_str(), className.c_str());
        if (!result)
        {
            // LOGD("Searching [%s] by iterating through [%s]...", name, image->getName());
            auto size = il2cpp_image_get_class_count(image);
            for (size_t i{0}; i < size; i++)
            {
                auto klass = il2cpp_image_get_class(image, i);
                if (klass->getFullName().compare(name) == 0)
                {
                    result = klass;
                    break;
                }
            }
            if (!result && g_DoLog)
                LOGE("There's no class : %s", name);
        }
        return result;
    }

    MethodInfo *GetClassMethod(Il2CppClass *klass, const char *methodName, int argsCount)
    {
        auto result = il2cpp_class_get_method_from_name(klass, methodName, argsCount);
        if (!result && g_DoLog)
            LOGE("There's no method : %s in %s", methodName, klass->getFullName().c_str());
        return result;
    }

    Il2CppImage *GetClassImage(Il2CppClass *klass)
    {
        return il2cpp_class_get_image(klass);
    }

    FieldInfo *GetClassField(Il2CppClass *klass, const char *fieldName)
    {
        auto result = il2cpp_class_get_field_from_name(klass, fieldName);
        if (!result && g_DoLog)
            LOGE("There's no field : %s", fieldName);
        return result;
    }

    void GetFieldValue(Il2CppObject *object, FieldInfo *field, void *outValue)
    {
        il2cpp_field_get_value(object, field, outValue);
    }

    void SetFieldValue(Il2CppObject *object, FieldInfo *field, void *newValue)
    {
        il2cpp_field_set_value(object, field, newValue);
    }

    FieldInfo *GetClassFields(Il2CppClass *klass, void **iter)
    {
        return il2cpp_class_get_fields(klass, iter);
    }

    void GetFieldStaticValue(FieldInfo *field, void *outValue)
    {
        il2cpp_field_static_get_value(field, outValue);
    }

    void SetFieldStaticValue(FieldInfo *field, void *outValue)
    {
        il2cpp_field_static_set_value(field, outValue);
    }

    Il2CppObject *GetFieldValueObect(Il2CppObject *object, FieldInfo *field)
    {
        return il2cpp_field_get_value_object(field, object);
    }

    MethodInfo *GetClassMethods(Il2CppClass *klass, void **iter)
    {
        return il2cpp_class_get_methods(klass, iter);
    }

    int32_t GetClassSize(Il2CppClass *klass)
    {
        return il2cpp_class_instance_size(klass);
    }

    uint32_t GetObjectSize(Il2CppObject *object)
    {
        return il2cpp_object_get_size(object);
    }
    Il2CppObject *NewObject(Il2CppClass *klass)
    {
        return il2cpp_object_new(klass);
    }

    uint32_t GetMethodParamCount(MethodInfo *method)
    {
        return il2cpp_method_get_param_count(method);
    }

    const char *GetMethodParamName(MethodInfo *method, uint32_t index)
    {
        return il2cpp_method_get_param_name(method, index);
    }

    std::vector<Il2CppClass *> GetClasses(Il2CppImage *image, const char *filter)
    {
        std::vector<Il2CppClass *> classes;
        auto size = il2cpp_image_get_class_count(image);
        for (size_t i{0}; i < size; i++)
        {
            auto klass = il2cpp_image_get_class(image, i);
            if (!filter || strstr(klass->getFullName().c_str(), filter))
                classes.push_back(klass);
        }
        return classes;
    }

    const char *GetMethodName(MethodInfo *method)
    {
        return il2cpp_method_get_name(method);
    }

    const char *GetClassName(Il2CppClass *klass)
    {
        return il2cpp_class_get_name(klass);
    }

    const char *GetClassNamespace(Il2CppClass *klass)
    {
        return il2cpp_class_get_namespace(klass);
    }

    uintptr_t GetFieldOffset(FieldInfo *field)
    {
        return il2cpp_field_get_offset(field);
    }

    void forEachClass(Il2CppClass *klass, void *classesPtr)
    {
        auto classes = *(std::vector<Il2CppClass *> *)classesPtr;
        classes.push_back(klass);
    }

    std::vector<Il2CppClass *> GetClasses()
    {
        std::vector<Il2CppClass *> classes;
        il2cpp_class_for_each(forEachClass, &classes);
        return classes;
    }

    std::unordered_map<Il2CppClass *, std::tuple<Il2CppClass **, size_t>> subClassesCache;
    const std::tuple<Il2CppClass **, size_t> &GetSubClasses(Il2CppClass *klass)
    {
        auto it = subClassesCache.find(klass);
        if (it != subClassesCache.end())
        {
            return it->second;
        }
        std::vector<Il2CppClass *> subClasses{};
        void *iter = nullptr;
        while (auto subKlass = il2cpp_class_get_nested_types(klass, &iter))
        {
            subClasses.push_back(subKlass);
        }
        subClassesCache.insert(std::make_pair(klass, std::make_tuple(subClasses.data(), subClasses.size())));
        return subClassesCache.at(klass);
    }

    Il2CppType *GetClassType(Il2CppClass *klass)
    {
        return il2cpp_class_get_type(klass);
    }

    bool GetClassIsGeneric(Il2CppClass *klass)
    {
        return il2cpp_class_is_generic(klass);
    }

    Il2CppClass *FindClass(const char *klassName)
    {
        PAUSE_LOG;
        auto &images = GetImages();
        for (auto image : images)
        {
            auto klass = image->getClass(klassName);
            if (klass)
                return klass;
        }
        return nullptr;
    }

    Il2CppClass *GetClassFromSystemType(Il2CppReflectionType *type)
    {
        return il2cpp_class_from_system_type(type);
    }

    Il2CppType *GetMethodReturnType(MethodInfo *method)
    {
        return il2cpp_method_get_return_type(method);
    }

    Il2CppType *GetMethodParam(MethodInfo *method, uint32_t index)
    {
        return il2cpp_method_get_param(method, index);
    }

    bool GetIsMethodGeneric(MethodInfo *method)
    {
        il2cpp_method_is_generic(method);
    }

    bool GetIsMethodInflated(MethodInfo *method)
    {
        return il2cpp_method_is_inflated(method);
    }

    Il2CppReflectionMethod *GetMethodObject(MethodInfo *method, Il2CppClass *refclass)
    {
        return il2cpp_method_get_object(method, refclass);
    }

    MethodInfo *GetMethodFromReflection(Il2CppReflectionMethod *method)
    {
        return il2cpp_method_get_from_reflection(method);
    }

    uint32_t GetMethodGenericCount(MethodInfo *method)
    {
        auto obj = method->getObject();
        auto args = obj->invoke_method<Il2CppArray<Il2CppObject *> *>("GetGenericArguments");
        return args->length();
    }

    MethodInfo *FindMethod(const char *klassName, const char *methodName, size_t argsCount)
    {
        PAUSE_LOG;
        auto &images = GetImages();
        for (auto image : images)
        {
            auto klass = image->getClass(klassName);
            if (klass)
            {
                return GetClassMethod(klass, methodName, argsCount);
            }
        }
        return nullptr;
    }

    Il2CppClass *GetMethodClass(MethodInfo *method)
    {
        return il2cpp_method_get_class(method);
    }

    Il2CppClass *GetClassFromType(Il2CppType *type)
    {
        return il2cpp_class_from_type(type);
    }

    Il2CppClass *GetTypeClass(Il2CppType *type)
    {
        return il2cpp_type_get_class_or_element_class(type);
    }

    bool GetTypeIsPointer(Il2CppType *type)
    {
        return il2cpp_type_is_pointer_type(type);
    }

    Il2CppType *GetFieldType(FieldInfo *field)
    {
        return il2cpp_field_get_type(field);
    }

    const char *GetFieldName(FieldInfo *field)
    {
        return il2cpp_field_get_name(field);
    }

    const char *GetTypeName(Il2CppType *type)
    {
        return il2cpp_type_get_name(type);
    }

    Il2CppObject *GetTypeObject(Il2CppType *type)
    {
        return il2cpp_type_get_object(type);
    }

    const char *GetChars(Il2CppString *str)
    {
        return reinterpret_cast<const char *>(il2cpp_string_chars(str));
    }

    Il2CppString *NewString(const char *str)
    {
        return il2cpp_string_new(str);
    }

    const char *GetImageName(Il2CppImage *image)
    {
        return il2cpp_image_get_name(image);
    }

    uint32_t GetArrayLength(_Il2CppArray *array)
    {
        return il2cpp_array_length(array);
    }

    _Il2CppArray *ArrayNew(Il2CppClass *elementTypeInfo, il2cpp_array_size_t length)
    {
        return il2cpp_array_new(elementTypeInfo, length);
    }

    Il2CppObject *GetBoxedValue(Il2CppClass *klass, void *value)
    {
        return il2cpp_value_box(klass, value);
    }

    std::tuple<Il2CppAssembly **, size_t> assembliesCache{nullptr, 0};
    const std::tuple<Il2CppAssembly **, size_t> &GetAssemblies()
    {
        const auto &[ass, size2] = assembliesCache;
        if (size2 > 0)
        {
            return assembliesCache;
        }
        size_t size = 0;
        auto asss = il2cpp_domain_get_assemblies(GetDomain(), &size);
        assembliesCache = std::make_tuple(asss, size);
        return assembliesCache;
    }

    std::vector<Il2CppImage *> imagesCache;
    const std::vector<Il2CppImage *> &GetImages()
    {
        if (!imagesCache.empty())
            return imagesCache;
        const auto &[ass, size] = GetAssemblies();
        for (size_t i = 0; i < size; i++)
        {
            imagesCache.push_back(GetImage(ass[i]));
        }
        return imagesCache;
    }

#if __DEBUG__
    struct Data
    {
        std::string name;
        std::chrono::time_point<std::chrono::system_clock> lastTime;
        int count;
        int maxCount;
    };
    std::unordered_map<void *, Data *> nameMaps;
    void Trace(Il2CppImage *image, std::initializer_list<char *> classesName,
               std::initializer_list<char *> ignoredMethods, bool nearBranchTrampoline, int maxSpam)
    {
        for (auto name : classesName)
        {
            auto classes = image->getClasses(name);
            if (classes.empty())
            {
                LOGD("No classes found for %s", name);
                continue;
            }
            for (auto klass : classes)
            {
                for (auto m : klass->getMethods())
                {
                    auto str = klass->getFullName() + "::" + m->getName();
                    bool skip = std::any_of(ignoredMethods.begin(), ignoredMethods.end(),
                                            [&](auto s) { return strstr(m->getName(), s) != nullptr; });
                    // for (auto ignored : ignoredMethods)
                    // {
                    //     if (strstr(m->getName(), ignored) != nullptr)
                    //     {
                    //         LOGD("Ignoring %s", str.c_str());
                    //         skip = true;
                    //         continue;
                    //     }
                    // }
                    if (skip)
                    {
                        LOGD("Ignoring %s", str.c_str());
                        continue;
                    }
                    // bool near = std::any_of(nearBranchTrampoline.begin(), nearBranchTrampoline.end(),
                    //                         [&](auto s) { return strstr(m->getName(), s) != nullptr; });
                    bool near = nearBranchTrampoline;
                    if (near)
                    {
                        LOGD("Tracing ( near ) %s", str.c_str());
                    }
                    else
                    {

                        LOGD("Tracing %s", str.c_str());
                    }
                    nameMaps.emplace(m->methodPointer, new Data{str, std::chrono::system_clock::now(), 0, maxSpam});

                    if (near)
                    {
                        dobby_enable_near_branch_trampoline();
                    }
                    auto result = DobbyInstrument(
                        m->methodPointer,
                        [](void *address, DobbyRegisterContext *ctx)
                        {
                            auto it = nameMaps.find(address);
                            if (it != nameMaps.end())
                            {
                                auto now = std::chrono::system_clock::now();
                                // if this function is called 5 times within 500ms, then remove address
                                if (std::chrono::duration_cast<std::chrono::milliseconds>(now - it->second->lastTime)
                                        .count() > 500)
                                {
                                    it->second->count = 0;
                                }
                                it->second->lastTime = now;
                                it->second->count++;
                                if (it->second->maxCount > 0 && it->second->count > it->second->maxCount)
                                {
                                    // LOGD("Removing %s", it->second->name.c_str());
                                    delete it->second;
                                    nameMaps.erase(it);
                                    return;
                                }
                                LOGD("%s", it->second->name.c_str());
                            }
                            else
                            {
                                // LOGD("%p NULL", address);
                            }
                        });
                    if (result != 0)
                    {
                        LOGE("Failed to instrument %s", str.c_str());
                    }
                    if (near)
                    {
                        dobby_disable_near_branch_trampoline();
                    }
                }
            }
            // DobbyInstrument(j, dobby_instrument_callback_t pre_handler)
        }
        LOGI("DONE");
    }
#endif
} // namespace Il2cpp
