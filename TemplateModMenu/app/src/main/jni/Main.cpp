#include <list>
#include <vector>
#include <cstring>
#include <pthread.h>
#include <thread>
#include <cstring>
#include <jni.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <dlfcn.h>
#include "Includes/Logger.h"
#include "Includes/obfuscate.h"
#include "Includes/Utils.h"
#include "KittyMemory/MemoryPatch.h"
#include "Menu/Setup.h"
#include "Il2cpp/Il2cpp.h"

// Target lib here
#define targetLibName OBFUSCATE("libil2cpp.so")

Il2CppImage *g_Image = nullptr;

#define REPLACE_NAME_METHOD(methodInfo, name, method)                                                                  \
    [&]                                                                                                                \
    {                                                                                                                  \
        void *old = methodInfo->methodPointer;                                                                         \
        void *n = methodInfo->replace(method);                                                                         \
        LOGD(OBFUSCATE("%s::%s (%p -> %p) HOOKED"), methodInfo->getClass()->getFullName().c_str(),                     \
             methodInfo->getName(), old, n);                                                                           \
        return n;                                                                                                      \
    }();
#define REPLACE_NAME_METHOD_ORIG(methodInfo, method, orig)                                                             \
    orig = (decltype(orig))REPLACE_NAME_METHOD(methodInfo, method)
#define REPLACE_NAME_KLASS(klass, name, method)                                                                        \
    REPLACE_NAME_METHOD(klass->getMethod(OBFUSCATE(name)), (const char *)OBFUSCATE(name), method)
#define REPLACE_NAME_KLASS_ORIG(klass, name, method, orig)                                                             \
    REPLACE_NAME_METHOD_ORIG(klass->getMethod(OBFUSCATE(name)), (const char *)OBFUSCATE(name), method, orig)
#define REPLACE_KLASS(klass, method) REPLACE_NAME_KLASS(klass, #method, method)
#define REPLACE_NAME(className, name, method)                                                                          \
    REPLACE_NAME_KLASS(g_Image->getClass((const char *)OBFUSCATE(className)), name, method)
#define REPLACE_NAME_ORIG(className, name, method, orig)                                                               \
    REPLACE_NAME_KLASS_ORIG(g_Image->getClass(OBFUSCATE(className)), name, method, orig)
#define REPLACE(className, method) REPLACE_NAME(className, #method, method)
#define REPLACE_ORIG(className, method, orig) REPLACE_NAME_ORIG(className, #method, method, orig)

void SampleHook(Il2CppObject *instance, int arg)
{
    instance->setField("field", 1000);
    return instance->invoke_method<void>("Sample", arg);
}

// we will run our hacks in a new thread so our while loop doesn't block process main thread
void *hack_thread(void *)
{
    LOGI(OBFUSCATE("pthread created"));

    // Check if target lib is loaded
    do
    {
        sleep(1);
    } while (!isLibraryLoaded(targetLibName));

    LOGI(OBFUSCATE("%s has been loaded"), (const char *)targetLibName);

    Il2cpp::Init();
    Il2cpp::EnsureAttached();

    LOGD(OBFUSCATE("HOOKING..."));
    //    g_Image = Il2cpp::GetAssembly("Game.Domain")->getImage();
    //
    //    REPLACE_NAME("Samle.Class", "Sample", SampleHook);

    LOGD(OBFUSCATE("HOOKED!"));

    return nullptr;
}

jobjectArray GetFeatureList(JNIEnv *env, [[maybe_unused]] jobject context)
{
    jobjectArray ret;

    const char *features[] = {
        OBFUSCATE("Category_The Category"), // Not counted
        OBFUSCATE("Toggle_The toggle"),
        OBFUSCATE("100_Toggle_True_The toggle 2"), // This one have feature number assigned, and switched on by default
        OBFUSCATE("110_Toggle_The toggle 3"),      // This one too
        OBFUSCATE("SeekBar_The slider_1_100"), OBFUSCATE("SeekBar_Kittymemory slider example_1_5"),
        OBFUSCATE("Spinner_The spinner_Items 1,Items 2,Items 3"), OBFUSCATE("Button_The button"),
        OBFUSCATE("ButtonLink_The button with link_https://www.youtube.com/"), // Not counted
        OBFUSCATE("ButtonOnOff_The On/Off button"), OBFUSCATE("CheckBox_The Check Box"),
        OBFUSCATE("CheckBox_The Check Box2"), OBFUSCATE("InputValue_Input number"),
        OBFUSCATE("InputValue_1000_Input number 2"), // Max value
        OBFUSCATE("InputText_Input text"), OBFUSCATE("RadioButton_Radio buttons_OFF,Mod 1,Mod 2,Mod 3"),

        // Create new collapse
        OBFUSCATE("Collapse_Collapse 1"), OBFUSCATE("CollapseAdd_Toggle_The toggle"),
        OBFUSCATE("CollapseAdd_Toggle_The toggle"), OBFUSCATE("123_CollapseAdd_Toggle_The toggle"),
        OBFUSCATE("122_CollapseAdd_CheckBox_Check box"), OBFUSCATE("CollapseAdd_Button_The button"),

        // Create new collapse again
        OBFUSCATE("Collapse_Collapse 2_True"), OBFUSCATE("CollapseAdd_SeekBar_The slider_1_100"),
        OBFUSCATE("CollapseAdd_InputValue_Input number"),

        OBFUSCATE("RichTextView_This is text view, not fully HTML."
                  "<b>Bold</b> <i>italic</i> <u>underline</u>"
                  "<br />New line <font color='red'>Support colors</font>"
                  "<br/><big>bigger Text</big>"),
        OBFUSCATE("RichWebView_<html><head><style>body{color: white;}</style></head><body>"
                  "This is WebView, with REAL HTML support!"
                  "<div style=\"background-color: darkblue; text-align: center;\">Support CSS</div>"
                  "<marquee style=\"color: green; font-weight:bold;\" direction=\"left\" scrollamount=\"5\" "
                  "behavior=\"scroll\">This is <u>scrollable</u> text</marquee>"
                  "</body></html>")};

    // Now you dont have to manually update the number everytime;
    int Total_Feature = (sizeof features / sizeof features[0]);
    ret = (jobjectArray)env->NewObjectArray(Total_Feature, env->FindClass(OBFUSCATE("java/lang/String")),
                                            env->NewStringUTF(""));

    for (int i = 0; i < Total_Feature; i++)
        env->SetObjectArrayElement(ret, i, env->NewStringUTF(features[i]));

    return (ret);
}

void Changes(JNIEnv *env, [[maybe_unused]] jclass clazz, [[maybe_unused]] jobject obj, jint featNum, jstring featName,
             jint value, jboolean boolean, jstring str)
{

    LOGD(OBFUSCATE("Feature name: %d - %s | Value: = %d | Bool: = %d | Text: = %s"), featNum,
         env->GetStringUTFChars(featName, nullptr), value, boolean,
         str != nullptr ? env->GetStringUTFChars(str, nullptr) : "");

    // BE CAREFUL NOT TO ACCIDENTALLY REMOVE break;

    switch (featNum)
    {
        case 0:
        {
            //            g_IncreaseMoney = boolean;
            break;
        }
        case 1:
        {
            //            g_AlwaysEnough = boolean;
            break;
        }
        case 2:
        {
            break;
        }
        case 3:
        {
            break;
        }
    }
}

__attribute__((constructor)) void lib_main()
{
    // Create a new thread so it does not block the main thread, means the game would not freeze
    pthread_t ptid;
    pthread_create(&ptid, nullptr, hack_thread, nullptr);
}

int RegisterMenu(JNIEnv *env)
{
    JNINativeMethod methods[] = {
        {OBFUSCATE("Icon"), OBFUSCATE("()Ljava/lang/String;"), reinterpret_cast<void *>(Icon)},
        {OBFUSCATE("IconWebViewData"), OBFUSCATE("()Ljava/lang/String;"), reinterpret_cast<void *>(IconWebViewData)},
        {OBFUSCATE("IsGameLibLoaded"), OBFUSCATE("()Z"), reinterpret_cast<void *>(isGameLibLoaded)},
        {OBFUSCATE("Init"), OBFUSCATE("(Landroid/content/Context;Landroid/widget/TextView;Landroid/widget/TextView;)V"),
         reinterpret_cast<void *>(Init)},
        {OBFUSCATE("SettingsList"), OBFUSCATE("()[Ljava/lang/String;"), reinterpret_cast<void *>(SettingsList)},
        {OBFUSCATE("GetFeatureList"), OBFUSCATE("()[Ljava/lang/String;"), reinterpret_cast<void *>(GetFeatureList)},
    };

    jclass clazz = env->FindClass(OBFUSCATE("com/android/support/Menu"));
    if (!clazz)
        return JNI_ERR;
    if (env->RegisterNatives(clazz, methods, sizeof(methods) / sizeof(methods[0])) != 0)
        return JNI_ERR;
    return JNI_OK;
}

int RegisterPreferences(JNIEnv *env)
{
    JNINativeMethod methods[] = {
        {OBFUSCATE("Changes"), OBFUSCATE("(Landroid/content/Context;ILjava/lang/String;IZLjava/lang/String;)V"),
         reinterpret_cast<void *>(Changes)},
    };
    jclass clazz = env->FindClass(OBFUSCATE("com/android/support/Preferences"));
    if (!clazz)
        return JNI_ERR;
    if (env->RegisterNatives(clazz, methods, sizeof(methods) / sizeof(methods[0])) != 0)
        return JNI_ERR;
    return JNI_OK;
}

int RegisterMain(JNIEnv *env)
{
    JNINativeMethod methods[] = {
        {OBFUSCATE("CheckOverlayPermission"), OBFUSCATE("(Landroid/content/Context;)V"),
         reinterpret_cast<void *>(CheckOverlayPermission)},
    };

    jclass clazz = env->FindClass(OBFUSCATE("com/android/support/Main"));
    if (!clazz)
        return JNI_ERR;
    if (env->RegisterNatives(clazz, methods, sizeof(methods) / sizeof(methods[0])) != 0)
        return JNI_ERR;

    return JNI_OK;
}

extern "C" JNIEXPORT jint

    JNICALL
    JNI_OnLoad(JavaVM *vm, [[maybe_unused]] void *reserved)
{
    JNIEnv *env;
    vm->GetEnv((void **)&env, JNI_VERSION_1_6);
    if (RegisterMenu(env) != 0)
        return JNI_ERR;
    if (RegisterPreferences(env) != 0)
        return JNI_ERR;
    if (RegisterMain(env) != 0)
        return JNI_ERR;
    return JNI_VERSION_1_6;
}
