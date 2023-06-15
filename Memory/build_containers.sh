#!/bin/bash

for folder in $(ls -d */)
do
    pushd $folder > /dev/null
    folder=${folder::-1}
    folder=${folder,,}
    
    sudo docker build -t masterarbeit-memory-$folder-c C
    sudo docker build -t masterarbeit-memory-$folder-java Java

    popd > /dev/null
done