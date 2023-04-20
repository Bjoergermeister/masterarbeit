#!/bin/bash

# Run tests with a regular process on a regular filesystem
function run_regular() {
    result_dir="../../../../Results"
    for i in {0..99}
    do
        ../../C/open "dir/$1.txt" "${result_dir}/Filesystem_Open_C_Regular_$1.txt"
    done
}

# Run tests with a regular process on a container filesystem
function run_manual(){
    result_dir="../../../../../Results"

    # Build lowerdir list
    lowerdirs="Layer0"
    for i in {1..15}
    do
        lowerdirs="$lowerdirs:Layer$i"
    done
    
    sudo mount -t overlay overlay -o lowerdir=${lowerdirs},upperdir=upper,workdir=workdir merged

    cd merged


    for i in {0..15}
    do
        ../../../C/open "dir/$i.txt" "$result_dir/Filesystem_Open_C_Manual_$i.txt"
    done

    cd ..

    sudo umount overlay
}

# Run tests in a container
function run_container() {
    volume="type=bind,source=$(pwd)/Results,target=/Results"
    image="masterarbeit-filesystem-open-c"
    for i in {0..15}
    do
        #echo "docker run --name $image --rm --mount $volume $image ./open dir/$i /Results/Filesystem_Open_C_Container_$i.txt"
        docker run --name "$image" -it --rm --mount "$volume" "$image" ./open "dir/$i.txt" "Results/Filesystem_Open_C_Container_$i.txt"         
    done
}

# Prepare and run regular tests
function regular() {
    echo "Executing tests in regular mode"

    pushd Filesystem/Regular > /dev/null

    for i in {0..15}
    do
        run_regular "$i"
    done 

    popd > /dev/null
}

# Prepare and run manual tests
function manual() {
    echo "Executing tests in manual mode"

    pushd Filesystem/Overlay > /dev/null

    for i in {0..100}
    do
        run_manual "$i"
    done

    popd > /dev/null
}

# Prepare and run container tests
function container() {

    pushd ../.. > /dev/null

    for i in {0..100}
    do
        run_container 
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