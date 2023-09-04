#!/bin/bash
inotifywait --event modify $(./get_file.sh)
