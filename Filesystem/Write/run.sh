#!/bin/bash

function reset_filesystem() {
    rm -rf Filesystem
    source ./create_filesystem.sh
}

# Prepare and run regular tests
function regular() {
    echo "Executing tests in regular mode"

    result_dir="../../../../Results"

    for i in {0..99}
    do
	echo "$i von 99"
        # C version
        pushd Filesystem/Regular > /dev/null

        ../../C/write "dir/1.txt" "${result_dir}/Filesystem_Write_C_Regular_1.txt"
        ../../C/write "dir/128.txt" "${result_dir}/Filesystem_Write_C_Regular_128.txt"
        ../../C/write "dir/256.txt" "${result_dir}/Filesystem_Write_C_Regular_256.txt"
        ../../C/write "dir/512.txt" "${result_dir}/Filesystem_Write_C_Regular_512.txt"
        ../../C/write "dir/1024.txt" "${result_dir}/Filesystem_Write_C_Regular_1024.txt"
        ../../C/write "dir/2048.txt" "${result_dir}/Filesystem_Write_C_Regular_2048.txt"

        popd > /dev/null

        reset_filesystem

        # Java version
        pushd Filesystem/Regular > /dev/null

        java -cp ../../Java Main "dir/1.txt" "${result_dir}/Filesystem_Write_Java_Regular_1.txt"
        java -cp ../../Java Main "dir/128.txt" "${result_dir}/Filesystem_Write_Java_Regular_128.txt"
        java -cp ../../Java Main "dir/256.txt" "${result_dir}/Filesystem_Write_Java_Regular_256.txt"
        java -cp ../../Java Main "dir/512.txt" "${result_dir}/Filesystem_Write_Java_Regular_512.txt"
        java -cp ../../Java Main "dir/1024.txt" "${result_dir}/Filesystem_Write_Java_Regular_1024.txt"
        java -cp ../../Java Main "dir/2048.txt" "${result_dir}/Filesystem_Write_Java_Regular_2048.txt"

        popd > /dev/null

        reset_filesystem
    done
}

# Prepare and run manual tests
function manual() {
    echo "Executing tests in manual mode"

    for i in {0..99}
    do
	echo "$i von 99"
        result_dir="../../../../Results"

        # C version
        cd Filesystem
        sudo mount -t overlay overlay -o lowerdir=Regular,upperdir=upper,workdir=workdir merged
        cd merged

        ../../C/write "dir/1.txt" "${result_dir}/Filesystem_Write_C_Manual_1.txt"
        ../../C/write "dir/128.txt" "${result_dir}/Filesystem_Write_C_Manual_128.txt"
        ../../C/write "dir/256.txt" "${result_dir}/Filesystem_Write_C_Manual_256.txt"
        ../../C/write "dir/512.txt" "${result_dir}/Filesystem_Write_C_Manual_512.txt"
        ../../C/write "dir/1024.txt" "${result_dir}/Filesystem_Write_C_Manual_1024.txt"
        ../../C/write "dir/2048.txt" "${result_dir}/Filesystem_Write_C_Manual_2048.txt"

        cd ../..
        sudo umount overlay
        rm Filesystem/upper/dir/*

        # Java version
        cd Filesystem
        sudo mount -t overlay overlay -o lowerdir=Regular,upperdir=upper,workdir=workdir merged
        cd merged

        java -cp "../../Java" Main "dir/1.txt" "${result_dir}/Filesystem_Write_Java_Manual_1.txt"
        java -cp "../../Java" Main "dir/128.txt" "${result_dir}/Filesystem_Write_Java_Manual_128.txt"
        java -cp "../../Java" Main "dir/256.txt" "${result_dir}/Filesystem_Write_Java_Manual_256.txt"
        java -cp "../../Java" Main "dir/512.txt" "${result_dir}/Filesystem_Write_Java_Manual_512.txt"
        java -cp "../../Java" Main "dir/1024.txt" "${result_dir}/Filesystem_Write_Java_Manual_1024.txt"
        java -cp "../../Java" Main "dir/2048.txt" "${result_dir}/Filesystem_Write_Java_Manual_2048.txt"

        cd ../..
        sudo umount overlay
        rm Filesystem/upper/dir/*
    done

}

# Prepare and run container tests
function container() {
    echo "Executing tests in container mode"

    pushd ../.. > /dev/null

    volume="type=bind,source=$(pwd)/Results,target=/Results"
    c_image="masterarbeit-filesystem-write-c"
    java_image="masterarbeit-filesystem-write-java"
    c_prefix="Results/Filesystem_Write_C_Container_"
    java_prefix="Results/Filesystem_Write_Java_Container_"

    for i in {0..99}
    do
	echo "$i von 99"
        # C container
        docker run --name "$c_image" -it --rm --mount "$volume" "$c_image" ./write "dir/1.txt" "${c_prefix}1.txt"
        docker run --name "$c_image" -it --rm --mount "$volume" "$c_image" ./write "dir/128.txt" "${c_prefix}128.txt"
        docker run --name "$c_image" -it --rm --mount "$volume" "$c_image" ./write "dir/256.txt" "${c_prefix}256.txt"
        docker run --name "$c_image" -it --rm --mount "$volume" "$c_image" ./write "dir/512.txt" "${c_prefix}512.txt"
        docker run --name "$c_image" -it --rm --mount "$volume" "$c_image" ./write "dir/1024.txt" "${c_prefix}1024.txt"
        docker run --name "$c_image" -it --rm --mount "$volume" "$c_image" ./write "dir/2048.txt" "${c_prefix}2048.txt"

        # Java container
        docker run --name "$java_image" -it --rm --mount "$volume" "$java_image" java Main "dir/1.txt" "${java_prefix}1.txt"
        docker run --name "$java_image" -it --rm --mount "$volume" "$java_image" java Main "dir/128.txt" "${java_prefix}128.txt"
        docker run --name "$java_image" -it --rm --mount "$volume" "$java_image" java Main "dir/256.txt" "${java_prefix}256.txt"
        docker run --name "$java_image" -it --rm --mount "$volume" "$java_image" java Main "dir/512.txt" "${java_prefix}512.txt"
        docker run --name "$java_image" -it --rm --mount "$volume" "$java_image" java Main "dir/1024.txt" "${java_prefix}1024.txt"
        docker run --name "$java_image" -it --rm --mount "$volume" "$java_image" java Main "dir/2048.txt" "${java_prefix}2048.txt"
    done

    popd > /dev/null
}

# Prepare and run container tests in privileged mode
function privileged() {
    echo "Executing tests in container mode"

    pushd ../.. > /dev/null

    volume="type=bind,source=$(pwd)/Results,target=/Results"
    c_image="masterarbeit-filesystem-write-c"
    java_image="masterarbeit-filesystem-write-java"
    c_prefix="Results/Filesystem_Write_C_Privileged_"
    java_prefix="Results/Filesystem_Write_Java_Privileged_"

    for i in {0..99}
    do
	echo "$i von 99"
        # C container
        docker run --name "$c_image" -it --rm --privileged --mount "$volume" "$c_image" ./write "dir/1.txt" "${c_prefix}1.txt"
        docker run --name "$c_image" -it --rm --privileged --mount "$volume" "$c_image" ./write "dir/128.txt" "${c_prefix}128.txt"
        docker run --name "$c_image" -it --rm --privileged --mount "$volume" "$c_image" ./write "dir/256.txt" "${c_prefix}256.txt"
        docker run --name "$c_image" -it --rm --privileged --mount "$volume" "$c_image" ./write "dir/512.txt" "${c_prefix}512.txt"
        docker run --name "$c_image" -it --rm --privileged --mount "$volume" "$c_image" ./write "dir/1024.txt" "${c_prefix}1024.txt"
        docker run --name "$c_image" -it --rm --privileged --mount "$volume" "$c_image" ./write "dir/2048.txt" "${c_prefix}2048.txt"

        # Java container
        docker run --name "$java_image" -it --rm --privileged --mount "$volume" "$java_image" java Main "dir/1.txt" "${java_prefix}1.txt"
        docker run --name "$java_image" -it --rm --privileged --mount "$volume" "$java_image" java Main "dir/128.txt" "${java_prefix}128.txt"
        docker run --name "$java_image" -it --rm --privileged --mount "$volume" "$java_image" java Main "dir/256.txt" "${java_prefix}256.txt"
        docker run --name "$java_image" -it --rm --privileged --mount "$volume" "$java_image" java Main "dir/512.txt" "${java_prefix}512.txt"
        docker run --name "$java_image" -it --rm --privileged --mount "$volume" "$java_image" java Main "dir/1024.txt" "${java_prefix}1024.txt"
        docker run --name "$java_image" -it --rm --privileged --mount "$volume" "$java_image" java Main "dir/2048.txt" "${java_prefix}2048.txt"
    done

    popd > /dev/null
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