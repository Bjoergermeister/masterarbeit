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
        
        pushd Filesystem/Regular > /dev/null
        
        ../../C/delete "dir/1.txt" "${result_dir}/Filesystem_Delete_C_Regular_1.txt"
        ../../C/delete "dir/128.txt" "${result_dir}/Filesystem_Delete_C_Regular_128.txt"
        ../../C/delete "dir/256.txt" "${result_dir}/Filesystem_Delete_C_Regular_256.txt"
        ../../C/delete "dir/512.txt" "${result_dir}/Filesystem_Delete_C_Regular_512.txt"
        ../../C/delete "dir/1024.txt" "${result_dir}/Filesystem_Delete_C_Regular_1024.txt"
        ../../C/delete "dir/2048.txt" "${result_dir}/Filesystem_Delete_C_Regular_2048.txt"
        
        popd > /dev/null
        reset_filesystem
    done
}

# Prepare and run manual tests
function manual() {
    echo "Executing tests in manual mode"

    for i in {0..99}
    do
        result_dir="../../../../Results"
        reset_filesystem

        cd Filesystem
        sudo mount -t overlay overlay -o lowerdir=Regular,upperdir=upper,workdir=workdir merged
        cd merged
    
        ../../C/delete "dir/1.txt" "${result_dir}/Filesystem_Delete_C_Manual_1.txt"
        ../../C/delete "dir/128.txt" "${result_dir}/Filesystem_Delete_C_Manual_128.txt"
        ../../C/delete "dir/256.txt" "${result_dir}/Filesystem_Delete_C_Manual_256.txt"
        ../../C/delete "dir/512.txt" "${result_dir}/Filesystem_Delete_C_Manual_512.txt"
        ../../C/delete "dir/1024.txt" "${result_dir}/Filesystem_Delete_C_Manual_1024.txt"
        ../../C/delete "dir/2048.txt" "${result_dir}/Filesystem_Delete_C_Manual_2048.txt"
    
        cd ../..
        sudo umount overlay
        rm -rf Filesystem/upper/dir/*
    done

}

# Prepare and run container tests
function container() {
    echo "Executing tests in container mode"

    pushd ../.. > /dev/null

    volume="type=bind,source=$(pwd)/Results,target=/Results"
    image="masterarbeit-filesystem-delete-c"
    output_file_prefix="Results/Filesystem_Delete_C_Container_"
    
    for i in {0..99}
    do
        docker run --name "$image" -it --rm --mount "$volume" "$image" ./delete "dir/1.txt" "${output_file_prefix}1.txt"
        docker run --name "$image" -it --rm --mount "$volume" "$image" ./delete "dir/128.txt" "${output_file_prefix}128.txt"
        docker run --name "$image" -it --rm --mount "$volume" "$image" ./delete "dir/256.txt" "${output_file_prefix}256.txt"
        docker run --name "$image" -it --rm --mount "$volume" "$image" ./delete "dir/512.txt" "${output_file_prefix}512.txt"
        docker run --name "$image" -it --rm --mount "$volume" "$image" ./delete "dir/1024.txt" "${output_file_prefix}1024.txt"
        docker run --name "$image" -it --rm --mount "$volume" "$image" ./delete "dir/2048.txt" "${output_file_prefix}2048.txt"
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