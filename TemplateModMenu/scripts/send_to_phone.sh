#!/bin/bash

set -xe

./stop_process.sh
adb push $(./get_file.sh) /sdcard/
adb shell "su -c 'mv /sdcard/libMyLibName.so $(./get_path.sh)/lib/arm64'"
sleep 1
./start_process.sh
