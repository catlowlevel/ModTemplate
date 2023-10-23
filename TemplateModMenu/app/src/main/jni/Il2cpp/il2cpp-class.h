#pragma once

#include <cstdint>
#include <vector>
// #include "And64InlineHook/And64InlineHook.hpp"
#include "Dobby/include/dobby.h"
#include "Includes/Logger.h"

typedef uint16_t Il2CppChar;
typedef uintptr_t il2cpp_array_size_t;
typedef int32_t TypeDefinitionIndex;
typedef int32_t GenericParameterIndex;
typedef char Il2CppNativeChar;

typedef struct Il2CppMemoryCallbacks Il2CppMemoryCallbacks;
typedef struct Il2CppImage Il2CppImage;
typedef struct Il2CppClass Il2CppClass;
// typedef struct Il2CppArrayBounds Il2CppArrayBounds;
typedef struct Il2CppAssembly Il2CppAssembly;
typedef struct Il2CppArrayType Il2CppArrayType;
typedef struct Il2CppGenericClass Il2CppGenericClass;
typedef struct Il2CppReflectionType Il2CppReflectionType;
typedef struct MonitorData MonitorData;
typedef Il2CppClass Il2CppVTable;
typedef struct EventInfo EventInfo;
typedef struct FieldInfo FieldInfo;
typedef struct FieldInfo FieldInfo;
typedef struct PropertyInfo PropertyInfo;
typedef struct Il2CppDomain Il2CppDomain;
typedef struct Il2CppException Il2CppException;
typedef struct Il2CppObject Il2CppObject;
typedef struct Il2CppReflectionMethod Il2CppReflectionMethod;
typedef struct Il2CppString Il2CppString;
typedef struct Il2CppThread Il2CppThread;
typedef struct Il2CppStackFrameInfo Il2CppStackFrameInfo;
typedef struct Il2CppManagedMemorySnapshot Il2CppManagedMemorySnapshot;
typedef struct Il2CppDebuggerTransport Il2CppDebuggerTransport;
typedef struct Il2CppMethodDebugInfo Il2CppMethodDebugInfo;
typedef struct Il2CppCustomAttrInfo Il2CppCustomAttrInfo;
typedef const struct ___Il2CppMetadataTypeHandle *Il2CppMetadataTypeHandle;
typedef const struct ___Il2CppMetadataGenericParameterHandle *Il2CppMetadataGenericParameterHandle;

typedef void (*Il2CppMethodPointer)();

typedef void (*il2cpp_register_object_callback)(Il2CppObject **arr, int size, void *userdata);

typedef void *(*il2cpp_liveness_reallocate_callback)(void *ptr, size_t size, void *userdata);

typedef void (*Il2CppFrameWalkFunc)(const Il2CppStackFrameInfo *info, void *user_data);

typedef size_t (*Il2CppBacktraceFunc)(Il2CppMethodPointer *buffer, size_t maxSize);

typedef const Il2CppNativeChar *(*Il2CppSetFindPlugInCallback)(const Il2CppNativeChar *);

typedef void (*Il2CppLogCallback)(const char *);

typedef enum
{
    IL2CPP_UNHANDLED_POLICY_LEGACY,
    IL2CPP_UNHANDLED_POLICY_CURRENT
} Il2CppRuntimeUnhandledExceptionPolicy;

typedef enum
{
    IL2CPP_GC_MODE_DISABLED = 0,
    IL2CPP_GC_MODE_ENABLED = 1,
    IL2CPP_GC_MODE_MANUAL = 2
} Il2CppGCMode;

typedef enum Il2CppStat
{
    IL2CPP_STAT_NEW_OBJECT_COUNT,
    IL2CPP_STAT_INITIALIZED_CLASS_COUNT,
    IL2CPP_STAT_METHOD_COUNT,
    IL2CPP_STAT_CLASS_STATIC_DATA_SIZE,
    IL2CPP_STAT_GENERIC_INSTANCE_COUNT,
    IL2CPP_STAT_GENERIC_CLASS_COUNT,
    IL2CPP_STAT_INFLATED_METHOD_COUNT,
    IL2CPP_STAT_INFLATED_TYPE_COUNT,
} Il2CppStat;

typedef enum Il2CppTypeEnum
{
    IL2CPP_TYPE_END = 0x00,
    IL2CPP_TYPE_VOID = 0x01,
    IL2CPP_TYPE_BOOLEAN = 0x02,
    IL2CPP_TYPE_CHAR = 0x03,
    IL2CPP_TYPE_I1 = 0x04,
    IL2CPP_TYPE_U1 = 0x05,
    IL2CPP_TYPE_I2 = 0x06,
    IL2CPP_TYPE_U2 = 0x07,
    IL2CPP_TYPE_I4 = 0x08,
    IL2CPP_TYPE_U4 = 0x09,
    IL2CPP_TYPE_I8 = 0x0a,
    IL2CPP_TYPE_U8 = 0x0b,
    IL2CPP_TYPE_R4 = 0x0c,
    IL2CPP_TYPE_R8 = 0x0d,
    IL2CPP_TYPE_STRING = 0x0e,
    IL2CPP_TYPE_PTR = 0x0f,
    IL2CPP_TYPE_BYREF = 0x10,
    IL2CPP_TYPE_VALUETYPE = 0x11,
    IL2CPP_TYPE_CLASS = 0x12,
    IL2CPP_TYPE_VAR = 0x13,
    IL2CPP_TYPE_ARRAY = 0x14,
    IL2CPP_TYPE_GENERICINST = 0x15,
    IL2CPP_TYPE_TYPEDBYREF = 0x16,
    IL2CPP_TYPE_I = 0x18,
    IL2CPP_TYPE_U = 0x19,
    IL2CPP_TYPE_FNPTR = 0x1b,
    IL2CPP_TYPE_OBJECT = 0x1c,
    IL2CPP_TYPE_SZARRAY = 0x1d,
    IL2CPP_TYPE_MVAR = 0x1e,
    IL2CPP_TYPE_CMOD_REQD = 0x1f,
    IL2CPP_TYPE_CMOD_OPT = 0x20,
    IL2CPP_TYPE_INTERNAL = 0x21,
    IL2CPP_TYPE_MODIFIER = 0x40,
    IL2CPP_TYPE_SENTINEL = 0x41,
    IL2CPP_TYPE_PINNED = 0x45,
    IL2CPP_TYPE_ENUM = 0x55,
    IL2CPP_TYPE_IL2CPP_TYPE_INDEX = 0xff
} Il2CppTypeEnum;

// forward definitions
namespace Il2cpp
{
    //@formatter:off
    void GetFieldValue(Il2CppObject *object, FieldInfo *field, void *outValue);
    void GetFieldStaticValue(FieldInfo *field, void *outValue);
    void SetFieldValue(Il2CppObject *object, FieldInfo *field, void *newValue);
    void SetFieldStaticValue(FieldInfo *field, void *outValue);
    Il2CppObject *GetBoxedValue(Il2CppClass *klass, void *value);
    //@formatter:on
} // namespace Il2cpp

struct Il2CppType
{
    union
    {
        void *dummy;
        TypeDefinitionIndex klassIndex;
        const Il2CppType *type;
        Il2CppArrayType *array;
        GenericParameterIndex genericParameterIndex;
        Il2CppGenericClass *generic_class;
    } data;
    unsigned int attrs : 16;
    Il2CppTypeEnum type : 8;
    unsigned int num_mods : 6;
    unsigned int byref : 1;
    unsigned int pinned : 1;

    bool isPointer();

    const char *getName();

    Il2CppClass *getClass();
};

struct MethodInfo
{
    void *methodPointer; // Il2CppMethodPointer

    MethodInfo *inflate(std::initializer_list<Il2CppClass *> types);

    Il2CppObject *getObject();

    template <typename Func>
    void *replace(Func func);

    const char *getName();

    Il2CppType *getReturnType();

    std::vector<std::pair<const char *, Il2CppType *>> getParamsInfo(); // param name, param type

    template <typename T, typename... Args>
    T invoke_static(Args &&...args);

  private:
    template <typename T, typename... Args>
    T invoke(Il2CppObject *instance, Args &&...args);

    template <typename T, typename... Args>
    T invoke(Il2CppObject *instance);

    static bool _isAlreadyHooked(uintptr_t ptr);

    static void _addToHookedMap(uintptr_t ptr, uintptr_t oPtr);

    static uintptr_t _getHookedMap(uintptr_t ptr);

    friend struct Il2CppObject;
};

template <typename T>
struct ValueType
{
    T value;
    Il2CppObject *box(Il2CppImage *image, const char *klassName);
    Il2CppObject *box(Il2CppClass *klass);
};

struct Il2CppObject
{
    union
    {
        Il2CppClass *klass;
        Il2CppVTable *vtable;
    };
    MonitorData *monitor;

    template <typename T, typename... Args>
    T invoke_method(MethodInfo *method, Args &&...args);

    template <typename T>
    T invoke_method(MethodInfo *method);

    template <typename T>
    T invoke_method(const char *methodName);

    template <typename T, typename... Args>
    T invoke_method(const char *methodName, Args... args);

    template <typename T>
    T getField(const char *name);

    template <typename T>
    void setField(const char *name, T newValue);

  private:
    uintptr_t _getFieldOffset(const char *name);
};

struct _Il2CppArray : Il2CppObject
{
    void *bounds;                   // Il2CppArrayBounds
    il2cpp_array_size_t max_length; // maybe same as length

    uint32_t length();
};

template <typename T>
struct Il2CppArray : _Il2CppArray
{
    T data[32];
};

struct Il2CppAssembly
{
    Il2CppImage *getImage();
};

struct Il2CppImage
{
    Il2CppClass *getClass(const char *name, int subClass = 0);

    const char *getName();
};

struct Il2CppClass
{
    MethodInfo *getMethod(const char *name);

    MethodInfo *findMethod(const char *name, size_t idx = 0);

    FieldInfo *getField(const char *fieldName);

    std::vector<FieldInfo *> getFields();

    Il2CppImage *getImage();

    bool isGeneric();

    std::vector<MethodInfo *> getMethods(const char *filter = nullptr);

    const char *getName();

    const char *getNamespace();

    std::string getFullName();

    size_t getSize();

    Il2CppObject *New();
};

struct FieldInfo
{

    template <typename T>
    T getValue(Il2CppObject *instance)
    {
        T value;
        Il2cpp::GetFieldValue(instance, this, &value);
        return value;
    }

    template <typename T>
    T getStaticValue()
    {
        T value;
        Il2cpp::GetFieldStaticValue(this, &value);
        return value;
    }

    template <typename T>
    void setValue(Il2CppObject *instance, T value)
    {
        Il2cpp::SetFieldValue(instance, this, &value);
    }

    template <typename T>
    void setStaticValue(T value)
    {
        Il2cpp::SetFieldStaticValue(this, &value);
    }

    uintptr_t getOffset();

    Il2CppType *getType();

    const char *getName();
};

struct Il2CppString : Il2CppObject
{
    //    int32_t len;
    //    char chars[32];

    std::string to_string();

    const char *to_char();
};

template <typename T>
struct List : Il2CppObject
{
    using Iter = T;

    int size()
    {
        return this->invoke_method<int>("get_Count");
    }

    T get(int index)
    {
        return this->invoke_method<T>("get_Item", index);
    }

    Iter begin()
    {
        return get(0);
    }

    Iter end()
    {
        return get(size());
    }
};

//////////////////////////USER TYPES/////////////////////////////
struct BigDouble
{
    double numerator;
    int64_t exponent;
};

//////////////////////////TEMPLATE IMPL/////////////////////////////

template <typename T>
T Il2CppObject::invoke_method(MethodInfo *method)
{
    return method->invoke<T>(this);
}

template <typename T, typename... Args>
T Il2CppObject::invoke_method(const char *methodName, Args... args)
{
    MethodInfo *method = klass->getMethod(methodName);
    return invoke_method<T>(method, std::forward<Args>(args)...);
}

template <typename T>
T Il2CppObject::invoke_method(const char *methodName)
{
    return invoke_method<T>(klass->getMethod(methodName));
}

template <typename T, typename... Args>
T Il2CppObject::invoke_method(MethodInfo *method, Args &&...args)
{
    return method->invoke<T>(this, std::forward<Args>(args)...);
}

template <typename T>
void *MethodInfo::replace(T func)
{
    if (_isAlreadyHooked((uintptr_t)methodPointer))
    {
        LOGD("Already hooked");
        return nullptr;
    }
    void *orig;
    DobbyHook(methodPointer, (void *)func, &orig);
    _addToHookedMap((uintptr_t)methodPointer, (uintptr_t)orig);
    return orig;
}

template <typename T, typename... Args>
T MethodInfo::invoke_static(Args &&...args)
{
    using Invoker = T (*)(Args..., MethodInfo *);
    auto address = _getHookedMap((uintptr_t)this->methodPointer);
    auto invoker = reinterpret_cast<Invoker>(address);
    return invoker(std::forward<Args>(args)..., this);
}

template <typename T, typename... Args>
T MethodInfo::invoke(Il2CppObject *instance)
{
    using Invoker = T (*)(Il2CppObject *, MethodInfo *);
    auto address = _getHookedMap((uintptr_t)this->methodPointer);
    auto invoker = reinterpret_cast<Invoker>(address);
    return invoker(instance, this);
}

template <typename T, typename... Args>
T MethodInfo::invoke(Il2CppObject *instance, Args &&...args)
{
    using Invoker = T (*)(Il2CppObject *, Args..., MethodInfo *);
    auto address = _getHookedMap((uintptr_t)this->methodPointer);
    auto invoker = reinterpret_cast<Invoker>(address);
    return invoker(instance, std::forward<Args>(args)..., this);
}

template <typename T>
T Il2CppObject::getField(const char *name)
{
    auto field = klass->getField(name);
    return field->getValue<T>(this);
    //        auto offset = _getFieldOffset(name);
    //        return (T) _getField(name);
    //        return (T) (this + offset);
}

template <typename T>
void Il2CppObject::setField(const char *name, T newValue)
{
    auto field = klass->getField(name);
    return field->setValue<T>(this, newValue);
}

template <typename T>
Il2CppObject *ValueType<T>::box(Il2CppImage *image, const char *klassName)
{
    auto klass = image->getClass(klassName);
    return box(klass);
}
template <typename T>
Il2CppObject *ValueType<T>::box(Il2CppClass *klass)
{
    return Il2cpp::GetBoxedValue(klass, this);
}
