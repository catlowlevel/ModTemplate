#!/bin/bash

#what does this command do
set -xe  

cd ~/apktool
/bin/cp Android-Mod-Menu/app/build/intermediates/apk/debug/app-debug.apk menu-test.apk
rm menu-test -rf
apktool d menu-test.apk
/bin/cp menu-test/smali/com figg/smali_classes7/ -r
/bin/cp menu-test/lib/arm64-v8a/libMyLibName.so figg/lib/arm64-v8a/ -r

apktool b figg

adb push figg/dist/figg.apk /sdcard/
