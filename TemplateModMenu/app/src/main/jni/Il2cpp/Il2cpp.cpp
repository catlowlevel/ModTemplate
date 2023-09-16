//
// Created by Perfare on 2020/7/4.
//

#include "Il2cpp.h"
#include <dlfcn.h>
#include <cstdlib>
#include <cstring>
#include <cinttypes>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <unistd.h>
#include <unordered_map>
#include <thread>
#include "log.h"
#include "il2cpp-tabledefs.h"
#include "il2cpp-class.h"

#include "xdl/include/xdl.h"

#define DO_API(r, n, p) r (*n) p

#include "il2cpp-api-functions.h"

#undef DO_API

static uint64_t il2cpp_base = 0;

void init_il2cpp_api(void *handle) {
#define DO_API(r, n, p) {                      \
    n = (r (*) p)xdl_sym(handle, #n, nullptr); \
}

#include "il2cpp-api-functions.h"

#undef DO_API
}

std::string get_method_modifier(uint32_t flags) {
    std::stringstream outPut;
    auto access = flags & METHOD_ATTRIBUTE_MEMBER_ACCESS_MASK;
    switch (access) {
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
    if (flags & METHOD_ATTRIBUTE_STATIC) {
        outPut << "static ";
    }
    if (flags & METHOD_ATTRIBUTE_ABSTRACT) {
        outPut << "abstract ";
        if ((flags & METHOD_ATTRIBUTE_VTABLE_LAYOUT_MASK) == METHOD_ATTRIBUTE_REUSE_SLOT) {
            outPut << "override ";
        }
    } else if (flags & METHOD_ATTRIBUTE_FINAL) {
        if ((flags & METHOD_ATTRIBUTE_VTABLE_LAYOUT_MASK) == METHOD_ATTRIBUTE_REUSE_SLOT) {
            outPut << "sealed override ";
        }
    } else if (flags & METHOD_ATTRIBUTE_VIRTUAL) {
        if ((flags & METHOD_ATTRIBUTE_VTABLE_LAYOUT_MASK) == METHOD_ATTRIBUTE_NEW_SLOT) {
            outPut << "virtual ";
        } else {
            outPut << "override ";
        }
    }
    if (flags & METHOD_ATTRIBUTE_PINVOKE_IMPL) {
        outPut << "extern ";
    }
    return outPut.str();
}

bool _il2cpp_type_is_byref(Il2CppType *type) {
    auto byref = type->byref;
    if (il2cpp_type_is_byref) {
        byref = il2cpp_type_is_byref(type);
    }
    return byref;
}

std::string dump_method(Il2CppClass *klass) {
    std::stringstream outPut;
    outPut << "\n\t// Methods\n";
    void *iter = nullptr;
    while (auto method = il2cpp_class_get_methods(klass, &iter)) {
        //TODO attribute
        if (method->methodPointer) {
            outPut << "\t// RVA: 0x";
            outPut << std::hex << (uint64_t) method->methodPointer - il2cpp_base;
            outPut << " VA: 0x";
            outPut << std::hex << (uint64_t) method->methodPointer;
        } else {
            outPut << "\t// RVA: 0x VA: 0x0";
        }
        /*if (method->slot != 65535) {
            outPut << " Slot: " << std::dec << method->slot;
        }*/
        outPut << "\n\t";
        uint32_t iflags = 0;
        auto flags = il2cpp_method_get_flags(method, &iflags);
        outPut << get_method_modifier(flags);
        //TODO genericContainerIndex
        auto return_type = il2cpp_method_get_return_type(method);
        if (_il2cpp_type_is_byref(return_type)) {
            outPut << "ref ";
        }
        auto return_class = il2cpp_class_from_type(return_type);
        outPut << il2cpp_class_get_name(return_class) << " " << il2cpp_method_get_name(method)
               << "(";
        auto param_count = il2cpp_method_get_param_count(method);
        for (int i = 0; i < param_count; ++i) {
            auto param = il2cpp_method_get_param(method, i);
            auto attrs = param->attrs;
            if (_il2cpp_type_is_byref(param)) {
                if (attrs & PARAM_ATTRIBUTE_OUT && !(attrs & PARAM_ATTRIBUTE_IN)) {
                    outPut << "out ";
                } else if (attrs & PARAM_ATTRIBUTE_IN && !(attrs & PARAM_ATTRIBUTE_OUT)) {
                    outPut << "in ";
                } else {
                    outPut << "ref ";
                }
            } else {
                if (attrs & PARAM_ATTRIBUTE_IN) {
                    outPut << "[In] ";
                }
                if (attrs & PARAM_ATTRIBUTE_OUT) {
                    outPut << "[Out] ";
                }
            }
            auto parameter_class = il2cpp_class_from_type(param);
            outPut << il2cpp_class_get_name(parameter_class) << " "
                   << il2cpp_method_get_param_name(method, i);
            outPut << ", ";
        }
        if (param_count > 0) {
            outPut.seekp(-2, outPut.cur);
        }
        outPut << ") { }\n";
        //TODO GenericInstMethod
    }
    return outPut.str();
}

std::string dump_property(Il2CppClass *klass) {
    std::stringstream outPut;
    outPut << "\n\t// Properties\n";
    void *iter = nullptr;
    while (auto prop_const = il2cpp_class_get_properties(klass, &iter)) {
        //TODO attribute
        auto prop = const_cast<PropertyInfo *>(prop_const);
        auto get = il2cpp_property_get_get_method(prop);
        auto set = il2cpp_property_get_set_method(prop);
        auto prop_name = il2cpp_property_get_name(prop);
        outPut << "\t";
        Il2CppClass *prop_class = nullptr;
        uint32_t iflags = 0;
        if (get) {
            outPut << get_method_modifier(il2cpp_method_get_flags(get, &iflags));
            prop_class = il2cpp_class_from_type(il2cpp_method_get_return_type(get));
        } else if (set) {
            outPut << get_method_modifier(il2cpp_method_get_flags(set, &iflags));
            auto param = il2cpp_method_get_param(set, 0);
            prop_class = il2cpp_class_from_type(param);
        }
        if (prop_class) {
            outPut << il2cpp_class_get_name(prop_class) << " " << prop_name << " { ";
            if (get) {
                outPut << "get; ";
            }
            if (set) {
                outPut << "set; ";
            }
            outPut << "}\n";
        } else {
            if (prop_name) {
                outPut << " // unknown property " << prop_name;
            }
        }
    }
    return outPut.str();
}

std::string dump_field(Il2CppClass *klass) {
    std::stringstream outPut;
    outPut << "\n\t// Fields\n";
    auto is_enum = il2cpp_class_is_enum(klass);
    void *iter = nullptr;
    while (auto field = il2cpp_class_get_fields(klass, &iter)) {
        //TODO attribute
        outPut << "\t";
        auto attrs = il2cpp_field_get_flags(field);
        auto access = attrs & FIELD_ATTRIBUTE_FIELD_ACCESS_MASK;
        switch (access) {
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
        if (attrs & FIELD_ATTRIBUTE_LITERAL) {
            outPut << "const ";
        } else {
            if (attrs & FIELD_ATTRIBUTE_STATIC) {
                outPut << "static ";
            }
            if (attrs & FIELD_ATTRIBUTE_INIT_ONLY) {
                outPut << "readonly ";
            }
        }
        auto field_type = il2cpp_field_get_type(field);
        auto field_class = il2cpp_class_from_type(field_type);
        outPut << il2cpp_class_get_name(field_class) << " " << il2cpp_field_get_name(field);
        //TODO 获取构造函数初始化后的字段值
        if (attrs & FIELD_ATTRIBUTE_LITERAL && is_enum) {
            uint64_t val = 0;
            il2cpp_field_static_get_value(field, &val);
            outPut << " = " << std::dec << val;
        }
        outPut << "; // 0x" << std::hex << il2cpp_field_get_offset(field) << "\n";
    }
    return outPut.str();
}

std::string dump_type(Il2CppType *type) {
    std::stringstream outPut;
    auto *klass = il2cpp_class_from_type(type);
    outPut << "\n// Namespace: " << il2cpp_class_get_namespace(klass) << "\n";
    auto flags = il2cpp_class_get_flags(klass);
    if (flags & TYPE_ATTRIBUTE_SERIALIZABLE) {
        outPut << "[Serializable]\n";
    }
    //TODO attribute
    auto is_valuetype = il2cpp_class_is_valuetype(klass);
    auto is_enum = il2cpp_class_is_enum(klass);
    auto visibility = flags & TYPE_ATTRIBUTE_VISIBILITY_MASK;
    switch (visibility) {
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
    if (flags & TYPE_ATTRIBUTE_ABSTRACT && flags & TYPE_ATTRIBUTE_SEALED) {
        outPut << "static ";
    } else if (!(flags & TYPE_ATTRIBUTE_INTERFACE) && flags & TYPE_ATTRIBUTE_ABSTRACT) {
        outPut << "abstract ";
    } else if (!is_valuetype && !is_enum && flags & TYPE_ATTRIBUTE_SEALED) {
        outPut << "sealed ";
    }
    if (flags & TYPE_ATTRIBUTE_INTERFACE) {
        outPut << "interface ";
    } else if (is_enum) {
        outPut << "enum ";
    } else if (is_valuetype) {
        outPut << "struct ";
    } else {
        outPut << "class ";
    }
    outPut << il2cpp_class_get_name(klass); //TODO genericContainerIndex
    std::vector<std::string> extends;
    auto parent = il2cpp_class_get_parent(klass);
    if (!is_valuetype && !is_enum && parent) {
        auto parent_type = il2cpp_class_get_type(parent);
        if (parent_type->type != IL2CPP_TYPE_OBJECT) {
            extends.emplace_back(il2cpp_class_get_name(parent));
        }
    }
    void *iter = nullptr;
    while (auto itf = il2cpp_class_get_interfaces(klass, &iter)) {
        extends.emplace_back(il2cpp_class_get_name(itf));
    }
    if (!extends.empty()) {
        outPut << " : " << extends[0];
        for (int i = 1; i < extends.size(); ++i) {
            outPut << ", " << extends[i];
        }
    }
    outPut << "\n{";
    outPut << dump_field(klass);
    outPut << dump_property(klass);
    outPut << dump_method(klass);
    //TODO EventInfo
    outPut << "}\n";
    return outPut.str();
}

void il2cpp_api_init(void *handle) {
    LOGI("il2cpp_handle: %p", handle);
    init_il2cpp_api(handle);
    if (il2cpp_domain_get_assemblies) {
        Dl_info dlInfo;
        if (dladdr((void *) il2cpp_domain_get_assemblies, &dlInfo)) {
            il2cpp_base = reinterpret_cast<uint64_t>(dlInfo.dli_fbase);
        }
        LOGI("il2cpp_base: %" PRIx64"", il2cpp_base);
    } else {
        LOGE("Failed to initialize il2cpp api.");
        return;
    }
    while (!il2cpp_is_vm_thread(nullptr)) {
        LOGI("Waiting for il2cpp_init...");
        sleep(1);
    }
//    auto domain = il2cpp_domain_get();
//    il2cpp_thread_attach(domain);
}

//void il2cpp_dump(const char *outDir) {
//    LOGI("dumping...");
//    size_t size;
//    auto domain = il2cpp_domain_get();
//    auto assemblies = il2cpp_domain_get_assemblies(domain, &size);
//    std::stringstream imageOutput;
//    for (int i = 0; i < size; ++i) {
//        auto image = il2cpp_assembly_get_image(assemblies[i]);
//        imageOutput << "// Image " << i << ": " << il2cpp_image_get_name(image) << "\n";
//    }
//    std::vector<std::string> outPuts;
//    if (il2cpp_image_get_class) {
//        LOGI("Version greater than 2018.3");
//        //使用il2cpp_image_get_class
//        for (int i = 0; i < size; ++i) {
//            auto image = il2cpp_assembly_get_image(assemblies[i]);
//            std::stringstream imageStr;
//            imageStr << "\n// Dll : " << il2cpp_image_get_name(image);
//            auto classCount = il2cpp_image_get_class_count(image);
//            for (int j = 0; j < classCount; ++j) {
//                auto klass = il2cpp_image_get_class(image, j);
//                auto type = il2cpp_class_get_type(const_cast<Il2CppClass *>(klass));
//                //LOGD("type name : %s", il2cpp_type_get_name(type));
//                auto outPut = imageStr.str() + dump_type(type);
//                outPuts.push_back(outPut);
//            }
//        }
//    } else {
//        LOGI("Version less than 2018.3");
//        //使用反射
//        auto corlib = il2cpp_get_corlib();
//        auto assemblyClass = il2cpp_class_from_name(corlib, "System.Reflection", "Assembly");
//        auto assemblyLoad = il2cpp_class_get_method_from_name(assemblyClass, "Load", 1);
//        auto assemblyGetTypes = il2cpp_class_get_method_from_name(assemblyClass, "GetTypes", 0);
//        if (assemblyLoad && assemblyLoad->methodPointer) {
//            LOGI("Assembly::Load: %p", assemblyLoad->methodPointer);
//        } else {
//            LOGI("miss Assembly::Load");
//            return;
//        }
//        if (assemblyGetTypes && assemblyGetTypes->methodPointer) {
//            LOGI("Assembly::GetTypes: %p", assemblyGetTypes->methodPointer);
//        } else {
//            LOGI("miss Assembly::GetTypes");
//            return;
//        }
//        typedef void *(*Assembly_Load_ftn)(void *, Il2CppString *, void *);
//        typedef _Il2CppArray *(*Assembly_GetTypes_ftn)(void *, void *);
//        for (int i = 0; i < size; ++i) {
//            auto image = il2cpp_assembly_get_image(assemblies[i]);
//            std::stringstream imageStr;
//            auto image_name = il2cpp_image_get_name(image);
//            imageStr << "\n// Dll : " << image_name;
//            //LOGD("image name : %s", image->name);
//            auto imageName = std::string(image_name);
//            auto pos = imageName.rfind('.');
//            auto imageNameNoExt = imageName.substr(0, pos);
//            auto assemblyFileName = il2cpp_string_new(imageNameNoExt.data());
//            auto reflectionAssembly = ((Assembly_Load_ftn) assemblyLoad->methodPointer)(nullptr,
//                                                                                        assemblyFileName,
//                                                                                        nullptr);
//            auto reflectionTypes = ((Assembly_GetTypes_ftn) assemblyGetTypes->methodPointer)(
//                    reflectionAssembly, nullptr);
//            auto items = reflectionTypes->vector;
//            for (int j = 0; j < reflectionTypes->max_length; ++j) {
//                auto klass = il2cpp_class_from_system_type((Il2CppReflectionType *) items[j]);
//                auto type = il2cpp_class_get_type(klass);
//                //LOGD("type name : %s", il2cpp_type_get_name(type));
//                auto outPut = imageStr.str() + dump_type(type);
//                outPuts.push_back(outPut);
//            }
//        }
//    }
//    LOGI("write dump file");
//    auto outPath = std::string(outDir).append("/files/dump.cs");
//    std::ofstream outStream(outPath);
//    outStream << imageOutput.str();
//    auto count = outPuts.size();
//    for (int i = 0; i < count; ++i) {
//        outStream << outPuts[i];
//    }
//    outStream.close();
//    LOGI("dump done!");
//}


namespace Il2cpp {
    void Init() {
        auto handle = xdl_open("libil2cpp.so", 0);
        il2cpp_api_init(handle);
        xdl_close(handle);
    }

    bool EnsureAttached() {
        auto curr = il2cpp_thread_current();
        if (!curr) {
            LOGI("Foreign thread!");
        } else {
            LOGI("Already Attached -> %llX", curr);
            return true;
        }
        LOGI("Attaching Thread");
        void *thread = il2cpp_thread_attach(il2cpp_domain_get());
        if (!thread) {
            LOGE("Attaching Failed");
            return false;
        }
        return true;
    }

    Il2CppDomain *GetDomain() {
        return il2cpp_domain_get();
    }

    Il2CppAssembly *GetAssembly(const char *name) {
        auto result = il2cpp_domain_assembly_open(il2cpp_domain_get(), name);
        if (!result)
            LOGE("There's no assembly : %s", name);
        return result;
    }

    Il2CppImage *GetImage(Il2CppAssembly *assembly) {
        auto result = il2cpp_assembly_get_image(assembly);
        if (!result) LOGE("GetImage return nullptr");
        return result;
    }

    Il2CppImage *GetImage(const char *assemblyName) {
        return GetImage(GetAssembly(assemblyName));
    }

    Il2CppClass *GetClass(Il2CppImage *image, const char *name) {
        std::string nameStr = name;
        const size_t dotIndex = nameStr.find_last_of('.');
        const std::string classNamespace = (dotIndex == std::string::npos) ? "" : nameStr.substr(0,
                                                                                                 dotIndex);
        const std::string className = nameStr.substr(dotIndex + 1);
        auto result = il2cpp_class_from_name(image, classNamespace.c_str(), className.c_str());
        if (!result) LOGE("There's no class : %s", name);
        return result;
    }


    MethodInfo *GetClassMethod(Il2CppClass *klass, const char *methodName, int argsCount) {
        auto result = il2cpp_class_get_method_from_name(klass, methodName, argsCount);
        if (!result) LOGE("There's no method : %s", methodName);
        return result;
    }

    Il2CppImage *GetClassImage(Il2CppClass *klass) {
        return il2cpp_class_get_image(klass);
    }

    FieldInfo *GetClassField(Il2CppClass *klass, const char *fieldName) {
        auto result = il2cpp_class_get_field_from_name(klass, fieldName);
        if (!result) LOGE("There's no field : %s", fieldName);
        return result;
    }

    void GetFieldValue(Il2CppObject *object, FieldInfo *field, void *outValue) {
        il2cpp_field_get_value(object, field, outValue);
    }

    void SetFieldValue(Il2CppObject *object, FieldInfo *field, void *newValue) {
        il2cpp_field_set_value(object, field, newValue);
    }

    FieldInfo *GetClassFields(Il2CppClass *klass, void **iter) {
        return il2cpp_class_get_fields(klass, iter);
    }

    void GetFieldStaticValue(FieldInfo *field, void *outValue) {
        il2cpp_field_static_get_value(field, outValue);
    }

    void SetFieldStaticValue(FieldInfo *field, void *outValue) {
        il2cpp_field_static_set_value(field, outValue);
    }

    Il2CppObject *GetFieldValueObect(Il2CppObject *object, FieldInfo *field) {
        return il2cpp_field_get_value_object(field, object);
    }

    MethodInfo *GetClassMethods(Il2CppClass *klass, void **iter) {
        return il2cpp_class_get_methods(klass, iter);
    }

    int32_t GetClassSize(Il2CppClass *klass) {
        return il2cpp_class_instance_size(klass);
    }

    uint32_t GetObjectSize(Il2CppObject *object) {
        return il2cpp_object_get_size(object);
    }

    uint32_t GetMethodParamCount(MethodInfo *method) {
        return il2cpp_method_get_param_count(method);
    }

    const char *GetMethodParamName(MethodInfo *method, uint32_t index) {
        return il2cpp_method_get_param_name(method, index);
    }

    const char *GetMethodName(MethodInfo *method) {
        return il2cpp_method_get_name(method);
    }

    const char *GetClassName(Il2CppClass *klass) {
        return il2cpp_class_get_name(klass);
    }

    const char *GetClassNamespace(Il2CppClass *klass) {
        return il2cpp_class_get_namespace(klass);
    }

    uintptr_t GetFieldOffset(FieldInfo *field) {
        return il2cpp_field_get_offset(field);
    }

    void forEachClass(Il2CppClass *klass, void *classesPtr) {
        auto classes = *(std::vector<Il2CppClass *> *) classesPtr;
        classes.push_back(klass);
    }

    std::vector<Il2CppClass *> GetClasses() {
        std::vector<Il2CppClass *> classes;
        il2cpp_class_for_each(forEachClass, &classes);
        return classes;
    }

    Il2CppType *GetMethodReturnType(MethodInfo *method) {
        return il2cpp_method_get_return_type(method);
    }

    Il2CppType *GetMethodParam(MethodInfo *method, uint32_t index) {
        return il2cpp_method_get_param(method, index);
    }

    Il2CppClass *GetClassFromType(Il2CppType *type) {
        return il2cpp_class_from_type(type);
    }

    bool GetTypeIsPointer(Il2CppType *type) {
        return il2cpp_type_is_pointer_type(type);
    }

    Il2CppType *GetFieldType(FieldInfo *field) {
        return il2cpp_field_get_type(field);
    }

    const char *GetFieldName(FieldInfo *field) {
        return il2cpp_field_get_name(field);
    }

    const char *GetTypeName(Il2CppType *type) {
        return il2cpp_type_get_name(type);
    }

    const char *GetChars(Il2CppString *str) {
        return reinterpret_cast<const char *>(il2cpp_string_chars(str));
    }

    const char *GetImageName(Il2CppImage *image) {
        return il2cpp_image_get_name(image);
    }

    uint32_t GetArrayLength(_Il2CppArray *array) {
        return il2cpp_array_length(array);
    }
}