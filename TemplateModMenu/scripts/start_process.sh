#!/bin/bash
adb shell monkey -p $(./get_package.sh $1) -c android.intent.category.LAUNCHER 1
