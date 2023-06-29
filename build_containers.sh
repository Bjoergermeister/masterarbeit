#!/bin/bash

declare -a Categories=("Filesystem" "Memory" "Network")

for category in ${Categories[@]};
do
    cd $category
    for operation in $(ls -d */)
    do
        cd $operation
        category=${category,,}
        operation=${operation::-1}
        operation=${operation,,}

        if [ "$category" = "filesystem" ];
        then
            docker build -t masterarbeit-$category-$operation-c -f C/Dockerfile .
            docker build -t masterarbeit-$category-$operation-java -f Java/Dockerfile .    
        else
            docker build -t masterarbeit-$category-$operation-c C
            docker build -t masterarbeit-$category-$operation-java Java
        fi

        cd ..
    done 
    cd ..
done