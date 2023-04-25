#!/bin/bash

# Run tests with a regular process on a container filesystem
function run_manual(){
    result_dir="../../../../../Results"    
    sudo mount -t overlay overlay -o lowerdir=lower,upperdir=upper,workdir=workdir merged

    cd merged
    
    ../../../C/create "new.txt" "$result_dir/Filesystem_Create_C_Manual.txt"

    cd ..

    sudo umount overlay
    rm upper/new.txt
}

# Prepare and run regular tests
function regular() {
    echo "Executing tests in regular mode"

    pushd Filesystem/Regular > /dev/null

    result_dir="../../../../Results"
    for i in {0..99}
    do
        ../../C/create "new.txt" "${result_dir}/Filesystem_Create_C_Regular.txt"
        rm new.txt
    done

    popd > /dev/null
}

# Prepare and run manual tests
function manual() {
    echo "Executing tests in manual mode"

    pushd Filesystem/Overlay > /dev/null

    for i in {0..99}
    do
        run_manual
    done

    popd > /dev/null
}

# Prepare and run container tests
function container() {

    pushd ../.. > /dev/null

    volume="type=bind,source=$(pwd)/Results,target=/Results"
    image="masterarbeit-filesystem-create-c"
    output_file="Results/Filesystem_Create_C_Container.txt"

    for i in {0..99}
    do
        docker run --name "$image" -it --rm --mount "$volume" "$image" ./create "new.txt" "$output_file"         
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