#!/bin/bash

if [ $# -ne 1 ]; then
    exit 1
fi

dir="$1"

if [ ! -d "$dir" ]; then
    exit 1
fi

for f in "$dir"/*; do
    finfo=$(file "$f")
    if [ -n "$(echo "$finfo" | grep -o 'ASCII')" ]; then
        echo "$finfo"
    fi
done

