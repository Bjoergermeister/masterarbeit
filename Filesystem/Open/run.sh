#!/bin/bash

# Run tests with a regular process on a regular filesystem
function run_regular() {

    result_dir="../../../../Results"
    for i in {0..99}
    do
        ../../C/open "dir/$1.txt" "${result_dir}/Filesystem_Open_C_Regular_$1.txt"
        java -cp ../../Java/ Main "dir/$1.txt" "${result_dir}/Filesystem_Open_Java_Regular_$1.txt"
    done
}

# Run tests with a regular process on a container filesystem
function run_manual(){
    result_dir="../../../../../Results"
    lowerdirs="Layer0:Layer1:Layer2:Layer3:Layer4:Layer5:Layer6:Layer7:Layer8:Layer9:Layer10:Layer11:Layer12:Layer13:Layer14:Layer015"

    sudo mount -t overlay overlay -o lowerdir=${lowerdirs},upperdir=upper,workdir=workdir merged
    cd merged

    for i in {0..15}
    do
        ../../../C/open "dir/$i.txt" "$result_dir/Filesystem_Open_C_Manual_$i.txt"
        java -cp ../../../Java Main "dir/$i.txt" "$result_dir/Filesystem_Open_Java_Manual_$i.txt"
    done

    cd ..
    sudo umount overlay
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

    pushd ../.. > /dev/null

    for iteration in {0..99}
    do
	    echo "$((iteration + 1)) von 100"
    	volume="type=bind,source=$(pwd)/Results,target=/Results"
        c_image="masterarbeit-filesystem-open-c"
        java_image="masterarbeit-filesystem-open-java"
        for i in {0..15}
        do
            c_result_file="Results/Filesystem_Open_C_Container_$i.txt"
            java_result_file="Results/Filesystem_Open_Java_Container_$i.txt"
            docker run --name "$c_image" --rm --mount "$volume" "$c_image" ./open "dir/$i.txt" "$c_result_file"
            docker run --name "$java_image" --rm --mount "$volume" "$java_image" java "Main" "dir/$i.txt" "$java_result_file"
        done
    done

    popd > /dev/null
}

# Prepare and run container tests in privileged mode
function privileged() {

    pushd ../.. > /dev/null

    for iteration in {0..99}
    do
	    echo "$((iteration + 1)) von 100"
    	volume="type=bind,source=$(pwd)/Results,target=/Results"
        c_image="masterarbeit-filesystem-open-c"
        java_image="masterarbeit-filesystem-open-java"
        for i in {0..15}
        do
            c_result_file="Results/Filesystem_Open_C_Privileged_$i.txt"
            java_result_file="Results/Filesystem_Open_Java_Privileged_$i.txt"
            docker run --name "$c_image" --rm --privileged --mount "$volume" "$c_image" ./open "dir/$i.txt" "$c_result_file"
            docker run --name "$java_image" --rm --privileged --mount "$volume" "$java_image" java "Main" "dir/$i.txt" "$java_result_file"
        done
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