#!/bin/bash

# Run tests with a regular process on a regular filesystem
function run_regular() {
    result_dir="../../../../Results"
    for i in {0..99}
    do
        # C version
        ../../C/read "dir/$1.txt" "${result_dir}/Filesystem_Read_C_Regular_$1.txt"

        # Java version
        java -cp "../../Java" Main "dir/$1.txt" "${result_dir}/Filesystem_Read_Java_Regular_$1.txt"
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

    # C version
    sudo mount -t overlay overlay -o lowerdir=${lowerdirs},upperdir=upper,workdir=workdir merged

    cd merged


    for i in {0..15}
    do
        ../../../C/read "dir/$i.txt" "$result_dir/Filesystem_Read_C_Manual_$i.txt"
    done

    cd ..

    sudo umount overlay

    # Java version
    sudo mount -t overlay overlay -o lowerdir=${lowerdirs},upperdir=upper,workdir=workdir merged
    cd merged


    for i in {0..15}
    do
        java -cp "../../../Java" Main "dir/$i.txt" "$result_dir/Filesystem_Read_Java_Manual_$i.txt"
    done

    cd ..

    sudo umount overlay
}

# Run tests in a container
function run_container() {
    volume="type=bind,source=$(pwd)/Results,target=/Results"
    c_image="masterarbeit-filesystem-read-c"
    java_image="masterarbeit-filesystem-read-java"
    for i in {0..15}
    do
        docker run --name "$c_image" -it --rm --mount "$volume" "$c_image" ./read "dir/$i.txt" "Results/Filesystem_Read_C_Container_$i.txt"
	    docker run --name "$java_image" -it --rm --mount "$volume" "$java_image" java Main "dir/$i.txt" "Results/Filesystem_Read_Java_Container_$i.txt"
    done
}

# Prepare and run regular tests
function regular() {
    echo "Executing tests in regular mode"

    pushd Filesystem/Regular > /dev/null

    for i in {0..15}
    do
	    echo "$((i + 1)) von 16"
        run_regular "$i"
    done 

    popd > /dev/null
}

# Prepare and run manual tests
function manual() {
    echo "Executing tests in manual mode"

    pushd Filesystem/Overlay > /dev/null

    for i in {0..99}
    do
	    echo "$((i + 1)) von 100"
        run_manual "$i"
    done

    popd > /dev/null
}

# Prepare and run container tests
function container() {
    echo "Executing tests in container mode"

    pushd ../.. > /dev/null

    for i in {0..99}
    do
	    echo "$((i + 1)) von 100"
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
