#!/bin/sh

facto(){
        res=1
        k=1
        while [ "$k" -le "$1"  ];do
            res=$((res*k))
            k=$((k+1))
        done
}

if [ $# -ne 1 ]; then
	exit 1
else
    facto $1
    echo "$res"
fi
