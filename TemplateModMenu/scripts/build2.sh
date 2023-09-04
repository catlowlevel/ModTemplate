#!/bin/bash

#what does this command do
set -xe  

cd ~/apktool
/bin/cp ~/apktool/DropBlockModMenu/app/build/intermediates/apk/debug/app-debug.apk menu-drop.apk
rm menu-drop -rf
apktool d menu-drop.apk
/bin/cp ~/apktool/menu-drop/smali/com ~/apktool/dropblock3d/smali_classes4 -r
/bin/cp ~/apktool/menu-drop/lib/arm64-v8a/libMyLibName.so ~/apktool/dropblock3d/lib/arm64-v8a/ -r

apktool b ~/apktool/dropblock3d

adb push ~/apktool/dropblock3d/dist/dropblock3d.apk /sdcard/
