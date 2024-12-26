#!/bin/bash 
curdir=$(cd $(dirname $0); pwd)
cd $curdir


if [ ! -d "/config/workspace/" ]; then 
    mkdir -p /config/workspace/; 
fi 

ln -sf ${curdir} /config/workspace/hibot 
#  python3 -m pip install -r ./src/auxiliary/pymodules/requirements.txt
