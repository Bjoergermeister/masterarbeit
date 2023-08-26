#!/bin/bash

jvm_args="-Xmx1024M -Xms1024M -XX:+UnlockExperimentalVMOptions -XX:+UseEpsilonGC"

# Prepare and run regular tests
function regular() {
    echo "Executing tests in regular mode"

    result_dir="../../Results"
    for i in {0..99}
    do
	    echo "$((i + 1)) von 100"
        C/write "../ipc/memory"
        #C/write "${result_dir}/Memory_Write_C_Regular"
        #java -cp Java/ $jvm_args  Main "${result_dir}/Memory_Write_Java_Regular"
    done
}

# Prepare and run manual tests
function manual() {
    echo "Executing tests in manual mode"

    mkdir -p /sys/fs/cgroup/memory/user.slice/masterarbeit
    echo "$((1024 * 1024 * 100))" > /sys/fs/cgroup/memory/user.slice/masterarbeit/memory.limit_in_bytes
    echo "$((1024 * 1024 * 200))" > /sys/fs/cgroup/memory/user.slice/masterarbeit/memory.memsw.limit_in_bytes
    echo $$ > /sys/fs/cgroup/memory/user.slice/masterarbeit/cgroup.procs

    result_dir="../../Results"
    for i in {0..99}
    do
	    echo "$((i + 1)) von 100"
        C/write "../ipc/memory"
        #C/write "$result_dir/Memory_Write_C_Manual"
        #java -cp Java $jvm_args Main "$result_dir/Memory_Write_Java_Manual"
    done

    rmdir /sys/fs/cgroup/memory/user.slice/masterarbeit
}

# Prepare and run container tests
function container() {
    echo "Executing tests in container mode"

    c_volume="type=bind,source=$(pwd)/../ipc,target=/ipc"
    c_image="masterarbeit-memory-write-c"
    c_result_file="/ipc/memory"

    java_volume="type=bind,source=$(pwd)/../../Results,target=/Results"    
    java_image="masterarbeit-memory-write-java"
    java_result_file="Results/Memory_Write_Java_Container"

    memory_limit=$((1024 * 1024 * 100));
    swap_limit=$((1024 * 1024 * 100 * 2));

    for i in {0..99}
    do
        echo "$((i + 1)) von 100"
        docker run --name "$c_image" --rm --mount "$c_volume" --ipc=host --memory "$memory_limit" --memory-swap "$swap_limit" "$c_image" ./write "$c_result_file"
        #docker run --name "$java_image" --rm --mount "$java_volume" --memory "$memory_limit" --memory-swap "$swap_limit" "$java_image" java $jvm_args "Main" "$java_result_file"
    done
}

# Prepare and run privileged tests
function privileged() {
    echo "Executing tests in privileged mode"

    c_volume="type=bind,source=$(pwd)/../ipc,target=/ipc"
    c_image="masterarbeit-memory-write-c"
    c_result_file="/ipc/memory"

    java_volume="type=bind,source=$(pwd)/../../Results,target=/Results"
    java_image="masterarbeit-memory-write-java"
    java_result_file="Results/Memory_Write_Java_Privileged"

    memory_limit=$((1024 * 1024 * 100));
    swap_limit=$((1024 * 1024 * 100 * 2));

    for i in {0..99}
    do
        echo "$((i + 1)) von 100"
        docker run --name "$c_image" --rm --privileged --mount "$c_volume" --ipc=host --memory "$memory_limit" --memory-swap "$swap_limit" "$c_image" ./write "$c_result_file"
        #docker run --name "$java_image" --rm --privileged --mount "$java_volume" --memory "$memory_limit" --memory-swap "$swap_limit" "$java_image" java $jvm_args "Main" "$java_result_file"
    done
}

# Prepare and execute tests

if [ "$1" = "regular"  ] || [ "$1" = "" ] 
then
    regular
fi

if [ "$1" = "manual"  ] || [ "$1" = "" ] 
then
    manual
fi

if [ "$1" = "container"  ] || [ "$1" = "" ] 
then
    container
fi

if [ "$1" = "privileged"  ] || [ "$1" = "" ] 
then
    privileged
fi
