#!/bin/bash
adb shell monkey -p $(./get_package.sh) -c android.intent.category.LAUNCHER 1
