#!/bin/bash

for folder in $(ls -d */)
do
    pushd $folder > /dev/null
    folder=${folder::-1}
    folder=${folder,,}
    
    docker build -t masterarbeit-filesystem-$folder-c -f Dockerfile-c .
    docker build -t masterarbeit-filesystem-$folder-java -f Dockerfile-java .

    popd > /dev/null
done