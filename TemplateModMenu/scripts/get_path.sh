#!/bin/bash

# adb shell "su -c 'find /data/app/ -maxdepth 3 -type d -iname \"$(./get_package.sh)*\" | head -n 1'"
adb shell pm path $(./get_package.sh $1) | cut --delimiter=':' -f 2 | sed 's%/[^/]*$%%'
