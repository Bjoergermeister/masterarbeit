#!/bin/bash

jvm_args="-Xmx1024M -Xms1024M -XX:+UnlockExperimentalVMOptions -XX:+UseEpsilonGC"

# Prepare and run regular tests
function regular() {
    echo "Executing tests in regular mode"

    result_dir="../../Results"
    for i in {0..99}
    do
	    echo "$((i + 1)) von 100"
        C/read "${result_dir}/Memory_Read_C_Regular"
        java -cp Java/ $jvm_args Main "${result_dir}/Memory_Read_Java_Regular"
    done
}

# Prepare and run manual tests
function manual() {
    echo "Executing tests in manual mode"

    mkdir -p /sys/fs/cgroup/memory/user.slice/masterarbeit
    echo "$((1024 * 1024 * 100))" > /sys/fs/cgroup/memory/user.slice/masterarbeit/memory.limit_in_bytes
    echo -1 > /sys/fs/cgroup/memory/user.slice/masterarbeit/memory.memsw.limit_in_bytes
    echo $$ > /sys/fs/cgroup/memory/user.slice/masterarbeit/cgroup.procs

    result_dir="../../Results"
    for i in {0..99}
    do
	    echo "$((i + 1)) von 100"
        C/read "$result_dir/Memory_Read_C_Manual"
        java -cp Java $jvm_args Main "$result_dir/Memory_Read_Java_Manual"
    done

    rmdir /sys/fs/cgroup/memory/user.slice/masterarbeit
}

# Prepare and run container tests
function container() {
    volume="type=bind,source=$(pwd)/../../Results,target=/Results"
    c_image="masterarbeit-memory-read-c"
    java_image="masterarbeit-memory-read-java"
    c_result_file="Results/Memory_Read_C_Container"
    java_result_file="Results/Memory_Read_Java_Container"

    memory_limit=$((1024 * 1024 * 100));
    swap_limit=-1 #Must be set so that the containers have enough swap memory to run. If unset, memory and swap usage would be limited to 100MB each

    for i in {0..99}
    do
        echo "$((i + 1)) von 100"
        docker run --name "$c_image" --rm --mount "$volume" --memory "$memory_limit" --memory-swap="$swap_limit" "$c_image" ./read "$c_result_file"
        docker run --name "$java_image" --rm --mount "$volume" --memory "$memory_limit" --memory-swap="$swap_limit" "$java_image" java $jvm_args "Main" "$java_result_file"
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
