#!/bin/bash

# Prepare and run regular tests
function regular() {
    echo "Executing tests in regular mode"

    result_dir="../../Results"
    for i in {0..99}
    do
	    echo "$((i + 1)) von 100"

        # C version
        C/allocation "${result_dir}/Memory_Allocation_C_Regular"

        # Java version
        java -cp Java/ Main "${result_dir}/Memory_Allocation_Java_Regular"
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

        # C version
        C/allocation "$result_dir/Memory_Allocation_C_Manual"

        # Java version
        java -cp Java Main "$result_dir/Memory_Allocation_Java_Manual"
    done

    mdir /sys/fs/cgroup/memory/user.slice/masterarbeit
}

# Prepare and run container tests
function container() {
    volume="type=bind,source=$(pwd)/../../Results,target=/Results"
    c_image="masterarbeit-memory-allocation-c"
    java_image="masterarbeit-memory-allocation-java"

    memory_limit=$((1024 * 1024 * 100));
    swap_limit=$((1024 * 1024 * 100 * 2));

    for i in {0..99}
    do
        echo "$((i + 1)) von 100"

        c_result_file="Results/Memory_Allocation_C_Container"
        java_result_file="Results/Memory_Allocation_Java_Container"
        docker run --name "$c_image" --rm --mount "$volume" --memory "$memory_limit" --memory-swap "$swap_limit" "$c_image" ./allocation "$c_result_file"
        docker run --name "$java_image" --rm --mount "$volume" --memory "$memory_limit" --memory-swap "$swap_limit" "$java_image" java "-Xms1024M" "-Xmx1024M" "Main" "$java_result_file"
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
